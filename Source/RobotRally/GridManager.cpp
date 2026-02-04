// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "GridManager.h"
#include "DrawDebugHelpers.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
	Width = 10;
	Height = 10;
	TileSize = 100.0f;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeGrid();
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if ENABLE_DRAW_DEBUG
	if (bDrawDebugGrid)
	{
		DrawDebugGrid();
	}
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
	if (!IsInBounds(Coords.X, Coords.Y))
	{
		return false;
	}
	ETileType Type = GetTileType(Coords);
	return Type != ETileType::Pit;
}

void AGridManager::DrawDebugGrid()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float HalfTile = TileSize * 0.5f;
	const float DrawZ = GetActorLocation().Z + 1.0f;

	for (auto& Pair : GridMap)
	{
		FIntVector Coords = Pair.Key;
		FTileData& Data = Pair.Value;

		FVector Center = GridToWorld(Coords);
		Center.Z = DrawZ;

		// Color based on tile type
		FColor TileColor;
		switch (Data.TileType)
		{
		case ETileType::Normal:        TileColor = FColor::Green;   break;
		case ETileType::Pit:           TileColor = FColor::Red;     break;
		case ETileType::ConveyorNorth: TileColor = FColor::Cyan;    break;
		case ETileType::ConveyorSouth: TileColor = FColor::Cyan;    break;
		case ETileType::ConveyorEast:  TileColor = FColor::Cyan;    break;
		case ETileType::ConveyorWest:  TileColor = FColor::Cyan;    break;
		case ETileType::Laser:         TileColor = FColor::Orange;  break;
		case ETileType::Checkpoint:    TileColor = FColor::Yellow;  break;
		default:                       TileColor = FColor::White;   break;
		}

		// Box outline for each tile (slightly inset so adjacent tiles don't overlap)
		FVector BoxExtent(HalfTile - 2.0f, HalfTile - 2.0f, 1.0f);
		DrawDebugBox(World, Center, BoxExtent, TileColor, false, -1.0f, 0, 2.0f);

		// Coordinate text
		FString CoordText = FString::Printf(TEXT("(%d,%d)"), Coords.X, Coords.Y);
		DrawDebugString(World, Center + FVector(0, 0, 10.0f), CoordText, nullptr, FColor::White, 0.0f, true);

		// Directional arrow for conveyors
		if (Data.TileType == ETileType::ConveyorNorth ||
			Data.TileType == ETileType::ConveyorSouth ||
			Data.TileType == ETileType::ConveyorEast ||
			Data.TileType == ETileType::ConveyorWest)
		{
			FVector ArrowEnd = Center;
			switch (Data.TileType)
			{
			case ETileType::ConveyorNorth: ArrowEnd += FVector(HalfTile * 0.8f, 0, 0); break;
			case ETileType::ConveyorSouth: ArrowEnd -= FVector(HalfTile * 0.8f, 0, 0); break;
			case ETileType::ConveyorEast:  ArrowEnd += FVector(0, HalfTile * 0.8f, 0); break;
			case ETileType::ConveyorWest:  ArrowEnd -= FVector(0, HalfTile * 0.8f, 0); break;
			default: break;
			}
			DrawDebugDirectionalArrow(World, Center, ArrowEnd, 20.0f, FColor::Blue, false, -1.0f, 0, 3.0f);
		}

		// Checkpoint number
		if (Data.TileType == ETileType::Checkpoint && Data.CheckpointNumber > 0)
		{
			FString CPText = FString::Printf(TEXT("CP:%d"), Data.CheckpointNumber);
			DrawDebugString(World, Center + FVector(0, 0, 25.0f), CPText, nullptr, FColor::Yellow, 0.0f, true);
		}
	}

	// Grid boundary outline
	FVector Origin = GetActorLocation();
	Origin.Z = DrawZ;
	FVector GridCenter = Origin + FVector(Width * TileSize * 0.5f - TileSize * 0.5f,
		Height * TileSize * 0.5f - TileSize * 0.5f, 0.0f);
	FVector GridExtent(Width * TileSize * 0.5f, Height * TileSize * 0.5f, 2.0f);
	DrawDebugBox(World, GridCenter, GridExtent, FColor::White, false, -1.0f, 0, 3.0f);
}
