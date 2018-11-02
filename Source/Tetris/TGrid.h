// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TGrid.generated.h"

class ATTetromino;

UCLASS()
class TETRIS_API ATGrid : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<int32> Grid;

	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Rows;
	UPROPERTY(EditAnywhere, Category = "Assign")
	int32 Columns;

public:	
	// Sets default values for this actor's properties
	ATGrid();

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	// Add the Tetromino to the Grid
	void OnLanded(ATTetromino* TetrominoLast);

	// Check if the Tetromino is still in the game area
	// TODO(Ninjin): Decide whether to use Tetromino directly and save a dependency or make the Tetromino Shapearray public (or stay private and copy that small thing)
	bool IsInBounds(ATTetromino* TetrominoCurrent);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
