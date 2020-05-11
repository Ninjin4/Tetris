// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/Public/TGrid.h"
#include "Classes/Components/InstancedStaticMeshComponent.h"
#include "Tetris/Public/TTetromino.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ATGrid::ATGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BlocksVisual = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlocksVisual"));
	RootComponent = BlocksVisual;

	OuterGridVisual = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("OuterGridVisual"));
	RootComponent = OuterGridVisual;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->SetOrthoWidth(2048.0f);

	Rows = 20;
	Columns = 20;
	RowsWithBuffer = Rows + 4;
}

#if WITH_EDITOR
void ATGrid::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	RowsWithBuffer = Rows + 4;
}
#endif // WITH_EDITOR

// Called when the game starts or when spawned
void ATGrid::BeginPlay()
{
	Super::BeginPlay();

	FTransform InvisibleTransform = FTransform(FQuat::Identity, FVector::OneVector, FVector::ZeroVector);
	RowsWithBuffer = Rows + 4;
	Blocks.Reserve(Columns * RowsWithBuffer);
	for(int32 i = 0; i < Columns * RowsWithBuffer; i++)
	{
		Blocks.Emplace(InvisibleTransform);
	}

	SpawnNewTetromino();

	SpawnLocation = FVector((float (Columns/2)) * 100.0f, 0.0f, (float (Rows)) * 100.0f);
}

bool ATGrid::CanTetrominoMoveHorizontal(TArray<FTransform> BlocksTetromino) const
{
	if(IsTetrominoOutOfBoundsVertical(BlocksTetromino) || IsTetrominoBlockedByGridBlocks(BlocksTetromino))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool ATGrid::CanTetrominoMoveDown(TArray<FTransform> BlocksTetromino) const
{
	if(IsTetrominoBelowGround(BlocksTetromino) || IsTetrominoBlockedByGridBlocks(BlocksTetromino))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool ATGrid::CanRotateTetromino(TArray<FTransform> BlocksTetromino) const
{
	if(IsTetrominoOutOfBoundsVertical(BlocksTetromino) || IsTetrominoBelowGround(BlocksTetromino)
		|| IsTetrominoBlockedByGridBlocks(BlocksTetromino))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool ATGrid::IsTetrominoOutOfBoundsVertical(TArray<FTransform> BlocksTetromino) const
{
	for(const FTransform& BlockTetromino : BlocksTetromino)
	{
		int32 Column = FMath::RoundToInt(BlockTetromino.GetTranslation().X / 100.0f);
		if(Column < 0 || Column >= Columns)
		{
			return true;
		}
	}
	return false;
}

bool ATGrid::IsTetrominoBelowGround(TArray<FTransform> BlocksTetromino) const
{
	for(const FTransform& Block : BlocksTetromino)
	{
		if(FMath::RoundToInt(Block.GetTranslation().Z / 100.0f) < 0)
		{
			return true;
		}
	}
	return false;
}

bool ATGrid::IsTetrominoBlockedByGridBlocks(TArray<FTransform> BlocksTetromino) const
{
	for(const FTransform& BlockTetromino : BlocksTetromino)
	{
		if(!(IsGridBlockVisible(ConvertBlockLocationToGridIndex(BlockTetromino))))
		{
			return true;
		}
	}
	return false;
}

void ATGrid::AddToGrid(TArray<FTransform> BlocksTetromino)
{
	for(FTransform& BlockTetromino : BlocksTetromino)
	{
		const int32 BlocksIndex = ConvertBlockLocationToGridIndex(BlockTetromino);
		Blocks[BlocksIndex].SetScale3D(BlockTetromino.GetScale3D());
	}

	CheckGridForFullLines();

	SpawnNewTetromino();
}

void ATGrid::CheckGridForFullLines()
{
	// TODO(Ninjin42): Only check the rows near the Tetromino position instead of the whole array
	for(int32 RowCurrent = 0; RowCurrent < Rows; RowCurrent++)
	{
		bool bFullLine = true;
		for(int32 ColumnCurrent = 0; ColumnCurrent < Columns; ColumnCurrent++)
		{
			if(IsGridBlockVisible(Index(FIntVector2D(RowCurrent, ColumnCurrent))))
			{
				bFullLine = false;
				break;
			}
		}
		if(bFullLine)
		{
			DeleteLine(RowCurrent);
			RowCurrent--;
		}
	}
}

void ATGrid::DeleteLine(int32 Row)
{
	for(int32 ColumnCurrent = 0; ColumnCurrent < Columns; ColumnCurrent++)
	{
		Blocks[Index(FIntVector2D(Row, ColumnCurrent))].SetScale3D(FVector::ZeroVector);
	}
	//Blocks.RemoveAt(Index(FIntVector2D(Row, 0)), Columns);

	// Move every row 1 unit down
	for(int32 StartElement = Index(FIntVector2D(Row, 0)); StartElement < Blocks.Num(); StartElement++)
	{
		if(IsGridBlockVisible(StartElement))
		{
			Blocks[StartElement].AddToTranslation(FVector(0.0f, 0.0f, -100.0f));
		}
	}

	//for(int32 i = 0; i < Columns; i++)
	//{
	//	Blocks.Emplace(nullptr);
	//}
}

void ATGrid::SpawnNewTetromino()
{
	int32 RandomTetromino = FMath::RandRange(0, TetrominoBPs.Num() - 1);
	GetWorld()->SpawnActor<ATTetromino>(TetrominoBPs[RandomTetromino], SpawnLocation, FRotator(0.0f));
	
	// Might want to use Deferred Spawning to wait for spawning until InitCustom is finished, but not necessary when spawning BPs

	//FTransform SpawnTransform(Rotation, Origin);
	//UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TetrominoCurrent::StaticClass(), SpawnTransform));
	//if(TetrominoCurrent != nullptr)
	//{
	//	MyDeferredActor->Init();
	//	UGameplayStatics::FinishSpawningActor(TetrominoCurrent, SpawnTransform);
	//}
}

int32 ATGrid::Index(FIntVector2D GridPosition) const
{
	return GridPosition.Row * Columns + GridPosition.Column;
}

int32 ATGrid::ConvertBlockLocationToGridIndex(FTransform Block) const
{
	int32 GridX = FMath::RoundToInt(Block.GetLocation().X / 100.0f);
	int32 GridY = FMath::RoundToInt(Block.GetLocation().Y / 100.0f);
	FIntVector2D GridPosition = FIntVector2D(GridX, GridY);

	return Index(GridPosition);
}

FIntVector2D ATGrid::GetBlockPositionFromWorld(int32 BlockIndex) const
{
	FIntVector2D GridPosition = FIntVector2D(FMath::RoundToInt(Blocks[BlockIndex].GetTranslation().Z / 100.0f),
		FMath::RoundToInt(Blocks[BlockIndex].GetTranslation().X / 100.0f));
	return GridPosition;
}

bool ATGrid::IsGridBlockVisible(int32 Index) const
{
	return FMath::IsNearlyZero(Blocks[Index].GetScale3D().Size()) ? false : true;
}
