// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTetromino.h"
#include "TGrid.generated.h"

class ATTetromino;

UCLASS()
class TETRIS_API ATGrid : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneDefault;

	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Columns;

	// Buffer on top when Tetromino and player rotates
	int32 RowsBuffer;

	void SpawnNewTetromino();

	// Helper function to find the array centre
	FVector SpawnLocation();

	// All possible Tetrominos are made in Blueprint
	UPROPERTY(EditAnywhere, Category = "Assign")
	TArray<TSubclassOf<ATTetromino>> TetrominoBPs;

	// The current of the possible Tetrominos made in Blueprint
	UPROPERTY(VisibleDefaultsOnly)
	ATTetromino* TetrominoCurrent;

	UPROPERTY(VisibleDefaultsOnly)
	TArray<UPaperSpriteComponent*> Blocks;

	// Helper functions when moving the Tetromino
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

	void MoveTetromino(FVector Direction);

	void MoveTetrominoDown(FVector Direction);

	void RotateTetromino();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Calculates the corresponding Index for the 2D Location
	FORCEINLINE int32 Index(FIntVector2D GridPosition)
	{
		return GridPosition.Row * Columns + GridPosition.Column;
	}
};
