// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "GridManager.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

#if WITH_EDITORONLY_DATA
#include "Materials/Material.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#endif

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	Width = 10;
	Height = 10;
	TileSize = 100.0f;
	CachedBaseMaterial = nullptr;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// Cache mesh references
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		CachedCubeMesh = CubeFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeFinder(
		TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeFinder.Succeeded())
	{
		CachedConeMesh = ConeFinder.Object;
	}
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	CreateBaseMaterial();
	InitializeGrid();
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridManager::CreateBaseMaterial()
{
#if WITH_EDITORONLY_DATA
	UMaterial* Mat = NewObject<UMaterial>(GetTransientPackage(), NAME_None, RF_Transient);
	Mat->MaterialDomain = MD_Surface;

	UMaterialExpressionVectorParameter* ColorParam =
		NewObject<UMaterialExpressionVectorParameter>(Mat);
	ColorParam->ParameterName = TEXT("Color");
	ColorParam->DefaultValue = FLinearColor::White;

	UMaterialEditorOnlyData* EditorData = Mat->GetEditorOnlyData();
	EditorData->ExpressionCollection.Expressions.Add(ColorParam);
	EditorData->BaseColor.Expression = ColorParam;
	EditorData->BaseColor.OutputIndex = 0;

	Mat->PreEditChange(nullptr);
	Mat->PostEditChange();

	CachedBaseMaterial = Mat;
	UE_LOG(LogTemp, Log, TEXT("GridManager: Created custom colored material"));
#else
	// Fallback: use BasicShapeMaterial (will be white, no color support)
	CachedBaseMaterial = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	UE_LOG(LogTemp, Warning, TEXT("GridManager: Using fallback BasicShapeMaterial (no color support)"));
#endif
}

void AGridManager::InitializeGrid()
{
	GridMap.Empty();
	for (int32 x = 0; x < Width; ++x)
	{
		for (int32 y = 0; y < Height; ++y)
		{
			FTileData NewData;
			NewData.TileType = ETileType::Normal;
			GridMap.Add(FIntVector(x, y, 0), NewData);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GridManager: Initialized %d tiles, CubeMesh=%s, Material=%s"),
		GridMap.Num(),
		CachedCubeMesh ? TEXT("OK") : TEXT("NULL"),
		CachedBaseMaterial ? TEXT("OK") : TEXT("NULL"));

	RefreshAllTileVisuals();
}

FVector AGridManager::GridToWorld(FIntVector Coords) const
{
	FVector Origin = GetActorLocation();
	return Origin + FVector(Coords.X * TileSize, Coords.Y * TileSize, 0.0f);
}

FIntVector AGridManager::WorldToGrid(FVector Location) const
{
	FVector RelativeLoc = Location - GetActorLocation();
	int32 x = FMath::RoundToInt(RelativeLoc.X / TileSize);
	int32 y = FMath::RoundToInt(RelativeLoc.Y / TileSize);
	return FIntVector(x, y, 0);
}

ETileType AGridManager::GetTileType(FIntVector Coords) const
{
	if (GridMap.Contains(Coords))
	{
		return GridMap[Coords].TileType;
	}
	return ETileType::Pit; // Outside grid is considered a pit
}

bool AGridManager::IsInBounds(int32 X, int32 Y) const
{
	return X >= 0 && X < Width && Y >= 0 && Y < Height;
}

bool AGridManager::IsValidTile(FIntVector Coords) const
{
	// Robots can enter any tile within bounds (pits kill via ProcessTileEffects)
	return IsInBounds(Coords.X, Coords.Y);
}

FTileData AGridManager::GetTileData(FIntVector Coords) const
{
	if (const FTileData* Data = GridMap.Find(Coords))
	{
		return *Data;
	}
	FTileData PitData;
	PitData.TileType = ETileType::Pit;
	return PitData;
}

int32 AGridManager::GetTotalCheckpoints() const
{
	int32 Count = 0;
	for (const auto& Pair : GridMap)
	{
		if (Pair.Value.TileType == ETileType::Checkpoint)
		{
			Count++;
		}
	}
	return Count;
}

void AGridManager::SetTileType(FIntVector Coords, const FTileData& Data)
{
	GridMap.Add(Coords, Data);

	// Update or create the visual mesh for this tile
	if (UStaticMeshComponent** Existing = TileMeshes.Find(Coords))
	{
		// Update material color
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>((*Existing)->GetMaterial(0));
		if (MID)
		{
			MID->SetVectorParameterValue(TEXT("Color"), GetTileColor(Data.TileType));
		}

		// Adjust Z for pits
		FVector Loc = (*Existing)->GetRelativeLocation();
		Loc.Z = (Data.TileType == ETileType::Pit) ? -5.0f : 0.0f;
		(*Existing)->SetRelativeLocation(Loc);
	}
	else
	{
		SpawnTileMesh(Coords, Data);
	}

	// Handle conveyor arrow
	DestroyConveyorArrow(Coords);
	if (IsConveyor(Data.TileType))
	{
		SpawnConveyorArrow(Coords, Data.TileType);
	}
}

void AGridManager::RefreshAllTileVisuals()
{
	// Destroy existing tile meshes
	for (auto& Pair : TileMeshes)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	TileMeshes.Empty();

	// Destroy existing arrow meshes
	for (auto& Pair : ArrowMeshes)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	ArrowMeshes.Empty();

	// Spawn new meshes for all tiles
	for (auto& Pair : GridMap)
	{
		SpawnTileMesh(Pair.Key, Pair.Value);
		if (IsConveyor(Pair.Value.TileType))
		{
			SpawnConveyorArrow(Pair.Key, Pair.Value.TileType);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("GridManager: Spawned %d tile meshes"), TileMeshes.Num());
}

void AGridManager::SpawnTileMesh(FIntVector Coords, const FTileData& Data)
{
	// Determine which mesh to use: per-type > general custom > engine fallback
	UStaticMesh* MeshToUse = nullptr;

	if (UStaticMesh** TypeMesh = TileTypeMeshes.Find(Data.TileType))
	{
		MeshToUse = *TypeMesh;
	}
	else if (TileMeshAsset)
	{
		MeshToUse = TileMeshAsset;
	}
	else
	{
		MeshToUse = CachedCubeMesh;
	}

	if (!MeshToUse)
	{
		UE_LOG(LogTemp, Error, TEXT("GridManager: No mesh available for tile, cannot spawn"));
		return;
	}

	FString CompName = FString::Printf(TEXT("Tile_%d_%d"), Coords.X, Coords.Y);
	UStaticMeshComponent* TileMesh = NewObject<UStaticMeshComponent>(this, FName(*CompName));
	TileMesh->SetupAttachment(RootComponent);
	TileMesh->SetStaticMesh(MeshToUse);

	// Position relative to grid origin
	FVector TilePos(Coords.X * TileSize, Coords.Y * TileSize, 0.0f);
	if (Data.TileType == ETileType::Pit)
	{
		TilePos.Z = -5.0f;
	}
	TileMesh->SetRelativeLocation(TilePos);

	// Scale: Cube default is 100x100x100, scale to tile size with small gap
	float TileScale = TileSize / 100.0f * 0.96f;
	TileMesh->SetRelativeScale3D(FVector(TileScale, TileScale, 0.05f));

	// Create colored material
	if (CachedBaseMaterial)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(CachedBaseMaterial, this);
		MID->SetVectorParameterValue(TEXT("Color"), GetTileColor(Data.TileType));
		TileMesh->SetMaterial(0, MID);
	}

	TileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TileMesh->RegisterComponent();

	TileMeshes.Add(Coords, TileMesh);
}

void AGridManager::SpawnConveyorArrow(FIntVector Coords, ETileType Type)
{
	if (!CachedConeMesh || !CachedBaseMaterial) return;

	FString CompName = FString::Printf(TEXT("Arrow_%d_%d"), Coords.X, Coords.Y);
	UStaticMeshComponent* Arrow = NewObject<UStaticMeshComponent>(this, FName(*CompName));
	Arrow->SetupAttachment(RootComponent);
	Arrow->SetStaticMesh(CachedConeMesh);

	// Position on top of tile
	FVector ArrowPos(Coords.X * TileSize, Coords.Y * TileSize, 4.0f);
	Arrow->SetRelativeLocation(ArrowPos);

	// Tilt cone to lay flat (pitch -90), then yaw for direction
	float Yaw = GetConveyorYaw(Type);
	Arrow->SetRelativeRotation(FRotator(-90.0f, Yaw, 0.0f));

	// Scale small
	Arrow->SetRelativeScale3D(FVector(0.18f, 0.18f, 0.25f));

	// Bright arrow color
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(CachedBaseMaterial, this);
	MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.9f, 0.95f, 1.0f));
	Arrow->SetMaterial(0, MID);

	Arrow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Arrow->RegisterComponent();

	ArrowMeshes.Add(Coords, Arrow);
}

