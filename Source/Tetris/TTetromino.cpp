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

// Called when the game starts or when spawned
void ATTetromino::BeginPlay()
{
	Super::BeginPlay();

}

void ATTetromino::RotateClockwise()
{
	AddActorWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	//for(UPaperSpriteComponent* Block : Blocks)
	//{
	//	Block->AddRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	//}
}

void ATTetromino::RotateCounterClockwise()
{
}

FVector2D ATTetromino::GetWorldIndices(int32 Blockelement)
{
	return FVector2D(Blocks[Blockelement]->GetComponentLocation().X, Blocks[Blockelement]->GetComponentLocation().Z);
}

void ATTetromino::RemoveBlock()
{

}

