// Fill out your copyright notice in the Description page of Project Settings.

#include "TGrid.h"
#include "TTetromino.h"

// Sets default values
ATGrid::ATGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Rows = 10;
	Columns = 10;
}

#if WITH_EDITOR
void ATGrid::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Clear the grid and make new entries if Rows or Columns are changed
	Grid.Empty();

	Grid.Reserve(Rows * Columns);
	for(int32 Index : Grid)
	{
		Grid.Add(0);
	}
}
#endif // WITH_EDITOR

// Called when the game starts or when spawned
void ATGrid::BeginPlay()
{
	Super::BeginPlay();

	int32 Width = (Rows + 2);
	int32 Height = (Columns + 2);
	Grid.Reserve(Width * Height);

	for(int32 x = 0; x < Width; x++)
	{
		for(int32 y = 0; y < Height; y++)
		{
			Grid.Add((x == 0 || x == Width - 1 || y == 0 || y == Height - 1) ? 9 : 0);
		}
	}
}

FVector ATGrid::SpawnLocation()
{
	return FVector((Rows-1) * 100.0f, 0.0f, (Columns-1)/2 * 100.0f);
}

void ATGrid::MoveTetromino(ATTetromino* TetrominoLast, FVector Direction)
{
	FVector PreMoveLocation = TetrominoLast->GetActorLocation();
	TetrominoLast->AddActorWorldOffset(Direction);
	if(!IsInBounds(TetrominoLast))
	{
		TetrominoLast->SetActorLocation(PreMoveLocation);
	}
}

void ATGrid::RotateTetromino(ATTetromino* TetrominoLast)
{
	TetrominoLast->RotateClockwise();
}

void ATGrid::OnLanded(ATTetromino* TetrominoLast)
{

}

bool ATGrid::IsInBounds(ATTetromino* TetrominoCurrent)
{
	for(int32 i = 0; i < 4; i++)
	{
		FVector2D WorldIndices = TetrominoCurrent->GetWorldIndices(i);
		int32 Rows = FMath::TruncToInt(WorldIndices.X);
		int32 Columns = FMath::TruncToInt(WorldIndices.Y); 

		if(Grid[Index(Rows, Columns)] != 0)
		{
			return false;
		}
	}

	return true;
}

bool ATGrid::IsRotationValid(ATTetromino* TetrominoCurrent)
{
	for(int32 i = 0; i < 4; i++)
	{
		FVector2D WorldIndices = TetrominoCurrent->GetWorldIndices(i);
		int32 Rows = FMath::TruncToInt(WorldIndices.X);
		int32 Columns = FMath::TruncToInt(WorldIndices.Y); 

		if(Grid[Index(Rows, Columns)] != 0)
		{
			return false;
		}
	}

	return true;
}
