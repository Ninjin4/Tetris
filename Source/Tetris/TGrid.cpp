// Fill out your copyright notice in the Description page of Project Settings.

#include "TGrid.h"


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
	
}

// Called every frame
void ATGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATGrid::OnLanded(ATTetromino * TetrominoLast)
{

}

bool ATGrid::IsInBounds(ATTetromino * TetrominoCurrent)
{
	return true;
}
