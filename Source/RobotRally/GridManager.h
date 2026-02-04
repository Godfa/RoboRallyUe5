// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	// Grid dimensions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Width = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Height = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float TileSize = 100.0f;

	// In-memory representation of the grid (Not a UPROPERTY because TMap with custom keys can be tricky, but FIntVector is a USTRUCT)
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

private:
	void InitializeGrid();
};
