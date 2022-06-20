// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/Public/TGrid.h"
#include "Tetris/Public/TTetromino.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Classes/Components/InstancedStaticMeshComponent.h"

ATGrid::ATGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	BlocksVisuals = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlocksVisuals"));
	SetRootComponent(BlocksVisuals);

	OuterGridVisual = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("OuterGridVisual"));
	OuterGridVisual->SetupAttachment(BlocksVisuals);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BlocksVisuals);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->SetOrthoWidth(2048.f);
}

#if WITH_EDITOR
void ATGrid::PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent)
{
	Super::PostEditChangeProperty(propertyChangedEvent);

	RowsWithBuffer = Rows + 4;

	InitGridOuter();

	CentreSpringArm();
}
#endif

void ATGrid::CentreSpringArm()
{
	SpringArm->SetRelativeLocation(BlockSize * FVector(
		static_cast<float>(Columns) / 2.f,
		static_cast<float>(Rows) / 2.f,
		0.f));
}

void ATGrid::BeginPlay()
{
	Super::BeginPlay();

	InitGrid();
	InitGridOuter();
	SpawnNewTetromino();

	GetWorld()->GetFirstPlayerController()->SetViewTarget(this);

	CentreSpringArm();
}

void ATGrid::InitGrid()
{
	CellCount = Rows * Columns;
	Blocks.Empty();
	Blocks.Reserve(CellCount);

	for (int32 rowIndex = 0; rowIndex < Rows; rowIndex++)
	{
		for (int32 columnIndex = 0; columnIndex < Columns; columnIndex++)
		{
			FVector location = FVector(rowIndex, columnIndex, 0.f) * BlockSize;
			FTransform transform = FTransform(FQuat::Identity, location, FVector::ZeroVector);
			Blocks.Emplace(transform);
		}
	}

	BlocksVisuals->ClearInstances();
	BlocksVisuals->AddInstances(Blocks, false, true);
}

void ATGrid::InitGridOuter()
{
	BlocksOuter.Empty();
	BlocksOuter.Reserve(2 * Rows + Columns);

	for (int32 y = 0; y < Rows; y++)
	{
		const FVector location = FVector(-BlockSize, BlockSize * y, 0.f);
		const FTransform transform = FTransform(FQuat::Identity, location, FVector(1.001f));
		BlocksOuter.Emplace(transform);
	}

	for (int32 y = 0; y < Rows; y++)
	{
		const FVector location = FVector(static_cast<float>(Columns) * BlockSize, BlockSize * y, 0.f);
		const FTransform transform = FTransform(FQuat::Identity, location, FVector(1.001f));
		BlocksOuter.Emplace(transform);
	}

	for (int32 x = -1; x < Columns+1; x++)
	{
		const FVector location = FVector(BlockSize * x, static_cast<float>(Rows) * BlockSize, 0.f);
		const FTransform transform = FTransform(FQuat::Identity, location, FVector(1.001));
		BlocksOuter.Emplace(transform);
	}

	OuterGridVisual->ClearInstances();
	OuterGridVisual->AddInstances(BlocksOuter, false, true);
}

void ATGrid::SpawnNewTetromino()
{
	/*ATTetromino* TTetromino = GetWorld()->SpawnActor<ATTetromino>(TetrominoBP, SpawnLocation, FRotator(0.f));*/
	// TODO: Current hack

	TTetromino = Cast<ATTetromino>(GetWorld()->GetFirstPlayerController()->GetPawn());
	ensure(IsValid(TTetromino));

	TTetromino->GenerateTetromino(Columns, this);

	// Might want to use Deferred Spawning to wait for spawning until InitCustom is finished, but not necessary when spawning BPs

	//FTransform SpawnTransform(Rotation, Origin);
	//UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TetrominoCurrent::StaticClass(), SpawnTransform));
	//if(TetrominoCurrent != nullptr)
	//{
	//	MyDeferredActor->Init();
	//	UGameplayStatics::FinishSpawningActor(TetrominoCurrent, SpawnTransform);
	//}
}

bool ATGrid::IsMoveHorizontalValid(const TArray<FTransform> tetrominoBlocks) const
{
	for(const FTransform& tetrominoBlock : tetrominoBlocks)
	{
		const FIntPoint tetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(tetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(tetrominoBlock.GetLocation().Y / BlockSize));

		if(IsBlockOutOfBoundsVertical(tetrominoInstanceGridPosition.X))
		{
			return false;
		}

		if(IsBlockOnTopOfGridBlock(tetrominoInstanceGridPosition))
		{
			return false;
		}
	}

	return true;
}

