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

	Blocks.Reserve(Columns * RowsBuffer);
	for(int32 i = 0; i < Columns * RowsBuffer; i++)
	{
		Blocks.Emplace(nullptr);
	}

	SpawnNewTetromino();
}

void ATGrid::MoveTetromino(FVector Direction)
{
	FVector PreMoveLocation = TetrominoCurrent->GetActorLocation();
	TetrominoCurrent->AddActorWorldOffset(Direction);
	if(IsTetrominoOutOfBoundsVertical() || AreTetrominoBlocksBlockedByBlocks())
	{
		TetrominoCurrent->SetActorLocation(PreMoveLocation);
	}
}

void ATGrid::MoveTetrominoDown(FVector Direction)
{
	FVector PreMoveLocation = TetrominoCurrent->GetActorLocation();
	TetrominoCurrent->AddActorWorldOffset(Direction);
	if(IsTetrominoBelowGround() || AreTetrominoBlocksBlockedByBlocks())
	{
		// Tetromino just hit the ground

		TetrominoCurrent->SetActorLocation(PreMoveLocation);
		
		AddToGrid();

		CheckGridForFullLines();

		SpawnNewTetromino();
	}
}

void ATGrid::RotateTetromino()
{
	FRotator PreRotateRotation = TetrominoCurrent->GetActorRotation();
	TetrominoCurrent->AddActorWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	if(IsTetrominoOutOfBoundsVertical() || IsTetrominoBelowGround() || AreTetrominoBlocksBlockedByBlocks())
	{
		TetrominoCurrent->SetActorRotation(PreRotateRotation);
	}
}

bool ATGrid::IsTetrominoOutOfBoundsVertical()
{
	return TetrominoCurrent->IsOutOfBoundsVertical(Columns);
}

bool ATGrid::IsTetrominoBelowGround()
{
	return TetrominoCurrent->IsBelowGround();
}

bool ATGrid::AreTetrominoBlocksBlockedByBlocks()
{
	for(int32 i = 0; i < 4; i++)
	{
		if(Blocks[Index(TetrominoCurrent->GetBlockPositionFromWorld(i))] != nullptr)
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
		Blocks[Index(TetrominoCurrent->GetBlockPositionFromWorld(i))] = TetrominoCurrent->Blocks[i];
		UE_LOG(LogTemp, Warning, TEXT("Index: %d"), Index(TetrominoCurrent->GetBlockPositionFromWorld(i)));
		TetrominoCurrent->Blocks[i]->Rename(nullptr, this);
	}

	// TODO(NINJIN42): TetrominoCurrent will point to nothing, probably change the whole code and always check TetrominoCurrent
	TetrominoCurrent->Destroy();
}

void ATGrid::CheckGridForFullLines()
{
	for(int32 RowCurrent = 0; RowCurrent < Rows; RowCurrent++)
	{
		bool bFullLine = true;
		for(int32 ColumnCurrent = 0; ColumnCurrent < Columns; ColumnCurrent++)
		{
			if(Blocks[Index(FIntVector2D(RowCurrent, ColumnCurrent))] == nullptr)
			{
				bFullLine = false;
				break;
			}
		}
		if(bFullLine)
		{
			DeleteLine(RowCurrent);
			// Hack for now, will check from bottom again, obviously can be made smarter but takes more time
			RowCurrent = 0;
		}
	}
}

void ATGrid::DeleteLine(int32 Row)
{
	for(int32 ColumnCurrent = 0; ColumnCurrent < Columns; ColumnCurrent++)
	{
		Blocks[Index(FIntVector2D(Row, ColumnCurrent))]->SetVisibility(false);
	}
	Blocks.RemoveAt(Index(FIntVector2D(Row, 0)), Columns);
	for(UPaperSpriteComponent* Block : Blocks)
	{
		if(Block)
		{
			Block->AddWorldOffset(FVector(0.0f, 0.0f, -100.0f));
		}
	}
	for(int32 i = 0; i < Columns; i++)
	{
		Blocks.Emplace(nullptr);
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

