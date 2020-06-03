// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/Public/TGrid.h"
#include "Classes/Components/InstancedStaticMeshComponent.h"
#include "Tetris/Public/TTetromino.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ATGrid::ATGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BlocksVisuals = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlocksVisuals"));
	SetRootComponent(BlocksVisuals);

	OuterGridVisual = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("OuterGridVisual"));
	OuterGridVisual->SetupAttachment(BlocksVisuals);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BlocksVisuals);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->SetOrthoWidth(2048.0f);

	Rows = 20;
	Columns = 20;
	RowsWithBuffer = Rows + 4;

	BlockSize = 100.0f;
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

	InitGrid();
	InitGridOuter();
	SpawnNewTetromino();

	GetWorld()->GetFirstPlayerController()->SetViewTarget(this);
	SpringArm->SetRelativeLocation(FVector(((float)Columns / 2.0f),
		((float)Rows / 2.0f), 0.0f) * BlockSize);
}

void ATGrid::InitGrid()
{
	CellCount = Rows * Columns;
	Blocks.Reserve(CellCount);

	for (int32 RowIndex = 0; RowIndex < Rows; RowIndex++)
	{
		for (int32 ColumnIndex = 0; ColumnIndex < Columns; ColumnIndex++)
		{
			FVector Location = FVector(RowIndex, ColumnIndex, 0.0f) * BlockSize;
			FTransform Transform = FTransform(FQuat::Identity, Location, FVector::ZeroVector);
			Blocks.Emplace(Transform);

			BlocksVisuals->AddInstanceWorldSpace(Blocks.Last());
		}
	}
}

void ATGrid::InitGridOuter()
{
	Blocks.Reserve(2 * Rows + Columns);

	for (int32 Y = 0; Y < Rows; Y++)
	{
		FVector Location = FVector(-BlockSize, BlockSize * Y, 0.0f);
		FTransform Transform = FTransform(FQuat::Identity, Location, FVector(1.001f));
		Blocks.Emplace(Transform);

		BlocksVisuals->AddInstanceWorldSpace(Blocks.Last());
	}

	for (int32 Y = 0; Y < Rows; Y++)
	{
		FVector Location = FVector(((float)Columns) * BlockSize, BlockSize * Y,  0.0f);
		FTransform Transform = FTransform(FQuat::Identity, Location, FVector(1.001f));
		Blocks.Emplace(Transform);

		BlocksVisuals->AddInstanceWorldSpace(Blocks.Last());
	}

	for (int32 X = -1; X < Columns+1; X++)
	{
		FVector Location = FVector(BlockSize * X, ((float)Rows) * BlockSize, 0.0f);
		FTransform Transform = FTransform(FQuat::Identity, Location, FVector(1.001f));
		Blocks.Emplace(Transform);

		BlocksVisuals->AddInstanceWorldSpace(Blocks.Last());
	}
}

