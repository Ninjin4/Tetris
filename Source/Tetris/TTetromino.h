// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTetromino.generated.h"

class UPaperSpriteComponent;
class UPaperSprite;

// Each letter could derive from this class and implement their own Init and Rotation function, but using an enum is quick and dirty
// Also, could easily add a change letter function during Falling Phase
UENUM(BlueprintType)
enum class ELetter : uint8
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
// Representation with a flat out TArray, because multiple Dimension TArray seems to be broken and only a 4x4 Matrix is availabe in Unreal
// Contains parameters for a Tetronimo that the designer can change in a Blueprint
// Also has functions that operate on the Shape array data, could be split to ATTetronimo, but looks cleaner in the struct
// TODO(Ninjin42): Make a new Struct for 2x2, 3x3 and 4x4 Matrices with rotation functions, possibly abstract;
// TODO(NINJIN42): Also, possibly rename to FTetronimoData to not get confused by ATTetronimo
USTRUCT(BlueprintType)
struct FTetromino
{
    GENERATED_BODY()

public:
	// Needed to name the shapes for switch cases
	UPROPERTY(EditAnywhere, Category = "Assign")
	ELetter Letter;

	// Representation of Tetromino in a 1D array, because there is no multidimensional TArray
	TArray<int32> Shape;

	FVector2D Position;

	// Visual Representation of each block, will be assigned to every Block
	UPROPERTY(EditAnywhere, Category = "Assgin")
	UPaperSprite* PaperSprite;

	// PixelSize of Sprite to adjust block location, should be more global, i.e. in TGrid, but might want to change size dynamically when an event happen
	UPROPERTY(EditAnywhere, Category = "Assgin")
	float SpriteSize;

	// Color for every block, could also be entirely replaced by simply using different material instances
	UPROPERTY(EditAnywhere, Category = "Assgin")
	FLinearColor Color;

	// Create shape depending on ShapeLetter, Super Rotation System (SRS), http://tetris.wikia.com/wiki/SRS
	// TODO(Ninjin42): Replace with custom Matrix struct and/or use FVector4 for all shapes, but rotation will need another function to rotate around a pivot
	void CreateShape()
	{
		// TODO(NINJIN42): Find the corresponding function to resize the array inside the switch statement instead of deleting all elements and adding them 1 by 1
		Shape.Empty();

		switch(Letter)
		{
			case ELetter::SL_I:
			{
				Shape.Reserve(16);
				Shape.Add(0); Shape.Add(0); Shape.Add(0); Shape.Add(0);
				Shape.Add(1); Shape.Add(1); Shape.Add(1); Shape.Add(1);
				Shape.Add(0); Shape.Add(0); Shape.Add(0); Shape.Add(0);
				Shape.Add(0); Shape.Add(0); Shape.Add(0); Shape.Add(0);
			}
			break;
			case ELetter::SL_T:
			{
				Shape.Reserve(9);
				Shape.Add(0); Shape.Add(1); Shape.Add(0);
				Shape.Add(1); Shape.Add(1); Shape.Add(1);
				Shape.Add(0); Shape.Add(0); Shape.Add(0);
			}
			break;
			case ELetter::SL_O:
			{
				Shape.Reserve(4);
				Shape.Add(1); Shape.Add(1);
				Shape.Add(1); Shape.Add(1);
			}
			break;
			case ELetter::SL_L:
			{
				Shape.Reserve(9);
				Shape.Add(0); Shape.Add(0); Shape.Add(1);
				Shape.Add(1); Shape.Add(1); Shape.Add(1);
				Shape.Add(0); Shape.Add(0); Shape.Add(0);
			}
			break;
			case ELetter::SL_J:
			{
				Shape.Reserve(9);
				Shape.Add(1); Shape.Add(0); Shape.Add(0);
				Shape.Add(1); Shape.Add(1); Shape.Add(1);
				Shape.Add(0); Shape.Add(0); Shape.Add(0);
			}
			break;
			case ELetter::SL_S:
			{
				Shape.Reserve(9);
				Shape.Add(0); Shape.Add(1); Shape.Add(1);
				Shape.Add(1); Shape.Add(1); Shape.Add(0);
				Shape.Add(0); Shape.Add(0); Shape.Add(0);
			}
			break;
			case ELetter::SL_Z:
			{
				Shape.Reserve(9);
				Shape.Add(1); Shape.Add(1); Shape.Add(0);
				Shape.Add(0); Shape.Add(1); Shape.Add(1);
				Shape.Add(0); Shape.Add(0); Shape.Add(0);
			}
			break;
			default:
				break;
		}
	}

	// Inplace algorithm to swap indices around, 3*(Dimesion-1) swaps
	// Take the first element and swap with all corner elements, then take next element and swap with all next to corner elements
	// Full explanation at https://www.youtube.com/watch?v=Jtu6dJ0Cb94
	// TODO(NINJIN42): Actually explain the algorithm here with comment graphics
	void RotateClockwise()
	{
		int32 Dimension = FMath::Sqrt(Shape.Num());
		int32 Count = 0;
		int32 Level = 0;
		int32 Last = Dimension - 1;
		int32 TotalNumberOfLevels = Dimension / 2;

		while(Level < TotalNumberOfLevels)
		{
			for(int32 i = Level; i < Last; i++)
			{
				Shape.Swap(Index(Level, i), Index(i, Last));
				Shape.Swap(Index(Level, i), Index(Last, Last - i + Level));
				Shape.Swap(Index(Level, i), Index(Last - i + Level, Level));
			}
			Level++;
			Last--;
		}
	}

	// Helper function for MultiDimension to 1Dimension conversion
	FORCEINLINE int32 Index(int32 Row, int32 Column)
	{
		int32 Dimension = FMath::Sqrt(Shape.Num());
		return Row * Dimension + Column;
	}
};

// Visual Representation of FTetronimo
UCLASS()
class TETRIS_API ATTetromino : public AActor
{
	GENERATED_BODY()

	// Saves all possible Shapes, can be used to change shape during gameplay
	// Can also be placed in game loop class
	UPROPERTY(EditAnywhere, Category = "Assign")
	TArray<FTetromino> Tetrominos;

	// Current Tetromino, Will be randomized when Tetromino is being created, many functions will use this
	int32 TetrominoCurrent;

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
	// But still put into array to be able to use foreach loops
	UPROPERTY(VisibleDefaultsOnly)
	TArray<UPaperSpriteComponent*> Blocks;

	// Reorder block sprite component
	void ReorderBlocks();

	// Helper function to convert One Dimensional Array to N-Dimensional Rows and Columns
	FVector IndexToRelativeLocation(int32 Index);

public:	
	// Sets default values for this actor's properties
	ATTetromino();

	// Functions to call from PlayerPawn
	void RotateClockwise();
	void RotateCounterClockwise();

	// Remove the block and destroy the actor completely if no other PaperSpriteComponent exists
	void RemoveBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};

