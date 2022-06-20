// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/Public/TTetromino.h"
#include "Tetris/Public/TGrid.h"
#include "TTetrominoData.h"

#include "Components/InputComponent.h"
#include "Classes/Components/InstancedStaticMeshComponent.h"

const FName ATTetromino::MoveRightName = FName(TEXT("MoveRight"));
const FName ATTetromino::MoveUpName = FName(TEXT("MoveUp"));
const FString ATTetromino::ContextString = FString(TEXT("TetrominoData Context"));

ATTetromino::ATTetromino()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BlocksVisuals = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlocksVisual"));
	SetRootComponent(BlocksVisuals);
	BlocksVisuals->SetUsingAbsoluteRotation(true);

	// Whenever a tetromino is spawned, instantly give the player control over it
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;
}

void ATTetromino::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &ATTetromino::DropPressed);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ATTetromino::MoveRightPressed);
	PlayerInputComponent->BindAction("MoveRight", IE_Released, this, &ATTetromino::MoveRightReleased);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ATTetromino::MoveLeftPressed);
	PlayerInputComponent->BindAction("MoveLeft", IE_Released, this, &ATTetromino::MoveLeftReleased);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ATTetromino::MoveDownPressed);
	PlayerInputComponent->BindAction("MoveDown", IE_Released, this, &ATTetromino::MoveDownReleased);
	PlayerInputComponent->BindAction("RotateClockwise", IE_Pressed, this, &ATTetromino::RotateClockwisePressed);
	PlayerInputComponent->BindAction("RotateClockwise", IE_Released, this, &ATTetromino::RotateClockwiseReleased);
	PlayerInputComponent->BindAction("RotateCounterClockwise", IE_Pressed, this, &ATTetromino::RotateCounterClockwisePressed);
	PlayerInputComponent->BindAction("RotateCounterClockwise", IE_Released, this, &ATTetromino::RotateCounterClockwiseReleased);
}
void ATTetromino::DropPressed()
{
}
void ATTetromino::MoveRightPressed()
{
	MoveRight = true;
	HorizontalInputTime = 0.f;
	CheckAndMoveHorizontal(1.f);
}
void ATTetromino::MoveRightReleased()
{
	MoveRight = false;
}
void ATTetromino::MoveLeftPressed()
{
	MoveLeft = true;
	HorizontalInputTime = 0.f;
	CheckAndMoveHorizontal(-1.f);
}
void ATTetromino::MoveLeftReleased()
{
	MoveLeft = false;
}
void ATTetromino::MoveDownPressed()
{
	MoveDown = true;
	DropInputTime = 0.f;
	CheckAndMoveDown();
}
void ATTetromino::MoveDownReleased()
{
	MoveDown = false;
}
void ATTetromino::RotateClockwisePressed()
{
	RotateClockwise = true;
	RotationInputTime = 0.f;
	CheckAndRotate(1.f);
}
void ATTetromino::RotateClockwiseReleased()
{
	RotateClockwise = false;
}
void ATTetromino::RotateCounterClockwisePressed()
{
	RotateCounterClockwise = true;
	RotationInputTime = 0.f;
	CheckAndRotate(-1.f);
}
void ATTetromino::RotateCounterClockwiseReleased()
{
	RotateCounterClockwise = false;
}

void ATTetromino::BeginPlay()
{
	Super::BeginPlay();

	SpawnStream = FRandomStream(FMath::Rand());
}

void ATTetromino::GenerateTetromino(const int32 gridColumns, ATGrid* gridNew)
{
	TArray<FName> RowNames = TetrominoDataTable->GetRowNames();
	const int32 rowCount = RowNames.Num();

	const int32 tetrominoRandomFromDataTable = SpawnStream.RandRange(0, rowCount-1);
	// TODO: Check if struct pointer is not handled by garbage collection
	const FTetrominoData* const tetrominoData = TetrominoDataTable->FindRow<FTetrominoData>(RowNames[tetrominoRandomFromDataTable], ContextString, true);

	ScaleFromColorCur = FVector::OneVector + (FVector(tetrominoData->Color) / 1000.f);
	RotationRuleCur = tetrominoData->RotationRule;

	// TODO: Might as well ask the designer to input rows/columns directly
	BlocksDimensionCur = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(tetrominoData->Blocks.Num())));

	if (!ensureMsgf(tetrominoData->Blocks.Num() % BlocksDimensionCur == 0, TEXT("Make sure every TetrominoShape has n*n cells!")))
	{
		return;
	}

	// If column half is x.5, floor to prefer left side spawn
	const float spawnLocationX =
		FMath::FloorToFloat(static_cast<float>(gridColumns-1) / 2.f) -
		FMath::FloorToFloat(static_cast<float>(BlocksDimensionCur) / 2.f);

	const FVector SpawnLocation = FVector(spawnLocationX, 0.f, 0.f);

	const float pivotLocationWorldX =
		FMath::FloorToFloat(static_cast<float>(gridColumns-1) / 2.f) -
		(BlocksDimensionCur % 2 == 1 ? 0.f : 0.5f);

	const float pivotLocationWorldY = static_cast<float>(BlocksDimensionCur-1) / 2.f;

	PivotLocationWorld = FVector(pivotLocationWorldX, pivotLocationWorldY, 0.f) * BlockSize;

	BlocksTransformsWorld.Empty();
	BlocksTransformsWorld.Reserve(FMath::Square(BlocksDimensionCur));
	for (int32 y = 0; y < BlocksDimensionCur; y++)
	{
		for (int32 x = 0; x < BlocksDimensionCur; x++)
		{
			if (tetrominoData->Blocks[y * BlocksDimensionCur + x] == true)
			{
				const FVector locationTetroGrid = FVector(static_cast<float>(x), static_cast<float>(y), 0.f);
				const FVector locationWorld = (locationTetroGrid + SpawnLocation) * BlockSize;
				BlocksTransformsWorld.Emplace(FTransform(FQuat::Identity, locationWorld, ScaleFromColorCur));
			}
		}
	}

	BlocksVisuals->ClearInstances();
	BlocksVisuals->AddInstances(BlocksTransformsWorld, false, true);

	Grid = gridNew;
	const bool bGameOver = !Grid->IsMoveDownValid(BlocksTransformsWorld);
	if (bGameOver)
	{
		SetActorTickEnabled(false);
	}
	else
	{
		SetActorTickEnabled(true);
	}
}

