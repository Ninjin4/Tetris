// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ATGrid;
class ATTetronimo;

// Implements all game logic, because it has easy access to input, controllers, is already part of gamemode
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
	ATTetronimo* TetronimoCurrent;

	void MoveRight();
	void MoveLeft();
	void RotateClockwise();
	void RotateCounterClockwise();

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
