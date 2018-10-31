// Fill out your copyright notice in the Description page of Project Settings.

#include "TPawn.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TGrid.h"
#include "TTetromino.h"

// Sets default values
ATPawn::ATPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Place Pawn in level and simply possess instead of online PlayerStart workflow
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	#pragma region SpringArm and Camera Settings
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	#pragma endregion


}

// Called when the game starts or when spawned
void ATPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region Input and Movement
// Called to bind functionality to input
void ATPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ATPawn::MoveRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ATPawn::MoveLeft);
	PlayerInputComponent->BindAction("RotateClockwise", IE_Pressed, this, &ATPawn::RotateClockwise);
	PlayerInputComponent->BindAction("RotateCounterClockwise", IE_Pressed, this, &ATPawn::RotateCounterClockwise);
}

void ATPawn::MoveRight()
{

}

void ATPawn::MoveLeft()
{

}

void ATPawn::RotateClockwise()
{

}

void ATPawn::RotateCounterClockwise()
{

}
#pragma endregion 
