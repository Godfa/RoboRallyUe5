// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "GridManager.generated.h"

// Forward declare EGridDirection (defined in RobotMovementComponent.h)
enum class EGridDirection : uint8;

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

// Wall bit flags for FTileData
static constexpr uint8 WALL_NORTH = 1 << 0;  // bit 0
static constexpr uint8 WALL_EAST  = 1 << 1;  // bit 1
static constexpr uint8 WALL_SOUTH = 1 << 2;  // bit 2
static constexpr uint8 WALL_WEST  = 1 << 3;  // bit 3

USTRUCT(BlueprintType)
struct FTileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileType = ETileType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CheckpointNumber = 0;

	// Wall flags (bit 0=North, 1=East, 2=South, 3=West)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Walls = 0;
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

	// Wall System API
	// Check if a wall exists on a specific edge of a tile
	UFUNCTION(BlueprintPure, Category = "Grid|Walls")
	bool HasWall(FIntVector Coords, EGridDirection Direction) const;

	// Set or remove a wall on a specific edge of a tile
	UFUNCTION(BlueprintCallable, Category = "Grid|Walls")
	void SetWall(FIntVector Coords, EGridDirection Direction, bool bEnabled);

	// Check if movement between two tiles is blocked by a wall
	UFUNCTION(BlueprintPure, Category = "Grid|Walls")
	bool IsMovementBlocked(FIntVector FromCoords, FIntVector ToCoords) const;

	// Refresh all wall visuals from current GridMap state
	void RefreshAllWallVisuals();

private:
	void InitializeGrid();
	void SpawnTileMesh(FIntVector Coords, const FTileData& Data);
	void SpawnConveyorArrow(FIntVector Coords, ETileType Type);
	void DestroyConveyorArrow(FIntVector Coords);
	void SpawnCheckpointLabel(FIntVector Coords, int32 CheckpointNumber);
	void DestroyCheckpointLabel(FIntVector Coords);
	void CreateBaseMaterial();

	// Wall visual helpers
	void SpawnWallMesh(FIntVector Coords, EGridDirection Direction);
	void RefreshWallVisual(FIntVector Coords);
	FVector GetWallOffset(EGridDirection Direction) const;
	FRotator GetWallRotation(EGridDirection Direction) const;
	FVector GetWallScale() const;
	uint8 DirectionToWallFlag(EGridDirection Direction) const;
	EGridDirection GetOppositeDirection(EGridDirection Direction) const;

	UPROPERTY()
	TMap<FIntVector, UStaticMeshComponent*> TileMeshes;

	UPROPERTY()
	TMap<FIntVector, UStaticMeshComponent*> ArrowMeshes;

	UPROPERTY()
	TMap<FIntVector, UTextRenderComponent*> CheckpointLabels;

	// Wall meshes stored by (Coords, Direction) - use a combined key
	// Key format: X + Y*1000 + DirectionBit*1000000
	UPROPERTY()
	TMap<int32, UStaticMeshComponent*> WallMeshes;

	UPROPERTY()
	USceneComponent* SceneRoot;

	UPROPERTY()
	UStaticMesh* CachedCubeMesh;

	UPROPERTY()
	UStaticMesh* CachedConeMesh;

	UPROPERTY()
	UMaterialInterface* CachedBaseMaterial;

	// Customizable wall mesh (falls back to CachedCubeMesh if not set)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid|Walls", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* WallMeshAsset;

	// Wall dimensions
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid|Walls", meta = (AllowPrivateAccess = "true"))
	float WallHeight = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grid|Walls", meta = (AllowPrivateAccess = "true"))
	float WallThickness = 5.0f;
};
