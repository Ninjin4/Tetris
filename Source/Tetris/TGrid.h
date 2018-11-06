// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTetromino.h"
#include "TGrid.generated.h"

class ATTetromino;

// The Grid is implemented in a 1D array, but functions operate on 2D logic for easier understanding
// It can possess a Tetromino and handles validation of the movement received from the Pawn
UCLASS()
class TETRIS_API ATGrid : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneDefault;

	// The Grid, implemented as a 1D mesh array
	UPROPERTY(VisibleDefaultsOnly)
	TArray<UPaperSpriteComponent*> Blocks;

	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Columns;

	// Buffer on top when Tetromino and player rotates right after spawn, will be used for array access instead of Rows
	int32 RowsBuffer;

	// All possible Tetrominos are made in Blueprint
	UPROPERTY(EditAnywhere, Category = "Assign")
	TArray<TSubclassOf<ATTetromino>> TetrominoBPs;

	// The current of the possible Tetrominos made in Blueprint that will be possessed
	UPROPERTY(VisibleDefaultsOnly)
	ATTetromino* TetrominoCurrent;

	// Will spawn and possess a new Tetromino
	void SpawnNewTetromino();

	// Helper function to find the Grid centre
	FVector SpawnLocation();

	// Check functions when moving the Tetromino
	bool IsTetrominoOutOfBoundsVertical();
	bool IsTetrominoBelowGround();
	bool AreTetrominoBlocksBlockedByBlocks();

	void AddToGrid();
	void CheckGridForFullLines();
	void DeleteLine(int32 Row);

public:	
	// Sets default values for this actor's properties
	ATGrid();

	// Only here if Outer Grid will be replaced by other visual blocks and if it should be made procedural
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	// Interface for Pawn
	void MoveTetromino(FVector Direction);
	void MoveTetrominoDown(FVector Direction);
	void RotateTetromino();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Calculates the corresponding Index for the 2D Location
	FORCEINLINE int32 Index(FIntVector2D GridPosition)
	{
		return GridPosition.Row * Columns + GridPosition.Column;
	}
};
