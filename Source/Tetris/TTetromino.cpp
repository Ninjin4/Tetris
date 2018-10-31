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

	MakeShape();
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

	MakeShape();
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
	MakeShape();
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

void ATTetromino::MakeShape()
{
	// Clear the shape, because Dimension might change
	Shape.Empty();

	// Create shape depending on ShapeLetter, Super Rotation System (SRS), http://tetris.wikia.com/wiki/SRS
	// TODO(Ninjin42): Replace with custom Matrix struct, this hardcoded style is super error-prone right now
	switch(ShapeLetter)
	{
		case EShapeLetter::SL_I:
		{
			ArrayDimension = 4;
			ArrayDimensionSquared = ArrayDimension * ArrayDimension;
			Shape.Reserve(ArrayDimensionSquared);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_T:
		{
			ArrayDimension = 3;
			ArrayDimensionSquared = ArrayDimension * ArrayDimension;
			Shape.Reserve(ArrayDimensionSquared);
			Shape.Add(0);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_O:
		{
			ArrayDimension = 2;
			ArrayDimensionSquared = ArrayDimension * ArrayDimension;
			Shape.Reserve(ArrayDimensionSquared);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
		}
			break;
		case EShapeLetter::SL_L:
		{
			ArrayDimension = 3;
			ArrayDimensionSquared = ArrayDimension * ArrayDimension;
			Shape.Reserve(ArrayDimensionSquared);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_J:
		{
			ArrayDimension = 3;
			ArrayDimensionSquared = ArrayDimension * ArrayDimension;
			Shape.Reserve(ArrayDimensionSquared);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_S:
		{
			ArrayDimension = 3;
			ArrayDimensionSquared = ArrayDimension * ArrayDimension;
			Shape.Reserve(ArrayDimensionSquared);
			Shape.Add(0);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
		}
			break;
		case EShapeLetter::SL_Z:
		{
			ArrayDimension = 3;
			ArrayDimensionSquared = ArrayDimension * ArrayDimension;
			Shape.Reserve(ArrayDimensionSquared);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(1);
			Shape.Add(1);
			Shape.Add(0);
			Shape.Add(0);
			Shape.Add(0);
		}
			break;
		default:
			break;
	}

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
	
	/*UE_LOG(LogTemp,Warning,TEXT("ArraySize: %d"), Shape.Num() );*/
}

FVector ATTetromino::IndexToRelativeLocation(int32 Index)
{
	// TODO(Ninjin42): Convert int32 to float properly later, function might be split to get an FVector2D with Row and Column,
	// this FVector2D could be used to represent the TArray like an N-Dimensional array, if there is no switch to a proper Matrix representation for the shape
	int32 Row = Index / ArrayDimension;
	int32 Column = Index % ArrayDimension;

	return FVector(SpriteSize * Column, 0.0f, SpriteSize * Row * -1.0f);
}

void ATTetromino::RotateClockwise()
{

}

void ATTetromino::RotateCounterClockwise()
{
}

void ATTetromino::RemoveBlock()
{
}
