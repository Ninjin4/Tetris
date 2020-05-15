 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "TTetromino.generated.h"

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
	TArray<bool> Blocks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERotationRule RotationRule;
};

class UInstancedStaticMeshComponent;
class ATGrid;
class UDataTable;

// "A tetromino is a geometric shape composed of four squares, connected orthogonally." - Google 
// Includes the visual representation with meshes, and helper functions to check if they are in a valid position
// Will be possessed by a PlayerController
UCLASS()
class TETRIS_API ATTetromino : public APawn
{
	GENERATED_BODY()

protected:
	// Input names
	static const FName MoveRightName;
	static const FName MoveUpName;
	static const FString ContextString;

	UPROPERTY(EditAnywhere, Category = "Assign")
	UDataTable* TetrominoDataTable;

	// TODO: Turn into FIntVector2D where ColumnCount and RowCount can be different
	int32 BlocksDimension;
	FVector ScaleFromColor;
	ERotationRule RotationRule;
	UPROPERTY(EditAnywhere, Category = "Assign")
	float BlockSize;

	// All transforms are in world space
	TArray<FTransform> Blocks;
	FVector PivotLocationWorld;

	// Visual representation of the Tetrominos, can be any type of mesh
	// Will manually updated in world space
	UPROPERTY(VisibleDefaultsOnly)
	UInstancedStaticMeshComponent* BlocksVisuals;

	// Player Input Functions
	void DropPressed();
	void MoveRightPressed();
	void MoveRightReleased();
	void MoveLeftPressed();
	void MoveLefttReleased();
	void MoveDownPressed();
	void MoveDownReleased();
	void RotateClockwisePressed();
	void RotateClockwiseReleased();
	void RotateCounterClockwisePressed();
	void RotateCounterClockwiseReleased();

	uint8 MoveRight : 1;
	uint8 MoveLeft : 1;
	uint8 RotateClockwise : 1;
	uint8 RotateCounterClockwise : 1;
	uint8 MoveDown : 1;

	void UpdateInputTimers(float DeltaTime);

	float HorizontalInputTime;
	float RotationInputTime;
	float DropInputTime;

	// TODO: Think about changing function args to a bool called Positive
	// TODO: Refactor since they share quite a few lines
	void CheckAndMoveHorizontal(float Direction);
	void CheckAndRotate(float Direction);
	void CheckAndMoveDown();

	void UpdateInstances();

	// Speed with no input
	UPROPERTY(EditAnywhere, Category = "Assign")
	float DropSpeed;

	// Speed (dropping and horizontal movement) when holding
	UPROPERTY(EditAnywhere, Category = "Assign")
	float MoveSpeed;

	// Speed (dropping and horizontal movement) when holding
	UPROPERTY(EditAnywhere, Category = "Assign")
	float RotationSpeed;

	// Instead of using Unreals global collision checks, check valid moves inside Grid gamestate class
	UPROPERTY()
	ATGrid* Grid;

	FRandomStream SpawnStream;

public:
	// Sets default values for this actor's properties
	ATTetromino();

	// TODO: I don't really like where this is going :/
	void SpawnTetromino(int32 GridColumns, ATGrid* GridNew);
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

protected:
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void Tick(float DeltaTime) override;
};
