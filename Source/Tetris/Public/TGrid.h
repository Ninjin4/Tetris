// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGrid.generated.h"

// Helper struct to hold the world position of the Tetrominos in a 2D array format
USTRUCT(BlueprintType)
struct FIntVector2D
{
    GENERATED_BODY()
	
public:
	int32 X;
	int32 Y;

	FIntVector2D()
	{
		X = 0;
		Y = 0;
	}

	FIntVector2D(int32 X, int32 Y)
		: X(X), Y(Y) {}
};

class ATTetromino;
class UInstancedStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
// TODO: Grid needs to know ERotationRule, but now Grid and Tetromino need to know each other
class ATTetromino;

// The Grid is implemented in a 1D array, but functions operate on 2D logic for easier understanding
// Handles validation of the movement received from the Pawn (basically replaces Unreals collision sweep)
UCLASS()
class TETRIS_API ATGrid : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Columns;
	int32 CellCount;

	// TODO: Delete when refactoring
	UPROPERTY()
	ATTetromino* TTetromino;

	// The Grid, implemented as a 1D mesh array
	TArray<FTransform> Blocks;
	TArray<FTransform> BlocksOuter;

	// Visual representation
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* BlocksVisuals;

	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* OuterGridVisual;

	// Might be useful if designer wants to change to Perspective Projection
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	// Set to Orthographic by default
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera; 

	// Buffer area where Tetrominoes are spawned
	int32 RowsWithBuffer;

	// TODO: Currently used to spawn new Tetros, move to Gamemode or something
	UPROPERTY(EditAnywhere, Category = "Assign")
	TSubclassOf<ATTetromino> TetrominoBP;

	// Will spawn and possess a new Tetromino
	void InitGrid();
	void InitGridOuter();
	void SpawnNewTetromino();

	// Helper function to find the Grid centre
	FVector SpawnLocation;

	// Check functions when moving the Tetromino
	bool IsBlockOutOfBoundsVertical(int32 TetrominoInstanceGridPositionX) const;
	bool IsBlockBelowGround(int32 TetrominoInstanceGridPositionY) const;
	bool IsBlockOnTopOfGridBlock(FIntVector2D TetrominoInstanceGridPosition) const;

	void CheckGridForFullLines();
	void DeleteRow(int32 Y);
	// TODO: Also used in Tetromino, make global, needs to be
	UPROPERTY(EditAnywhere, Category = "Assign")
	float BlockSize;

	// Helper functions to convert Locations to GridPostions
	// TODO: Check if turning these to FORCEINLINE performs faster
	int32 FindIndex(FIntVector2D GridPosition) const;
	// An instance scale of 0 determines if the gridpostion is already filled with a block
	bool IsGridBlockVisible(int32 Index) const;

	// TODO: Refactor, same code can be found in TTetromino
	void UpdateInstances(TArray<FTransform> Blocks);

public:	
	// Sets default values for this actor's properties
	ATGrid();

	// Only here if Outer Grid will be replaced by other visual blocks and if it should be made procedural
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	// Interface for Pawn, might turn into lambda and call it CollisionCheck
	bool IsMoveHorizontalValid(const TArray<FTransform> TetrominoBlocks) const;
	bool IsMoveDownValid(const TArray<FTransform> TetrominoBlocks) const;
	bool IsRotationValid(const TArray<FTransform> TetrominoBlocks) const;
	
	void AddToGrid(const TArray<FTransform> TetrominoBlocks);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
