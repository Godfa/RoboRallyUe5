// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "GridManager.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Normal,
	Pit,
	ConveyorNorth,
	ConveyorSouth,
	ConveyorEast,
	ConveyorWest,
	Laser,
	Checkpoint
};

USTRUCT(BlueprintType)
struct FTileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileType = ETileType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CheckpointNumber = 0;
};

UCLASS()
class ROBOTRALLY_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Grid dimensions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Width = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Height = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float TileSize = 100.0f;

	// Customizable tile mesh (falls back to engine cube if not set)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid|Meshes")
	UStaticMesh* TileMeshAsset;

	// Optional per-tile-type meshes (if set, overrides TileMeshAsset for that type)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid|Meshes")
	TMap<ETileType, UStaticMesh*> TileTypeMeshes;

	UPROPERTY(VisibleAnywhere, Category = "Grid")
	TMap<FIntVector, FTileData> GridMap;

	// Convert grid coordinates to world location
	UFUNCTION(BlueprintPure, Category = "Grid")
	FVector GridToWorld(FIntVector Coords) const;

	// Convert world location to grid coordinates
	UFUNCTION(BlueprintPure, Category = "Grid")
	FIntVector WorldToGrid(FVector Location) const;

	// Check what's on a specific tile
	UFUNCTION(BlueprintCallable, Category = "Grid")
	ETileType GetTileType(FIntVector Coords) const;

	// Returns true if coordinates are within grid boundaries
	UFUNCTION(BlueprintPure, Category = "Grid")
	bool IsInBounds(int32 X, int32 Y) const;

	// Returns true if tile is within bounds and not a Pit
	UFUNCTION(BlueprintPure, Category = "Grid")
	bool IsValidTile(FIntVector Coords) const;

	// Get full tile data (type + checkpoint number etc.)
	UFUNCTION(BlueprintPure, Category = "Grid")
	FTileData GetTileData(FIntVector Coords) const;

	// Get total number of checkpoints on the board
	UFUNCTION(BlueprintPure, Category = "Grid")
	int32 GetTotalCheckpoints() const;

	// Set tile type and update visual
	UFUNCTION(BlueprintCallable, Category = "Grid")
	void SetTileType(FIntVector Coords, const FTileData& Data);

	// Refresh all tile visuals from current GridMap state
	void RefreshAllTileVisuals();

	// Get color for a tile type
	static FLinearColor GetTileColor(ETileType Type);

	// Returns true if tile type is a conveyor
	static bool IsConveyor(ETileType Type);

	// Get yaw rotation for conveyor arrow
	static float GetConveyorYaw(ETileType Type);

private:
	void InitializeGrid();
	void SpawnTileMesh(FIntVector Coords, const FTileData& Data);
	void SpawnConveyorArrow(FIntVector Coords, ETileType Type);
	void DestroyConveyorArrow(FIntVector Coords);
	void CreateBaseMaterial();

	UPROPERTY()
	TMap<FIntVector, UStaticMeshComponent*> TileMeshes;

	UPROPERTY()
	TMap<FIntVector, UStaticMeshComponent*> ArrowMeshes;

	UPROPERTY()
	USceneComponent* SceneRoot;

	UPROPERTY()
	UStaticMesh* CachedCubeMesh;

	UPROPERTY()
	UStaticMesh* CachedConeMesh;

	UPROPERTY()
	UMaterialInterface* CachedBaseMaterial;
};
