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
}

// Called when the game starts or when spawned
void ATTetromino::BeginPlay()
{
	Super::BeginPlay();

	TetrominoCurrent = FMath::RandRange(0,Tetrominos.Num()-1);

	Tetrominos[TetrominoCurrent].CreateShape();

	ReorderBlocks();

	if(Tetrominos[TetrominoCurrent].PaperSprite != nullptr)
	{
		for(UPaperSpriteComponent* BlockCurrent : Blocks)
		{
			BlockCurrent->SetSprite(Tetrominos[TetrominoCurrent].PaperSprite);
			BlockCurrent->SetSpriteColor(Tetrominos[TetrominoCurrent].Color);
		}
	}
}

void ATTetromino::ReorderBlocks()
{
	// Iterate through the shape, find blocks and set the location of visual representation accordingly
	int32 LatestIndex = 0;
	int32 LatestBlock = 0;

	while(LatestIndex < Tetrominos[TetrominoCurrent].Shape.Num())
	{
		if(Tetrominos[TetrominoCurrent].Shape[LatestIndex] > 0)
		{
			Blocks[LatestBlock]->SetRelativeLocation(IndexToRelativeLocation(LatestIndex));
			LatestBlock++;
		}	
		LatestIndex++;
	}
}

void ATTetromino::RotateClockwise()
{
	Tetrominos[TetrominoCurrent].RotateClockwise();
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
	int32 Dimension = FMath::Sqrt(Tetrominos[TetrominoCurrent].Shape.Num());
	int32 Row = Index / Dimension;
	int32 Column = Index % Dimension;

	return FVector(Tetrominos[TetrominoCurrent].SpriteSize * Column, 0.0f, Tetrominos[TetrominoCurrent].SpriteSize * Row * -1.0f);
}


