// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTetromino.generated.h"

class UPaperSpriteComponent;
class UPaperSprite;

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

// Google: A tetromino is a geometric shape composed of four squares, connected orthogonally.
// Includes the visual representation with meshes, and helper functions to check if they are in a valid position
UCLASS()
class TETRIS_API ATTetromino : public AActor
{
	GENERATED_BODY()

	// TODO(Ninjin42): Acts as a pivot point for rotation, but Tetrominos should have a rotation function that can be overridden for I and O shapes
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* SceneDefault;

	// Current visual representation of the Tetrominos, can be any type of mesh; created manually to allow inspectation in Editor
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block0;
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block1;
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block2;
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block3;

	// Helper variable to bulk edit Sprite Color
	UPROPERTY(EditAnywhere, Category = "Assign")
	FLinearColor Color;

public:
	// Save all Paperspritecomponents in an array, helpful to use foreach on all components
	// TODO(Ninjin42): Public access for Grid to call functions for PaperSpriteComponent, this kinda breaks encapsulation and needs a proper public function for Grid
	UPROPERTY(VisibleDefaultsOnly)
	TArray<UPaperSpriteComponent*> Blocks;

	// Sets default values for this actor's properties
	ATTetromino();

	// To see changes of the Color bulk edit in the editor
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	// Interface for Grid
	FIntVector2D GetBlockPositionFromWorld(int32 Blockelement);
	bool IsOutOfBoundsVertical(int32 Columns);
	bool IsBelowGround();
};
