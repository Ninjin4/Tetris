// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATGrid;
class ATTetromino;
class UPaperSprite;

// Handles all game logic, because it has easy access to input, controllers, is already part of gamemode
// Could shift game logic to Gamemode, Gamestate, TLevelScriptActor or even Controller
// Design Suggestion: Possess a single Tetromino and try to implement the game logic around the framework, but that needs more thoughts
UCLASS()
class TETRIS_API ATPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera; 

	UPROPERTY(EditAnywhere)
	ATGrid* Grid;

	UPROPERTY(VisibleAnywhere)
	ATTetromino* TetronimoCurrent;

	// Visual Representation of each block
	UPROPERTY(EditAnywhere, Category = "Assgin")
	UPaperSprite* PaperSprite;

	// PixelSize of Sprite to adjust block location
	UPROPERTY(EditAnywhere, Category = "Assgin")
	float SpriteSize;

	// Color for every block
	UPROPERTY(EditAnywhere, Category = "Assgin")
	FLinearColor Color;

	// Player Input Functions
	void MoveRight();
	void MoveLeft();
	void RotateClockwise();
	void MoveDown();

	void SpawnNewTetronimo();

	// Timer to drop Tetromino every X seconds
	FTimerHandle DropTetrominoOneUnitHandle;

	UFUNCTION()
	void OnDropTetrominoOneUnit();

public:
	// Sets default values for this pawn's properties
	ATPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
};
