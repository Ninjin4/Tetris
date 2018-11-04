// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATGrid;
class UPaperSprite;

// Handles all game logic, because it has easy access to input, controllers, is already part of gamemode
// Could shift game logic to Gamemode, Gamestate, TLevelScriptActor or even Controller
// Design Suggestion: Possess a single Tetromino and try to implement the game logic around the framework, but that needs more thoughts
UCLASS()
class TETRIS_API ATPawn : public APawn
{
	GENERATED_BODY()

	// Might be useful if designer wants to change to Perspective Projection
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	// Set to Orthographic by default
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera; 

	// Calls functions to Move/Rotate the Tetromino
	UPROPERTY(EditAnywhere, Category = "Assign")
	ATGrid* Grid;



	// Player Input Functions
	void MoveRight();
	void MoveLeft();
	void RotateClockwise();
	void MoveDown();

	// Spawns a new Tetromino
	void SpawnNewTetromino();

	// Timer to drop Tetromino every X seconds
	FTimerHandle DropTetrominoOneUnitHandle;

	// The corresponding function that will be called
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