void ATTetromino::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateInputTimers(DeltaTime);

	// UKismetSystemLibrary::DrawDebugPoint(GetWorld(), PivotLocationWorld + FVector(0.f, 0.f, BlockSize), 5.f, FLinearColor::Green);
}

// TODO: Refactor, remove code duplication
void ATTetromino::UpdateInputTimers(float deltaTime)
{
	if (MoveRight != MoveLeft)
	{
		float moveDirection;

		if (MoveRight)
		{
			HorizontalInputTime += deltaTime * MoveSpeed;
			moveDirection = 1.f;
		}
		else
		{
			HorizontalInputTime += deltaTime * MoveSpeed;
			moveDirection = -1.f;
		}

		if (HorizontalInputTime > 1.f)
		{
			HorizontalInputTime = 0.f;
			CheckAndMoveHorizontal(moveDirection);
		}
	}

	if (RotateClockwise != RotateCounterClockwise)
	{
		float moveDirection;

		if (RotateClockwise)
		{
			RotationInputTime += deltaTime * RotationSpeed;
			moveDirection = 1.f;
		}
		else
		{
			RotationInputTime += deltaTime * RotationSpeed;
			moveDirection = -1.f;
		}

		if (RotationInputTime > 1.f)
		{
			RotationInputTime = 0.f;
			CheckAndRotate(moveDirection);
		}
	}

	if (MoveDown)
	{
		DropInputTime += deltaTime * MoveSpeed;
	}
	else
	{
		DropInputTime += deltaTime * DropSpeed;
	}

	if (DropInputTime > 1.f)
	{
		DropInputTime = 0.f;
		CheckAndMoveDown();
	} 
}

void ATTetromino::CheckAndMoveHorizontal(float direction)
{
	const FVector moveDirection = FVector(BlockSize * direction, 0.f, 0.f);
	for (FTransform& block : BlocksTransformsWorld)
	{
		block.SetLocation(block.GetLocation() + moveDirection);
	}

	const bool bAllowMove = Grid->IsMoveHorizontalValid(BlocksTransformsWorld);
	if (bAllowMove)
	{
		PivotLocationWorld += moveDirection;
		BlocksVisuals->BatchUpdateInstancesTransforms(0, BlocksTransformsWorld, true, true, true);
	}
	else
	{
		for (FTransform& block : BlocksTransformsWorld)
		{
			block.SetLocation(block.GetLocation() - moveDirection);
		}
	}
}

void ATTetromino::CheckAndRotate(float direction)
{
	for (FTransform& block : BlocksTransformsWorld)
	{
		const FVector rotationPivot = block.GetLocation() - PivotLocationWorld;
		const FQuat rotation = FQuat::MakeFromEuler(FVector(0.f, 0.f, direction * 90.f));
		const FVector rotationVector = rotation.RotateVector(rotationPivot);
		// FVector rotationVector = UKismetMathLibrary::Quat_RotateVector(rotation, rotationPivot);
		const FVector newVector = PivotLocationWorld + rotationVector;
		block.SetLocation(newVector);
	}

	const bool bAllowMove = Grid->IsRotationValid(BlocksTransformsWorld);
	if (bAllowMove)
	{
		BlocksVisuals->BatchUpdateInstancesTransforms(0, BlocksTransformsWorld, true, true, true);
	}
	else
	{
		for (FTransform& block : BlocksTransformsWorld)
		{
			const FVector rotationPivot = block.GetLocation() - PivotLocationWorld;
			const FQuat rotation = FQuat::MakeFromEuler(FVector(0.f, 0.f, -direction * 90.f));
			const FVector rotationVector = rotation.RotateVector(rotationPivot);
			// FVector rotationVector = UKismetMathLibrary::Quat_RotateVector(rotation, rotationPivot);
			const FVector newVector = PivotLocationWorld + rotationVector;
			block.SetLocation(newVector);
		}
	}
}

void ATTetromino::CheckAndMoveDown()
{
	const FVector moveDirection = FVector(0.f, BlockSize, 0.f);
	for (FTransform& block : BlocksTransformsWorld)
	{
		block.SetLocation(block.GetLocation() + moveDirection);
	}

	const bool allowMove = Grid->IsMoveDownValid(BlocksTransformsWorld);
	if (allowMove)
	{
		PivotLocationWorld += moveDirection;
		BlocksVisuals->BatchUpdateInstancesTransforms(0, BlocksTransformsWorld, true, true, true);
	}
	else
	{
		for (FTransform& block : BlocksTransformsWorld)
		{
			block.SetLocation(block.GetLocation() - moveDirection);
		}
		Grid->AddToGrid(BlocksTransformsWorld);
	}
}
