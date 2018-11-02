// Fill out your copyright notice in the Description page of Project Settings.

#include "TTetromino.h"
#include "Classes/PaperSpriteComponent.h"
#include "Classes/PaperSprite.h"

// Sets default values
ATTetromino::ATTetromino()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneDefault = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneDefault;

	Block0 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block0"));
	Block0->SetupAttachment(RootComponent);
	Block1 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block1"));
	Block1->SetupAttachment(RootComponent);
	Block2 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block2"));
	Block2->SetupAttachment(RootComponent);
	Block3 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block3"));
	Block3->SetupAttachment(RootComponent);

	int32 BlocksCount = 4;
	Blocks.Reserve(BlocksCount);
	Blocks.Emplace(Block0);
	Blocks.Emplace(Block1);
	Blocks.Emplace(Block2);
	Blocks.Emplace(Block3);

	ShapeLetter = EShapeLetter::SL_I;
	Color = FLinearColor(FColor::Red);
	SpriteSize = 32.0f;

	CreateShape();
}

#if WITH_EDITOR
void ATTetromino::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PaperSprite != nullptr)
	{
		for(UPaperSpriteComponent* BlockCurrent : Blocks)
		{
			BlockCurrent->SetSprite(PaperSprite);
			BlockCurrent->SetSpriteColor(Color);
		}
	}

	CreateShape();
}
#endif // WITH_EDITOR

void ATTetromino::InitCustom(UPaperSprite* PaperSprite, FLinearColor Color)
{
	if(PaperSprite != nullptr)
	{
		for(UPaperSpriteComponent* BlockCurrent : Blocks)
		{
			BlockCurrent->SetSprite(PaperSprite);
			BlockCurrent->SetSpriteColor(Color);
		}
	}

	ShapeLetter = static_cast<EShapeLetter>( FMath::RandRange(0,6) );
	CreateShape();
}

// Called when the game starts or when spawned
void ATTetromino::BeginPlay()
{
	Super::BeginPlay();
	
	//if(PaperSprite != nullptr)
	//{
	//	for(UPaperSpriteComponent* BlockCurrent : Blocks)
	//	{
	//		BlockCurrent->SetSprite(PaperSprite);
	//		BlockCurrent->SetSpriteColor(Color);
	//	}
	//}
}

void ATTetromino::CreateShape()
{
	// TODO(NINJIN42): Find the corresponding function to resize the array inside the switch statement instead of deleting all elements and adding them 1 by 1
	Shape.Empty();

	// Create shape depending on ShapeLetter, Super Rotation System (SRS), http://tetris.wikia.com/wiki/SRS
	// TODO(Ninjin42): Replace with custom Matrix struct, this hardcoded style is super error-prone right now
	switch(ShapeLetter)
	{
		case EShapeLetter::SL_I:
		{
			Shape.Reserve(16);
			Shape.Add(0); Shape.Add(0); Shape.Add(0); Shape.Add(0);
			Shape.Add(1); Shape.Add(1); Shape.Add(1); Shape.Add(1);
			Shape.Add(0); Shape.Add(0); Shape.Add(0); Shape.Add(0);
			Shape.Add(0); Shape.Add(0); Shape.Add(0); Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_T:
		{
			Shape.Reserve(9);
			Shape.Add(0); Shape.Add(1); Shape.Add(0);
			Shape.Add(1); Shape.Add(1); Shape.Add(1);
			Shape.Add(0); Shape.Add(0); Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_O:
		{
			Shape.Reserve(4);
			Shape.Add(1); Shape.Add(1);
			Shape.Add(1); Shape.Add(1);
		}
			break;
		case EShapeLetter::SL_L:
		{
			Shape.Reserve(9);
			Shape.Add(0); Shape.Add(0); Shape.Add(1);
			Shape.Add(1); Shape.Add(1); Shape.Add(1);
			Shape.Add(0); Shape.Add(0); Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_J:
		{
			Shape.Reserve(9);
			Shape.Add(1); Shape.Add(0); Shape.Add(0);
			Shape.Add(1); Shape.Add(1); Shape.Add(1);
			Shape.Add(0); Shape.Add(0); Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_S:
		{
			Shape.Reserve(9);
			Shape.Add(0); Shape.Add(1); Shape.Add(1);
			Shape.Add(1); Shape.Add(1); Shape.Add(0);
			Shape.Add(0); Shape.Add(0); Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_Z:
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

	ReorderBlocks();
}

void ATTetromino::ReorderBlocks()
{
	// Iterate through the shape, find blocks and set the location of visual representation accordingly
	int32 LatestIndex = 0;
	int32 LatestBlock = 0;

	while(LatestIndex < Shape.Num())
	{
		if(Shape[LatestIndex] > 0)
		{
			Blocks[LatestBlock]->SetRelativeLocation(IndexToRelativeLocation(LatestIndex));
			LatestBlock++;
		}	
		LatestIndex++;
	}
}

void ATTetromino::RotateClockwise()
{
	// Inplace algorithm to swap indices around, 3*(Dimesion-1) swaps
	// Take the first element and swap with all corner elements, then take next element and swap with all next to corner elements
	// Full explanation at https://www.youtube.com/watch?v=Jtu6dJ0Cb94
	// TODO(NINJIN42): Actually explain the algorithm here with comment graphics

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

	ReorderBlocks();
}

void ATTetromino::RotateCounterClockwise()
{
}

void ATTetromino::RemoveBlock()
{
}

FVector ATTetromino::IndexToRelativeLocation(int32 Index)
{
	// TODO(Ninjin42): Convert int32 to float properly later, function might be split to get an FVector2D with Row and Column,
	// this FVector2D could be used to represent the TArray like an N-Dimensional array, if there is no switch to a proper Matrix representation for the shape
	int32 Dimension = FMath::Sqrt(Shape.Num());
	int32 Row = Index / Dimension;
	int32 Column = Index % Dimension;

	return FVector(SpriteSize * Column, 0.0f, SpriteSize * Row * -1.0f);
}

int32 ATTetromino::Index(int32 Row, int32 Column)
{
	int32 Dimension = FMath::Sqrt(Shape.Num());
	return Row * Dimension + Column;
}
