// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTetromino.generated.h"

class UPaperSpriteComponent;
class UPaperSprite;

USTRUCT(BlueprintType)
struct FIntVector2D
{
    GENERATED_BODY()
	
public:
	int32 Row;
	int32 Column;

	FIntVector2D()
	{
		Row = 99;
		Column = 99;
	}

	FIntVector2D(int32 Row, int32 Column)
		: Row(Row), Column(Column) {}
};

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

	// Helper variable to bulk edit Sprite Color
	UPROPERTY(EditAnywhere, Category = "Assign")
	FLinearColor Color;

public:
	// Save all Paperspritecomponents in an array, so you can use foreach on all components
	// TODO(NINJIN42): Public access for Grid to call functions for PaperSpriteComponent, this kinda breaks encapsulation and needs a proper public function for Grid
	UPROPERTY(VisibleDefaultsOnly)
	TArray<UPaperSpriteComponent*> Blocks;

	// Sets default values for this actor's properties
	ATTetromino();

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif // WITH_EDITOR

	FIntVector2D GetGridPositionFromWorld(int32 Blockelement);

};