void AGridManager::DestroyConveyorArrow(FIntVector Coords)
{
	if (UStaticMeshComponent** Existing = ArrowMeshes.Find(Coords))
	{
		if (*Existing)
		{
			(*Existing)->DestroyComponent();
		}
		ArrowMeshes.Remove(Coords);
	}
}

bool AGridManager::IsConveyor(ETileType Type)
{
	return Type == ETileType::ConveyorNorth
		|| Type == ETileType::ConveyorSouth
		|| Type == ETileType::ConveyorEast
		|| Type == ETileType::ConveyorWest;
}

float AGridManager::GetConveyorYaw(ETileType Type)
{
	switch (Type)
	{
	case ETileType::ConveyorNorth: return 0.0f;    // +X
	case ETileType::ConveyorEast:  return 90.0f;   // +Y
	case ETileType::ConveyorSouth: return 180.0f;  // -X
	case ETileType::ConveyorWest:  return 270.0f;  // -Y
	default:                       return 0.0f;
	}
}

FLinearColor AGridManager::GetTileColor(ETileType Type)
{
	switch (Type)
	{
	case ETileType::Normal:        return FLinearColor(0.2f, 0.6f, 0.2f);
	case ETileType::Pit:           return FLinearColor(0.15f, 0.05f, 0.05f);
	case ETileType::ConveyorNorth: return FLinearColor(0.1f, 0.5f, 0.7f);
	case ETileType::ConveyorSouth: return FLinearColor(0.1f, 0.5f, 0.7f);
	case ETileType::ConveyorEast:  return FLinearColor(0.1f, 0.5f, 0.7f);
	case ETileType::ConveyorWest:  return FLinearColor(0.1f, 0.5f, 0.7f);
	case ETileType::Laser:         return FLinearColor(0.8f, 0.4f, 0.1f);
	case ETileType::Checkpoint:    return FLinearColor(0.8f, 0.7f, 0.1f);
	default:                       return FLinearColor::White;
	}
}
