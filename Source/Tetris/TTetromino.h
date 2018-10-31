// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTetromino.generated.h"

class UPaperSpriteComponent;
class UPaperSprite;

// Each letter could derive from this class and implement their own Init and Rotation function, but using an enum is quick and dirty
// Also, could easily add a change letter function during Falling Phase
UENUM(/*BlueprintType*/) // Disabled for now since Code is might be written in C++ entirely
enum class EShapeLetter : uint8
{
    SL_I	UMETA(DisplayName = "I"),
    SL_T	UMETA(DisplayName = "T"),
	SL_O	UMETA(DisplayName = "O"),
	SL_L	UMETA(DisplayName = "L"),
	SL_J	UMETA(DisplayName = "J"),
	SL_S	UMETA(DisplayName = "S"),
	SL_Z	UMETA(DisplayName = "Z")
};

// Google: "A tetromino is a geometric shape composed of four squares, connected orthogonally"
// Representation with a flat out TArray, because multiple Dimension TArray seem to be broken and only a 4x4 Matrix is availabe in Unreal
// TODO(Ninjin42): Make a new Struct for 2x2, 3x3 and 4x4 Matrices with rotation functions, possibly abstract
UCLASS()
class TETRIS_API ATTetromino : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Assign")
	EShapeLetter ShapeLetter;
	int32 ArrayDimension;
	int32 ArrayDimensionSquared;
	TArray<int32> Shape;

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* SceneDefault;

	// Created manually to inspect in Editor
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block0;
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block1;
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block2;
	UPROPERTY(VisibleDefaultsOnly)
	UPaperSpriteComponent* Block3;
	// But still put into array for neat foreach loops functions
	UPROPERTY(VisibleDefaultsOnly)
	TArray<UPaperSpriteComponent*> Blocks;

	// Visual Representation of each block
	UPROPERTY(EditAnywhere, Category = "Assgin")
	UPaperSprite* PaperSprite;

	// PixelSize of Sprite to adjust block location
	UPROPERTY(EditAnywhere, Category = "Assgin")
	float SpriteSize;

	// Color for every block
	UPROPERTY(EditAnywhere, Category = "Assgin")
	FLinearColor Color;

	// Helper function when EShapeLetter is changed
	void MakeShape();

	// Helper function to convert One Dimensional Array to N-Dimensional Rows and Columns
	FVector IndexToRelativeLocation(int32 Index);

public:	
	// Sets default values for this actor's properties
	ATTetromino();

	// Will be called after spawning the Tetromino
	void InitCustom(UPaperSprite* PaperSprite, FLinearColor Color);

	// Change EShapeLetter and Material
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	// Functions to call from PlayerPawn
	void RotateClockwise();
	void RotateCounterClockwise();

	// Remove the block and destroy the actor completely if no other PaperSpriteComponent exists
	void RemoveBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};

