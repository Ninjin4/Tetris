 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "TTetromino.generated.h"

UENUM()
enum class EDirectionInput : uint8
{
	None,
    Top,
	Down,
	Left,
	Right
};

UENUM(BlueprintType)
enum class ERotationRule : uint8
{
	None,
    Centre,
	FlipFlop
};

USTRUCT(BlueprintType)
struct FTetrominoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<bool> Shape;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERotationRule RotationRule;
};

class UInstancedStaticMeshComponent;
class ATGrid;

// "A tetromino is a geometric shape composed of four squares, connected orthogonally." - Google 
// Includes the visual representation with meshes, and helper functions to check if they are in a valid position
// Will be possessed by a PlayerController
UCLASS()
class TETRIS_API ATTetromino : public APawn
{
	GENERATED_BODY()

	// Input names
	static const FName MoveRightName;
	static const FName MoveUpName;

	UPROPERTY(EditAnywhere, Category = "Assign")
	class UDataTable* TetrominoDataTable;

	// Used internally to update Transforms, it's also faster than GetTransformInstance
	// Easier to deal with in Niagara
	TArray<FTransform> Blocks;

	// Visual representation of the Tetrominos, can be any type of mesh
	UPROPERTY(VisibleDefaultsOnly)
	UInstancedStaticMeshComponent* BlocksVisual;

	UPROPERTY(EditAnywhere, Category = "Assign")
	float BlockSize;

	// Player Input Functions
	void MoveRight(float Strength);
	void MoveUp(float Strength);
	void UpdateInput();
	void UpdateDirectionInput(float DeltaTime);
	FVector2D InputMoveRaw;
	FVector2D InputMoveRemap;
	EDirectionInput DirectionInputCurrent;
	UPROPERTY(EditAnywhere, Category = "Assign")
	float DeadZoneCentre;
	float DeadZoneCentreSquared;

	float HorizontalInputTime;
	float DropInputTime;

	// Speed with no input
	UPROPERTY(EditAnywhere, Category = "Assign")
	float DropSpeed;

	// Speed (dropping and horizontal movement) when holding
	UPROPERTY(EditAnywhere, Category = "Assign")
	float MoveSpeedHolding;

	// Instead of using Unreals global collision checks, check valid moves inside Grid gamestate class
	UPROPERTY()
	ATGrid* Grid;

public:
	// Sets default values for this actor's properties
	ATTetromino();
	
#if WITH_EDITOR
	// Update DeadZoneCentreSquared
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

protected:
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void Tick(float DeltaTime) override;
};
