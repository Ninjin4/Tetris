 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TTetrominoRotationRule.h"
#include "TTetromino.generated.h"

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

	static const FName MoveRightName;
	static const FName MoveUpName;
	static const FString ContextString;

public:
	ATTetromino();

	virtual void Tick(float DeltaTime) override;

	void GenerateTetromino(const int32 gridColumns, ATGrid* gridNew);

protected:

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Player Input Functions
	void DropPressed();
	void MoveRightPressed();
	void MoveRightReleased();
	void MoveLeftPressed();
	void MoveLeftReleased();
	void MoveDownPressed();
	void MoveDownReleased();
	void RotateClockwisePressed();
	void RotateClockwiseReleased();
	void RotateCounterClockwisePressed();
	void RotateCounterClockwiseReleased();

	void UpdateInputTimers(float deltaTime);

	// TODO: Think about changing function args to a bool called Positive
	// TODO: Refactor since they share quite a few lines
	void CheckAndMoveHorizontal(float direction);
	void CheckAndRotate(float direction);
	void CheckAndMoveDown();

	UPROPERTY(EditAnywhere, Category = "Assign")
	UDataTable* TetrominoDataTable;

	// Visual representation of the Tetrominos, can be any type of mesh
	// Will manually updated in world space
	UPROPERTY(VisibleDefaultsOnly)
	UInstancedStaticMeshComponent* BlocksVisuals;

	UPROPERTY(EditAnywhere, Category = "Assign")
	float BlockSize = 100.f;

	// Speed with no input
	UPROPERTY(EditAnywhere, Category = "Assign")
	float DropSpeed = 3.f;

	// Speed (dropping and horizontal movement) when holding
	UPROPERTY(EditAnywhere, Category = "Assign")
	float MoveSpeed = 9.f;

	// Speed (dropping and horizontal movement) when holding
	UPROPERTY(EditAnywhere, Category = "Assign")
	float RotationSpeed = 3.f;

	// TODO Turn into smart pointer
	// Instead of using Unreals global collision checks, check valid moves inside Grid gamestate class
	UPROPERTY()
	ATGrid* Grid;

	// TODO: Turn into FIntPoint where ColumnCount and RowCount can be different
	int32 BlocksDimensionCur;
	FVector ScaleFromColorCur;
	ERotationRule RotationRuleCur;

	TArray<FTransform> BlocksTransformsWorld;
	FVector PivotLocationWorld;

	float HorizontalInputTime;
	float RotationInputTime;
	float DropInputTime;

	uint8 MoveRight : 1;
	uint8 MoveLeft : 1;
	uint8 RotateClockwise : 1;
	uint8 RotateCounterClockwise : 1;
	uint8 MoveDown : 1;

	FRandomStream SpawnStream;
};
