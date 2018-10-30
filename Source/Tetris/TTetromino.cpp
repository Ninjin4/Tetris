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

// Called when the game starts or when spawned
void ATTetromino::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATTetromino::MakeShape()
{
	// Clear the shape, because Dimension might change
	Shape.Empty();

	// Create shape depending on ShapeLetter, Super Rotation System (SRS)
	// TODO(Ninjin42): Replace with custom Matrix struct
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

	int32 LatestIndex = 0;
	int32 LatestBlock = 0;
	
	/*UE_LOG(LogTemp,Warning,TEXT("ArraySize: %d"), Shape.Num() );*/

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

FVector ATTetromino::IndexToRelativeLocation(int32 Index)
{
	int32 Row = Index / ArrayDimension;
	int32 Column = Index % ArrayDimension;

	return FVector(SpriteSize * Column, 0.0f, SpriteSize * Row);
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
