// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetris/TPawn.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Tetris/TGrid.h"

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
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->SetOrthoWidth(2048.0f);
	#pragma endregion

	DropSpeed = 1.0;
	DropSpeedFast = 10.0f;
}

// Called when the game starts or when spawned
void ATPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Timeline Bindings
	FOnTimelineFloatStatic TetrominoDropTimelineCallback;
	FOnTimelineEventStatic TetrominoDropTimelineFinishedCallback;
	TetrominoDropTimelineCallback.BindUFunction(this, FName("OnUpdateTetrominoDrop"));
	TetrominoDropTimelineFinishedCallback.BindUFunction(this, FName("OnFinishTetrominoDrop"));
	TetrominoDropTimeline.AddInterpFloat(TetrominoDropCurve, TetrominoDropTimelineCallback);
	TetrominoDropTimeline.SetTimelineFinishedFunc(TetrominoDropTimelineFinishedCallback);

	// Timeline default values set to drop every second, all speed calculations will be made with playrate
	TetrominoDropTimeline.SetTimelineLength(1.0f);
	TetrominoDropTimeline.SetPlayRate(DropSpeed);
	TetrominoDropTimeline.PlayFromStart();
}

// Called every frame
void ATPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TetrominoDropTimeline.TickTimeline(DeltaTime);
}

#pragma region Input and Movement
// Called to bind functionality to input
void ATPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ATPawn::MoveRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ATPawn::MoveLeft);
	PlayerInputComponent->BindAction("RotateClockwise", IE_Pressed, this, &ATPawn::RotateClockwise);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ATPawn::MoveDownPressed);
	PlayerInputComponent->BindAction("MoveDown", IE_Released, this, &ATPawn::MoveDownReleased);
}

void ATPawn::MoveRight()
{
	Grid->MoveTetromino(FVector(100.0f, 0.0f, 0.0f));
}

void ATPawn::MoveLeft()
{
	Grid->MoveTetromino(FVector(-100.0f, 0.0f, 0.0f));
}

void ATPawn::MoveDownPressed()
{
	TetrominoDropTimeline.SetPlayRate(DropSpeedFast);
}

void ATPawn::MoveDownReleased()
{
	TetrominoDropTimeline.SetPlayRate(DropSpeed);
}

void ATPawn::RotateClockwise()
{
	Grid->RotateTetromino();
}
#pragma endregion 

void ATPawn::OnUpdateTetrominoDrop()
{
}

void ATPawn::OnFinishTetrominoDrop()
{
	Grid->MoveTetrominoDown(FVector(0.0f, 0.0f, -100.0f));
	TetrominoDropTimeline.PlayFromStart();
}