void ATGrid::SpawnNewTetromino()
{
	/*ATTetromino* TTetromino = GetWorld()->SpawnActor<ATTetromino>(TetrominoBP, SpawnLocation, FRotator(0.0f));*/
	// TODO: Current hack
	
	TTetromino = Cast<ATTetromino>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (TTetromino)
	{
		TTetromino->SpawnTetromino(Columns, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tetromino cast failed!"));
	}

	// Might want to use Deferred Spawning to wait for spawning until InitCustom is finished, but not necessary when spawning BPs

	//FTransform SpawnTransform(Rotation, Origin);
	//UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TetrominoCurrent::StaticClass(), SpawnTransform));
	//if(TetrominoCurrent != nullptr)
	//{
	//	MyDeferredActor->Init();
	//	UGameplayStatics::FinishSpawningActor(TetrominoCurrent, SpawnTransform);
	//}
}

bool ATGrid::IsMoveHorizontalValid(const TArray<FTransform> TetrominoBlocks) const
{
	for(const FTransform& TetrominoBlock : TetrominoBlocks)
	{
		FIntPoint TetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(TetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(TetrominoBlock.GetLocation().Y / BlockSize));
		
		if(IsBlockOutOfBoundsVertical(TetrominoInstanceGridPosition.X))
		{
			return false;
		}

		if(IsBlockOnTopOfGridBlock(TetrominoInstanceGridPosition))
		{
			return false;
		}
	}

	return true;
}

bool ATGrid::IsMoveDownValid(const TArray<FTransform> TetrominoBlocks) const
{
	for(const FTransform& TetrominoBlock : TetrominoBlocks)
	{
		FIntPoint TetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(TetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(TetrominoBlock.GetLocation().Y / BlockSize));
		
		if(IsBlockBelowGround(TetrominoInstanceGridPosition.Y))
		{
			return false;
		}

		if(IsBlockOnTopOfGridBlock(TetrominoInstanceGridPosition))
		{
			return false;
		}
	}

	return true;
}

bool ATGrid::IsRotationValid(const TArray<FTransform> TetrominoBlocks) const
{
	for(const FTransform& TetrominoBlock : TetrominoBlocks)
	{
		FIntPoint TetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(TetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(TetrominoBlock.GetLocation().Y / BlockSize));
		
		if(IsBlockOutOfBoundsVertical(TetrominoInstanceGridPosition.X))
		{
			return false;
		}

		if(IsBlockBelowGround(TetrominoInstanceGridPosition.Y))
		{
			return false;
		}
		
		if(IsBlockOnTopOfGridBlock(TetrominoInstanceGridPosition))
		{
			return false;
		}
	}

	return true;
}

bool ATGrid::IsBlockOutOfBoundsVertical(int32 TetrominoInstanceGridPositionX) const
{
	return ((TetrominoInstanceGridPositionX < 0) || (TetrominoInstanceGridPositionX > Columns-1));
}

bool ATGrid::IsBlockBelowGround(int32 TetrominoInstanceGridPositionY) const
{
	return (TetrominoInstanceGridPositionY > Rows-1);
}

bool ATGrid::IsBlockOnTopOfGridBlock(FIntPoint TetrominoInstanceGridPosition) const
{
	int32 GridIndex = FindIndex(TetrominoInstanceGridPosition);

	return IsGridBlockVisible(GridIndex);
}

bool ATGrid::IsGridBlockVisible(int32 Index) const
{
	const bool IsScaleZero = (FMath::IsNearlyZero(Blocks[Index].GetScale3D().SizeSquared())) ? true : false;
	return !IsScaleZero;
}

void ATGrid::AddToGrid(const TArray<FTransform> TetrominoBlocks)
{
	for(const FTransform& TetrominoBlock : TetrominoBlocks)
	{
		FIntPoint TetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(TetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(TetrominoBlock.GetLocation().Y / BlockSize));
		
		int32 Index = FindIndex(TetrominoInstanceGridPosition);
		Blocks[Index] = TetrominoBlock;
	}

	CheckGridForFullLines();
	TTetromino->SpawnTetromino(Columns, this);
}

void ATGrid::CheckGridForFullLines()
{
	// TODO(Ninjin42): Only check the rows near the Tetromino position instead of the whole array
	for(int32 Y = 0; Y < Rows; Y++)
	{
		bool bFullLine = true;
		int32 X = 0;
		for(X; X < Columns; X++)
		{
			if(!(IsGridBlockVisible(FindIndex(FIntPoint(X, Y)))))
			{
				bFullLine = false;
				break;
			}
		}

		if(bFullLine)
		{
			DeleteRow(Y);
		}
	}
	UpdateInstances(Blocks);
}

void ATGrid::DeleteRow(int32 Y)
{
	int32 IndexStart = FindIndex(FIntPoint(0, Y));
	Blocks.RemoveAt(IndexStart, Columns, true);

	for(int32 Index = IndexStart-1; Index > -1; Index--)
	{
		Blocks[Index].SetLocation(Blocks[Index].GetLocation() + FVector(0.0f, BlockSize, 0.0f));
	}

	for(int32 X = Columns-1; X > -1; X--)
	{
		const FVector Location = FVector(X, Y, 0.0f) * BlockSize;
		const FTransform Transform = FTransform(FQuat::Identity, Location, FVector::ZeroVector);
		Blocks.Insert(Transform, 0);
	}

}

int32 ATGrid::FindIndex(FIntPoint GridPosition) const
{
	return GridPosition.Y * Columns + GridPosition.X;
}

void ATGrid::UpdateInstances(TArray<FTransform> BlocksCur)
{
	int32 BlocksCount = BlocksCur.Num();
	for (int32 BlockIndex = 0; BlockIndex < BlocksCount; BlockIndex++)
	{
		BlocksVisuals->UpdateInstanceTransform(BlockIndex, BlocksCur[BlockIndex], true);
	}
	BlocksVisuals->UpdateInstanceTransform(0, BlocksCur[0], true, true);
}
