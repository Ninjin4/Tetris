// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/Public/TTetromino.h"
#include "Components/InputComponent.h"
#include "Classes/Components/InstancedStaticMeshComponent.h"
#include "Tetris/Public/TGrid.h"
#include "Kismet/KismetMathLibrary.h"

const FName ATTetromino::MoveRightName = FName(TEXT("MoveRight"));
const FName ATTetromino::MoveUpName = FName(TEXT("MoveUp"));

// Sets default values
ATTetromino::ATTetromino()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Whenever a tetromino is spawned, instantly give the player control over it
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Always initialize at least 1 block in the centre
	Blocks.Add(FTransform::Identity);

	BlocksVisual = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlocksVisual"));
	SetRootComponent(BlocksVisual);
	BlocksVisual->SetUsingAbsoluteRotation(true);

	BlockSize = 100.0f;
	DropSpeed = 1.0f;
	MoveSpeedHolding = 3.0f;

	DeadZoneCentre = 0.2f;
	DeadZoneCentreSquared = FMath::Square(DeadZoneCentre);
}

#if WITH_EDITOR
void ATTetromino::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DeadZoneCentreSquared = FMath::Square(DeadZoneCentre);
}
#endif // WITH_EDITOR

// Called to bind functionality to input
void ATTetromino::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(MoveRightName, this, &ATTetromino::MoveRight);
	PlayerInputComponent->BindAxis(MoveUpName, this, &ATTetromino::MoveUp);
}
void ATTetromino::MoveRight(float Strength)
{
}
void ATTetromino::MoveUp(float Strength)
{
}

void ATTetromino::BeginPlay()
{
	Super::BeginPlay();

	TArray<FName> RowNames = TetrominoDataTable->GetRowNames();
	int32 RowCount = RowNames.Num();
	int32 TetrominoCurrent = UKismetMathLibrary::RandomInteger(RowCount);
	static const FString ContextString(TEXT("TetrominoData Context"));
	FTetrominoData* TetrominoData = TetrominoDataTable->FindRow<FTetrominoData>(RowNames[TetrominoCurrent], ContextString, true);
	
	const FVector ScaleFromColor = FVector::OneVector + (FVector(TetrominoData->Color) / BlockSize);
	int32 TetrominoDimension = FMath::Square(TetrominoData->Shape.Num());
}

void ATTetromino::UpdateInput()
{
	InputMoveRaw = FVector2D(GetInputAxisValue(MoveRightName), GetInputAxisValue(MoveUpName));
	// Deadzone and Remap back to 0-1
	InputMoveRemap = InputMoveRaw * ((InputMoveRaw.Size() - DeadZoneCentre) / (1.0f - DeadZoneCentre));
}

void ATTetromino::UpdateDirectionInput(float DeltaTime)
{
	EDirectionInput DirectionInputNew;

	const bool InputNone = (InputMoveRaw.SizeSquared() < DeadZoneCentreSquared) ? true : false;
	if (InputNone)
	{
		DirectionInputNew = EDirectionInput::None;
	}
	else
	{
		const bool HorizontalMove = (FMath::Abs(InputMoveRemap.X) > FMath::Abs(InputMoveRemap.Y)) ? true : false;

		if (HorizontalMove)
		{
			DirectionInputNew = (InputMoveRemap.X > 0.0f) ? EDirectionInput::Right : EDirectionInput::Left;
		}
		else
		{
			DirectionInputNew = (InputMoveRemap.Y > 0.0f) ? EDirectionInput::Top : EDirectionInput::Down;
		}
	}
	
	if (DirectionInputNew == DirectionInputCurrent)
	{
		if (DirectionInputNew == EDirectionInput::None)
		{
			DropInputTime = HorizontalInputTime + DeltaTime * DropSpeed;
		}
		else if (DirectionInputNew == EDirectionInput::Down)
		{
			DropInputTime = HorizontalInputTime + DeltaTime * MoveSpeedHolding;
		}
		else
		{
			HorizontalInputTime = HorizontalInputTime + DeltaTime * MoveSpeedHolding;
		}
	}
	else
	{
		HorizontalInputTime = 1.0f;
		DirectionInputCurrent = DirectionInputNew;
	}
}

void ATTetromino::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateInput();
	UpdateDirectionInput(DeltaTime);

	if (HorizontalInputTime >= 1.0f)
	{
		switch (DirectionInputCurrent)
		{
		case EDirectionInput::None:
		{
			
		}
			break;
		case EDirectionInput::Top:
		{
			FRotator PreRotateRotation = GetActorRotation();
			AddActorWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
		}
			break;
		case EDirectionInput::Down:
		{
			const FVector TetrominoLocationOld = GetActorLocation();
			const FVector TetrominoLocationNew = TetrominoLocationOld - FVector::DownVector * 100.0f;
			bool AllowMove = Grid->CanTetrominoMoveHorizontal(Blocks);
			if (AllowMove)
			{
				SetActorLocation(TetrominoLocationNew);
			}
			else
			{
				Destroy();
			}
		}
			break;
		case EDirectionInput::Left:
		{
			const FVector TetrominoLocationOld = GetActorLocation();
			const FVector TetrominoLocationNew = TetrominoLocationOld - FVector::LeftVector * 100.0f;
			bool AllowMove = Grid->CanTetrominoMoveHorizontal(Blocks);
			if (AllowMove)
			{
				SetActorLocation(TetrominoLocationNew);
			}
		}
			break;
		case EDirectionInput::Right:
		{

		}
			break;
		default:
			break;
		}
	}
}