bool ATGrid::IsMoveDownValid(const TArray<FTransform> tetrominoBlocks) const
{
	for(const FTransform& tetrominoBlock : tetrominoBlocks)
	{
		const FIntPoint tetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(tetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(tetrominoBlock.GetLocation().Y / BlockSize));

		if(IsBlockBelowGround(tetrominoInstanceGridPosition.Y))
		{
			return false;
		}

		if(IsBlockOnTopOfGridBlock(tetrominoInstanceGridPosition))
		{
			return false;
		}
	}

	return true;
}

bool ATGrid::IsRotationValid(const TArray<FTransform> tetrominoBlocks) const
{
	for(const FTransform& tetrominoBlock : tetrominoBlocks)
	{
		const FIntPoint tetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(tetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(tetrominoBlock.GetLocation().Y / BlockSize));

		if(IsBlockOutOfBoundsVertical(tetrominoInstanceGridPosition.X))
		{
			return false;
		}

		if(IsBlockBelowGround(tetrominoInstanceGridPosition.Y))
		{
			return false;
		}

		if(IsBlockOnTopOfGridBlock(tetrominoInstanceGridPosition))
		{
			return false;
		}
	}

	return true;
}

bool ATGrid::IsBlockOutOfBoundsVertical(const int32 tetrominoInstanceGridPositionX) const
{
	return tetrominoInstanceGridPositionX < 0 || tetrominoInstanceGridPositionX > Columns-1;
}

bool ATGrid::IsBlockBelowGround(const int32 tetrominoInstanceGridPositionY) const
{
	return tetrominoInstanceGridPositionY > Rows-1;
}

bool ATGrid::IsBlockOnTopOfGridBlock(const FIntPoint& tetrominoInstanceGridPosition) const
{
	const int32 gridIndex = FindIndex(tetrominoInstanceGridPosition);

	return IsGridBlockVisible(gridIndex);
}

bool ATGrid::IsGridBlockVisible(const int32 index) const
{
	const bool isScaleZero = FMath::IsNearlyZero(Blocks[index].GetScale3D().SizeSquared()) ? true : false;
	return !isScaleZero;
}

void ATGrid::AddToGrid(const TArray<FTransform> tetrominoBlocks)
{
	for(const FTransform& tetrominoBlock : tetrominoBlocks)
	{
		const FIntPoint tetrominoInstanceGridPosition = FIntPoint(
			FMath::RoundToInt(tetrominoBlock.GetLocation().X / BlockSize),
			FMath::RoundToInt(tetrominoBlock.GetLocation().Y / BlockSize));

		const int32 index = FindIndex(tetrominoInstanceGridPosition);
		Blocks[index] = tetrominoBlock;
	}

	CheckGridForFullLines();
	TTetromino->GenerateTetromino(Columns, this);
}

void ATGrid::CheckGridForFullLines()
{
	// TODO(Ninjin): Only check the rows near the Tetromino position instead of the whole array
	for(int32 y = 0; y < Rows; y++)
	{
		bool bFullLine = true;
		for(int32 x = 0; x < Columns; x++)
		{
			if(!IsGridBlockVisible(FindIndex(FIntPoint(x, y))))
			{
				bFullLine = false;
				break;
			}
		}

		if(bFullLine)
		{
			DeleteRow(y);
		}
	}
	BlocksVisuals->BatchUpdateInstancesTransforms(0, Blocks, true, true, true);

}

void ATGrid::DeleteRow(const int32 y)
{
	const int32 indexStart = FindIndex(FIntPoint(0, y));
	Blocks.RemoveAt(indexStart, Columns, true);

	for(int32 index = indexStart-1; index > -1; index--)
	{
		Blocks[index].SetLocation(Blocks[index].GetLocation() + FVector(0.f, BlockSize, 0.f));
	}

	for(int32 x = Columns-1; x > -1; x--)
	{
		const FVector location = FVector(x, y, 0.f) * BlockSize;
		const FTransform transform = FTransform(FQuat::Identity, location, FVector::ZeroVector);
		Blocks.Insert(transform, 0);
	}

}

int32 ATGrid::FindIndex(const FIntPoint& gridPosition) const
{
	return gridPosition.Y * Columns + gridPosition.X;
}
