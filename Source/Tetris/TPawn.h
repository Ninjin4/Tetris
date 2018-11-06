// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "TPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATGrid;
class UPaperSprite;

// TPawn contains mostly input logic and has access to the Grid to call the corresponding functions, it also triggers the automatic down movement with a timeline
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
	void MoveDownPressed();
	void MoveDownReleased();

	// Timeline to drop Tetromino every second
	FTimeline TetrominoDropTimeline;

	// The corresponding update function
	UFUNCTION()
	void OnUpdateTetrominoDrop();

	// The corresponding finish function
	UFUNCTION()
	void OnFinishTetrominoDrop();

	// Timeline needs a dummy curve to read values on the update function
	UPROPERTY(EditAnywhere, Category = "Assign")
	UCurveFloat* TetrominoDropCurve;

	// Normal Playrate
	UPROPERTY(EditAnywhere, Category = "Assign")
	float DropSpeed;

	// Playrate when player presses down
	UPROPERTY(EditAnywhere, Category = "Assign")
	float DropSpeedFast;

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
