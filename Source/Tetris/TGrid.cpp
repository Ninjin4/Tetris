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

	int32 Width = Rows;
	int32 Height = Columns;
	Grid.Reserve(Width * Height);

	for(int32 x = 0; x < Width; x++)
	{
		for(int32 y = 0; y < Height; y++)
		{
			Grid.Add(/*(x == 0 || x == Width - 1 || y == 0 || y == Height - 1) ? 9 : */0);
		}
	}

	SpawnNewTetromino();
}

void ATGrid::MoveTetromino(FVector Direction)
{
	FVector PreMoveLocation = TetrominoCurrent->GetActorLocation();
	TetrominoCurrent->AddActorWorldOffset(Direction);
	if(!IsInBounds())
	{
		TetrominoCurrent->SetActorLocation(PreMoveLocation);
	}
}

void ATGrid::MoveTetrominoDown(FVector Direction)
{
	FVector PreMoveLocation = TetrominoCurrent->GetActorLocation();
	TetrominoCurrent->AddActorWorldOffset(Direction);
	if(!IsOnGround())
	{
		TetrominoCurrent->SetActorLocation(PreMoveLocation);
		// Tetromino hits the ground
		SpawnNewTetromino();
	}
}

void ATGrid::RotateTetromino()
{
	FRotator PreRotateRotation = TetrominoCurrent->GetActorRotation();
	TetrominoCurrent->AddActorWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	if(!IsInBounds())
	{
		TetrominoCurrent->SetActorRotation(PreRotateRotation);
	}
}

bool ATGrid::IsInBounds()
{
	for(int32 i = 0; i < 4; i++)
	{
		if(TetrominoCurrent->GetGridPositionFromWorld(i).Column < 0 || TetrominoCurrent->GetGridPositionFromWorld(i).Column > Columns)
		{
			return false;
		}
	}
	return true;
}

bool ATGrid::IsOnGround()
{
	for(int32 i = 0; i < 4; i++)
	{
		if(TetrominoCurrent->GetGridPositionFromWorld(i).Row < 0)
		{
			return false;
		}
	}
	return true;
}

bool ATGrid::IsRotationValid()
{
	for(int32 i = 0; i < 4; i++)
	{
		if(Grid[Index(TetrominoCurrent->GetGridPositionFromWorld(i))] != 0)
		{
			return false;
		}
	}

	return true;
}

void ATGrid::OnLanded()
{

}

void ATGrid::SpawnNewTetromino()
{
	int32 RandomTetromino = FMath::RandRange(0, TetrominoBPs.Num() - 1);
	// TODO(NINJIN42): Use Deferred Spawning to wait for spawning until InitCustom is finished
	TetrominoCurrent = GetWorld()->SpawnActor<ATTetromino>(TetrominoBPs[RandomTetromino], SpawnLocation(), FRotator(0.0f));
	
	// The Deferred Spawning functions
	//FTransform SpawnTransform(Rotation, Origin);
	//UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TetrominoCurrent::StaticClass(), SpawnTransform));
	//if(TetrominoCurrent != nullptr)
	//{
	//	MyDeferredActor->Init(ShootDir);
	//	UGameplayStatics::FinishSpawningActor(TetrominoCurrent, SpawnTransform);
	//}
}

FVector ATGrid::SpawnLocation()
{
	return FVector((Columns-1)/2.0f * 100.0f, 0.0f, (Rows-1) * 100.0f);
}
