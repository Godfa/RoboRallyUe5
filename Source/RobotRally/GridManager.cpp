// Copyright (c) 2026 Robot Rally Team. All Rights Reserved.

#include "GridManager.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	Width = 10;
	Height = 10;
	TileSize = 100.0f;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeGrid();
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
