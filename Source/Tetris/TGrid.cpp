// Fill out your copyright notice in the Description page of Project Settings.

#include "TGrid.h"
#include "Classes/PaperSpriteComponent.h"

// Sets default values
ATGrid::ATGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneDefault = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneDefault;

	Rows = 20;
	Columns = 20;
	RowsBuffer = Rows + 4;
}

#if WITH_EDITOR
void ATGrid::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	RowsBuffer = Rows + 4;
}
#endif // WITH_EDITOR

// Called when the game starts or when spawned
void ATGrid::BeginPlay()
{
	Super::BeginPlay();

	RowsBuffer = Rows + 4;
	Grid.Reserve(Columns * RowsBuffer);

	for(int32 x = 0; x < Columns; x++)
	{
		for(int32 y = 0; y < RowsBuffer; y++)
		{
			Grid.Add(0);
		}
	}
	SpawnNewTetromino();
}

void ATGrid::MoveTetromino(FVector Direction)
{
	FVector PreMoveLocation = TetrominoCurrent->GetActorLocation();
	TetrominoCurrent->AddActorWorldOffset(Direction);
	if(IsOutOfBounds() || IsBlockedByGridBlocks())
	{
		TetrominoCurrent->SetActorLocation(PreMoveLocation);
	}
}

void ATGrid::MoveTetrominoDown(FVector Direction)
{
	FVector PreMoveLocation = TetrominoCurrent->GetActorLocation();
	TetrominoCurrent->AddActorWorldOffset(Direction);
	if(IsBelowGround() || IsBlockedByGridBlocks())
	{
		TetrominoCurrent->SetActorLocation(PreMoveLocation);
		// Tetromino just hit the ground
		AddToGrid();

		CheckGridForFullLines();

		SpawnNewTetromino();
	}
}

void ATGrid::RotateTetromino()
{
	FRotator PreRotateRotation = TetrominoCurrent->GetActorRotation();
	TetrominoCurrent->AddActorWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	if(IsOutOfBounds() || IsBelowGround() || IsBlockedByGridBlocks())
	{
		TetrominoCurrent->SetActorRotation(PreRotateRotation);
	}
}

bool ATGrid::IsOutOfBounds()
{
	for(int32 i = 0; i < 4; i++)
	{
		if(TetrominoCurrent->GetGridPositionFromWorld(i).Column < 0 || TetrominoCurrent->GetGridPositionFromWorld(i).Column > Columns)
		{
			return true;
		}
	}
	return false;
}

bool ATGrid::IsBelowGround()
{
	for(int32 i = 0; i < 4; i++)
	{
		if(TetrominoCurrent->GetGridPositionFromWorld(i).Row < 0)
		{
			return true;
		}
	}
	return false;
}

bool ATGrid::IsBlockedByGridBlocks()
{
	for(int32 i = 0; i < 4; i++)
	{
		if(Grid[Index(TetrominoCurrent->GetGridPositionFromWorld(i))] != 0)
		{
			return true;
		}
	}
	return false;
}

void ATGrid::AddToGrid()
{
	for(int32 i = 0; i < 4; i++)
	{
		Grid[Index(TetrominoCurrent->GetGridPositionFromWorld(i))] = 1;
		Blocks.Emplace(TetrominoCurrent->Blocks[i]);
		TetrominoCurrent->Blocks[i]->Rename(nullptr, this);
	}

	// TODO(NINJIN42): TetrominoCurrent will point to nothing, probably change the whole code and always check TetrominoCurrent
	TetrominoCurrent->Destroy();
}

void ATGrid::CheckGridForFullLines()
{
	for(int32 x = 0; x < Rows; x++)
	{
		bool bFullLine = true;
		for(int32 y = 0; y < Columns; y++)
		{
			if(Grid[Index(FIntVector2D(x, y))] != 0)
			{
				bFullLine = false;
			}
		}
		if(bFullLine)
		{
			DeleteLine(x);
		}
	}
}

void ATGrid::DeleteLine(int32 Row)
{
	for(int32 y = 0; y < Columns; y++)
	{
		Blocks[Index(FIntVector2D(Row, y))]->SetVisibility(false);
	}
	Blocks.RemoveAt(Index(FIntVector2D(Row, Columns)), Columns);
	for(UPaperSpriteComponent* Block : Blocks)
	{
		Block->AddWorldOffset(FVector(0.0f, 0.0f, -100.0f));
	}

	Grid.RemoveAt(Row, Columns);
	for(int32 y = 0; y < Columns; y++)
	{
		Grid.Add(0);
	}
}

void ATGrid::SpawnNewTetromino()
{
	int32 RandomTetromino = FMath::RandRange(0, TetrominoBPs.Num() - 1);
	TetrominoCurrent = GetWorld()->SpawnActor<ATTetromino>(TetrominoBPs[RandomTetromino], SpawnLocation(), FRotator(0.0f));
	
	// Might want to use Deferred Spawning to wait for spawning until InitCustom is finished, not necessary when spawning BPs anyway

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
	// Top, Centre
	return FVector((float (Columns/2)) * 100.0f, 0.0f, (float (Rows)) * 100.0f);
}

