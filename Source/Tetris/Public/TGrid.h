// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGrid.generated.h"

class ATTetromino;
class UInstancedStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;

// The Grid is implemented in a 1D array, but functions operate on 2D logic for easier understanding
// Handles validation of the movement received from the Pawn (basically replaces Unreals collision sweep)
UCLASS()
class TETRIS_API ATGrid : public AActor
{
	GENERATED_BODY()

public:
	ATGrid();

	// Only here if Outer Grid will be replaced by other visual blocks and if it should be made procedural
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& propertyChangedEvent) override;
#endif

	// Interface for Pawn, might turn into lambda and call it CollisionCheck
	bool IsMoveHorizontalValid(const TArray<FTransform> tetrominoBlocks) const;
	bool IsMoveDownValid(const TArray<FTransform> tetrominoBlocks) const;
	bool IsRotationValid(const TArray<FTransform> tetrominoBlocks) const;

	void AddToGrid(const TArray<FTransform> tetrominoBlocks);

protected:
	virtual void BeginPlay() override;

	void InitGrid();
	void InitGridOuter();
	void CentreSpringArm();
	void SpawnNewTetromino();

	// Check functions when moving the Tetromino
	bool IsBlockOutOfBoundsVertical(const int32 tetrominoInstanceGridPositionX) const;
	bool IsBlockBelowGround(const int32 tetrominoInstanceGridPositionY) const;
	bool IsBlockOnTopOfGridBlock(const FIntPoint& tetrominoInstanceGridPosition) const;

	void CheckGridForFullLines();
	void DeleteRow(const int32 y);

	// Helper functions to convert Locations to GridPostions
	// TODO: Check if turning these to FORCEINLINE performs faster
	int32 FindIndex(const FIntPoint& gridPosition) const;
	// An instance scale of 0 determines if the gridpostion is already filled with a block
	bool IsGridBlockVisible(const int32 index) const;

	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Rows = 20;
	// Buffer area above the grid where Tetrominoes are spawned
	int32 RowsWithBuffer = Rows + 4;

	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Columns = 20;
	int32 CellCount = 0;

	// TODO: Currently used to spawn new Tetros, move to Gamemode or something
	UPROPERTY(EditAnywhere, Category = "Assign")
	TSubclassOf<ATTetromino> TetrominoBP;

	// TODO: Also used in Tetromino, make global, needs to be
	UPROPERTY(EditAnywhere, Category = "Assign")
	float BlockSize = 100.0f;

	// TODO: Delete when refactoring or turn into smart pointer
	UPROPERTY()
	ATTetromino* TTetromino;

	// Visual representation of blocks inside playing area
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* BlocksVisuals;

	// Visual representation of outer blocks to show max movement
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* OuterGridVisual;

	// Might be useful if designer wants to change to Perspective Projection
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	// Set to Orthographic by default
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	// The Grid, implemented as a 1D mesh array
	TArray<FTransform> Blocks;
	TArray<FTransform> BlocksOuter;

	// Helper function to find the Grid centre
	FVector SpawnLocation;
};
