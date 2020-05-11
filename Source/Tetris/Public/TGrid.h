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
	int32 Row;
	int32 Column;

	FIntVector2D()
	{
		Row = 0;
		Column = 0;
	}

	FIntVector2D(int32 Row, int32 Column)
		: Row(Row), Column(Column) {}
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

	TArray<FTransform> Blocks;

	// The Grid, implemented as a 1D mesh array
	UPROPERTY(VisibleDefaultsOnly)
	UInstancedStaticMeshComponent* BlocksVisual;

	UInstancedStaticMeshComponent* OuterGridVisual;

	// Might be useful if designer wants to change to Perspective Projection
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	// Set to Orthographic by default
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera; 

	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Columns;

	// Buffer area where Tetrominoes are spawned
	int32 RowsWithBuffer;

	// All possible Tetrominos are made in Blueprint
	UPROPERTY(EditAnywhere, Category = "Assign")
	TArray<TSubclassOf<ATTetromino>> TetrominoBPs;

	// Will spawn and possess a new Tetromino
	void SpawnNewTetromino();

	// Helper function to find the Grid centre
	FVector SpawnLocation;

	// Check functions when moving the Tetromino
	bool IsTetrominoOutOfBoundsVertical(TArray<FTransform> BlocksTetromino) const;
	bool IsTetrominoBelowGround(TArray<FTransform> BlocksTetromino) const;
	bool IsTetrominoBlockedByGridBlocks(TArray<FTransform> BlocksTetromino) const;

	void CheckGridForFullLines();
	void DeleteLine(int32 Row);

	// Helper functions to convert Locations to GridPostions
	// TODO: Check if turning these to FORCEINLINE performs faster
	int32 Index(FIntVector2D GridPosition) const;
	int32 ConvertBlockLocationToGridIndex(FTransform Block) const;
	FIntVector2D GetBlockPositionFromWorld(int32 Blockelement) const;
	// An instance scale of 0 determines if the gridpostion is already filled with a block
	bool IsGridBlockVisible(int32 Index) const;

public:	
	// Sets default values for this actor's properties
	ATGrid();

	// Only here if Outer Grid will be replaced by other visual blocks and if it should be made procedural
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	// Interface for Pawn
	bool CanTetrominoMoveHorizontal(TArray<FTransform> BlocksTetromino) const;
	bool CanTetrominoMoveDown(TArray<FTransform> BlocksTetromino) const;
	bool CanRotateTetromino(TArray<FTransform> BlocksTetromino) const;
	void AddToGrid(TArray<FTransform> BlocksTetromino);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
