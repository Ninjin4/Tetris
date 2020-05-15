// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/Public/TTetromino.h"
#include "Components/InputComponent.h"
#include "Classes/Components/InstancedStaticMeshComponent.h"
#include "Tetris/Public/TGrid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

const FName ATTetromino::MoveRightName = FName(TEXT("MoveRight"));
const FName ATTetromino::MoveUpName = FName(TEXT("MoveUp"));
const FString ATTetromino::ContextString = FString(TEXT("TetrominoData Context"));


// Sets default values
ATTetromino::ATTetromino()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Whenever a tetromino is spawned, instantly give the player control over it
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	BlocksVisuals = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlocksVisual"));
	SetRootComponent(BlocksVisuals);
	BlocksVisuals->SetUsingAbsoluteRotation(true);

	BlockSize = 100.0f;
	DropSpeed = 3.0f;
	MoveSpeed = 9.0f;
	RotationSpeed = 3.0f;

	AutoReceiveInput = EAutoReceiveInput::Player0;
}

#if WITH_EDITOR
void ATTetromino::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

// Called to bind functionality to input
void ATTetromino::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &ATTetromino::DropPressed);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ATTetromino::MoveRightPressed);
	PlayerInputComponent->BindAction("MoveRight", IE_Released, this, &ATTetromino::MoveRightReleased);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ATTetromino::MoveLeftPressed);
	PlayerInputComponent->BindAction("Moveleft", IE_Released, this, &ATTetromino::MoveLefttReleased);
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
	HorizontalInputTime = 0.0f;
	CheckAndMoveHorizontal(1.0f);
}
void ATTetromino::MoveRightReleased()
{
	MoveRight = false;
}
void ATTetromino::MoveLeftPressed()
{
	MoveLeft = true;
	HorizontalInputTime = 0.0f;
	CheckAndMoveHorizontal(-1.0f);
}
void ATTetromino::MoveLefttReleased()
{
	MoveLeft = false;
}
void ATTetromino::MoveDownPressed()
{
	MoveDown = true;
	DropInputTime = 0.0f;
	CheckAndMoveDown();
}
void ATTetromino::MoveDownReleased()
{
	MoveDown = false;
}
void ATTetromino::RotateClockwisePressed()
{
	RotateClockwise = true;
	RotationInputTime = 0.0f;
	CheckAndRotate(1.0f);
}
void ATTetromino::RotateClockwiseReleased()
{
	RotateClockwise = false;
}
void ATTetromino::RotateCounterClockwisePressed()
{
	RotateCounterClockwise = true;
	RotationInputTime = 0.0f;
	CheckAndRotate(-1.0f);
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

void ATTetromino::SpawnTetromino(int32 GridColumns, ATGrid* GridNew)
{
	TArray<FName> RowNames = TetrominoDataTable->GetRowNames();
	int32 RowCount = RowNames.Num();

	int32 TetrominoRandomFromDataTable = SpawnStream.RandRange(0, RowCount-1);
	// TODO: Check if struct pointer is not handled by garbage collection
	FTetrominoData* TetrominoData = TetrominoDataTable->FindRow<FTetrominoData>(RowNames[TetrominoRandomFromDataTable], ContextString, true);

	ScaleFromColor = FVector::OneVector + (FVector(TetrominoData->Color) / 1000.0f);
	RotationRule = TetrominoData->RotationRule;

	// TODO: Might as well ask the designer to input rows/columns directly
	BlocksDimension = FMath::RoundToInt(FMath::Sqrt((float)(TetrominoData->Blocks.Num())));

	if (TetrominoData->Blocks.Num() % BlocksDimension != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Make sure every TetrominoShape has n*n cells!"));
	}

	// If column half is x.5, floor to prefer left side spawn
	const float SpawnLocationX = FMath::FloorToFloat(((float)(GridColumns-1)) / 2.0f)
		- FMath::FloorToFloat(((float)BlocksDimension) / 2.0f);

	const FVector SpawnLocation = FVector(SpawnLocationX, 0.0f, 0.0f);

	float PivotLocationWorldX = FMath::FloorToFloat(((float)(GridColumns-1)) / 2.0f)
		- ((BlocksDimension % 2 == 1) ? 0.0f : 0.5f);
	float PivotLocationWorldY = ((float)(BlocksDimension-1)) / 2.0f;
	PivotLocationWorld = FVector(PivotLocationWorldX, PivotLocationWorldY, 0.0f) * BlockSize;

	Blocks.Empty();
	Blocks.Reserve(FMath::Square(BlocksDimension));
	BlocksVisuals->ClearInstances();
	for (int32 Y = 0; Y < BlocksDimension; Y++)
	{
		for (int32 X = 0; X < BlocksDimension; X++)
		{
			if (TetrominoData->Blocks[Y * BlocksDimension + X] == true)
			{
				const FVector LocationTetroGrid = FVector((float)X, (float)Y, 0.0f);
				const FVector LocationWorld = (LocationTetroGrid + SpawnLocation) * BlockSize;
				Blocks.Emplace(FTransform(FQuat::Identity, LocationWorld, ScaleFromColor));
				BlocksVisuals->AddInstanceWorldSpace(Blocks.Last());
			}
		}
	}

	Grid = GridNew;
	bool bGameOver = !(Grid->IsMoveDownValid(Blocks));
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

	UKismetSystemLibrary::DrawDebugPoint(GetWorld(), PivotLocationWorld + FVector(0.0f, 0.0f, BlockSize), 5.0f, FLinearColor::Green);
}

// TODO: Refactor, remove code duplication
void ATTetromino::UpdateInputTimers(float DeltaTime)
{
	if (MoveRight != MoveLeft)
	{
		float MoveDirection;

		if (MoveRight)
		{
			HorizontalInputTime += DeltaTime * MoveSpeed;
			MoveDirection = 1.0f;
		}
		else
		{
			HorizontalInputTime += DeltaTime * MoveSpeed;
			MoveDirection = -1.0f;
		}

		if (HorizontalInputTime > 1.0f)
		{
			HorizontalInputTime = 0.0f;
			CheckAndMoveHorizontal(MoveDirection);
		}
	}

	if (RotateClockwise != RotateCounterClockwise)
	{
		float MoveDirection;

		if (RotateClockwise)
		{
			RotationInputTime += DeltaTime * RotationSpeed;
			MoveDirection = 1.0f;
		}
		else
		{
			RotationInputTime += DeltaTime * RotationSpeed;
			MoveDirection = -1.0f;
		}

		if (RotationInputTime > 1.0f)
		{
			RotationInputTime = 0.0f;
			CheckAndRotate(MoveDirection);
		}
	}

	if (MoveDown)
	{
		DropInputTime += DeltaTime * MoveSpeed;
	}
	else
	{
		DropInputTime += DeltaTime * DropSpeed;
	}

	if (DropInputTime > 1.0f)
	{
		DropInputTime = 0.0f;
		CheckAndMoveDown();
	} 
}

void ATTetromino::CheckAndMoveHorizontal(float Direction)
{
	FVector MoveDirection = FVector(BlockSize * Direction, 0.0f, 0.0f);
	for (FTransform& Block : Blocks)
	{
		Block.SetLocation(Block.GetLocation() + MoveDirection);
	}

	bool bAllowMove = Grid->IsMoveHorizontalValid(Blocks);
	if (bAllowMove)
	{
		PivotLocationWorld += MoveDirection;
		UpdateInstances();
	}
	else
	{
		for (FTransform& Block : Blocks)
		{
			Block.SetLocation(Block.GetLocation() - MoveDirection);
		}
	}
}

void ATTetromino::CheckAndRotate(float Direction)
{
	for (FTransform& Block : Blocks)
	{
		FVector RotationPivot = Block.GetLocation() - PivotLocationWorld;
		FQuat Rotation = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, Direction * 90.0f));
		FVector RotationVector = UKismetMathLibrary::Quat_RotateVector(Rotation, RotationPivot);
		FVector NewVector = PivotLocationWorld + RotationVector;
		Block.SetLocation(NewVector);
	}

	bool bAllowMove = Grid->IsRotationValid(Blocks);
	if (bAllowMove)
	{
		UpdateInstances();
	}
	else
	{
		for (FTransform& Block : Blocks)
		{
			FVector RotationPivot = Block.GetLocation() - PivotLocationWorld;
			FQuat Rotation = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, -Direction * 90.0f));
			FVector RotationVector = UKismetMathLibrary::Quat_RotateVector(Rotation, RotationPivot);
			FVector NewVector = PivotLocationWorld + RotationVector;
			Block.SetLocation(NewVector);
		}
	}
}

void ATTetromino::CheckAndMoveDown()
{
	FVector MoveDirection = FVector(0.0f, BlockSize, 0.0f);
	for (FTransform& Block : Blocks)
	{
		Block.SetLocation(Block.GetLocation() + MoveDirection);
	}

	bool AllowMove = Grid->IsMoveDownValid(Blocks);
	if (AllowMove)
	{
		PivotLocationWorld += MoveDirection;
		UpdateInstances();
	}
	else
	{
		for (FTransform& Block : Blocks)
		{
			Block.SetLocation(Block.GetLocation() - MoveDirection);
		}
		Grid->AddToGrid(Blocks);
	}
}

void ATTetromino::UpdateInstances()
{
	int32 BlocksCount = Blocks.Num();
	for (int32 BlockIndex = 0; BlockIndex < BlocksCount; BlockIndex++)
	{
		BlocksVisuals->UpdateInstanceTransform(BlockIndex, Blocks[BlockIndex], true);
	}
	BlocksVisuals->UpdateInstanceTransform(0, Blocks[0], true, true);
}
