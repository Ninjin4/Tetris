// Fill out your copyright notice in the Description page of Project Settings.

#include "TGrid.h"


// Sets default values
ATGrid::ATGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ATGrid::OnLanded(ATTetromino * TetrominoLast)
{
}

bool ATGrid::IsInBounds(ATTetromino * TetrominoCurrent)
{
	return true;
}

// Called when the game starts or when spawned
void ATGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

