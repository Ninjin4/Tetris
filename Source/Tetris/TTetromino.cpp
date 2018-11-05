// Fill out your copyright notice in the Description page of Project Settings.

#include "TTetromino.h"
#include "Classes/PaperSpriteComponent.h"
#include "Classes/PaperSprite.h"

// Sets default values
ATTetromino::ATTetromino()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneDefault = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneDefault;

	Block0 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block0"));
	Block0->SetupAttachment(RootComponent);
	Block0->bAbsoluteRotation = true;
	Block1 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block1"));
	Block1->SetupAttachment(RootComponent);
	Block0->bAbsoluteRotation = true;
	Block2 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block2"));
	Block2->SetupAttachment(RootComponent);
	Block0->bAbsoluteRotation = true;
	Block3 = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Block3"));
	Block3->SetupAttachment(RootComponent);
	Block0->bAbsoluteRotation = true;

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

FIntVector2D ATTetromino::GetGridPositionFromWorld(int32 Blockelement)
{
	FIntVector2D GridPosition = FIntVector2D(FMath::RoundToInt(Blocks[Blockelement]->GetComponentLocation().Z / 100.0f),
		FMath::RoundToInt(Blocks[Blockelement]->GetComponentLocation().X / 100.0f));
	return GridPosition;
}
