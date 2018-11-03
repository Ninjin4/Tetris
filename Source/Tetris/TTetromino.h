// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTetromino.generated.h"

class UPaperSpriteComponent;
class UPaperSprite;

UCLASS()
class TETRIS_API ATTetromino : public AActor
{
	GENERATED_BODY()

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

	// Helper variable to bulk edit Sprite Color
	UPROPERTY(EditAnywhere, Category = "Assign")
	FLinearColor Color;

public:	
	// Sets default values for this actor's properties
	ATTetromino();

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	// Functions to call from PlayerPawn
	void RotateClockwise();
	void RotateCounterClockwise();

	FVector2D GetWorldIndices(int32 Blockelement);

	// Remove the block and destroy the actor completely if no other PaperSpriteComponent exists
	void RemoveBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};

