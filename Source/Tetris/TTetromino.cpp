// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/TTetromino.h"
#include "Classes/PaperSprite.h"
#include "Classes/PaperSpriteComponent.h"

// Sets default values
ATTetromino::ATTetromino()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneDefault = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneDefault;

	Block0 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block0"));
	Block0->SetupAttachment(RootComponent);
	Block0->SetUsingAbsoluteRotation(true);
	Block1 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block1"));
	Block1->SetupAttachment(RootComponent);
	Block1->SetUsingAbsoluteRotation(true);
	Block2 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block2"));
	Block2->SetupAttachment(RootComponent);
	Block2->SetUsingAbsoluteRotation(true);
	Block3 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block3"));
	Block3->SetupAttachment(RootComponent);
	Block3->SetUsingAbsoluteRotation(true);

	int32 BlocksCount = 4;
	Blocks.Reserve(BlocksCount);
	Blocks.Emplace(Block0);
	Blocks.Emplace(Block1);
	Blocks.Emplace(Block2);
	Blocks.Emplace(Block3);
}

#if WITH_EDITOR
void ATTetromino::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for(UPaperSpriteComponent* Block : Blocks)
	{
		Block->SetSpriteColor(Color);
	}
}
#endif // WITH_EDITOR

FIntVector2D ATTetromino::GetBlockPositionFromWorld(int32 Blockelement)
{
	FIntVector2D GridPosition = FIntVector2D(FMath::RoundToInt(Blocks[Blockelement]->GetComponentLocation().Z / 100.0f),
		FMath::RoundToInt(Blocks[Blockelement]->GetComponentLocation().X / 100.0f));
	return GridPosition;
}

bool ATTetromino::IsOutOfBoundsVertical(int32 Columns)
{
	for(UPaperSpriteComponent* Block : Blocks)
	{
		int32 Column = FMath::RoundToInt(Block->GetComponentLocation().X / 100.0f);
		if(Column < 0 || Column >= Columns)
		{
			return true;
		}
	}
	return false;
}

bool ATTetromino::IsBelowGround()
{
	for(UPaperSpriteComponent* Block : Blocks)
	{
		if(FMath::RoundToInt(Block->GetComponentLocation().Z / 100.0f) < 0)
		{
			return true;
		}
	}
	return false;
}
