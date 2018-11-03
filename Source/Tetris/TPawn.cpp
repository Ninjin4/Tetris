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
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->SetOrthoWidth(2048.0f);
	#pragma endregion

}

// Called when the game starts or when spawned
void ATPawn::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnNewTetromino();
	
	GetWorldTimerManager().SetTimer(DropTetrominoOneUnitHandle, this, &ATPawn::OnDropTetrominoOneUnit, 1.0f, true, 2.0f);

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
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ATPawn::MoveDown);
}

void ATPawn::MoveRight()
{
	Grid->MoveTetromino(TetrominoCurrent, FVector(100.0f, 0.0f, 0.0f));
}

void ATPawn::MoveLeft()
{
	Grid->MoveTetromino(TetrominoCurrent, FVector(-100.0f, 0.0f, 0.0f));
}

void ATPawn::MoveDown()
{
	Grid->MoveTetromino(TetrominoCurrent, FVector(0.0f, 0.0f, -100.0f));
}

void ATPawn::RotateClockwise()
{
	Grid->RotateTetromino(TetrominoCurrent);
}
#pragma endregion 

void ATPawn::OnDropTetrominoOneUnit()
{
	// TODO(NINJIN42): Replace -100 by making SpriteSize public or implementing a FORCEINLINE Getter
	TetrominoCurrent->AddActorWorldOffset(FVector(0.0f, 0.0f, -100.0f));
}

void ATPawn::SpawnNewTetromino()
{
	int32 RandomTetromino = FMath::RandRange(0, TetrominoBPs.Num() - 1);
	// TODO(NINJIN42): Use Deferred Spawning to wait for spawning until InitCustom is finished
	TetrominoCurrent = GetWorld()->SpawnActor<ATTetromino>(TetrominoBPs[RandomTetromino], Grid->SpawnLocation(), FRotator(0.0f));
	
	// The Deferred Spawning functions
	//FTransform SpawnTransform(Rotation, Origin);
	//UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TetrominoCurrent::StaticClass(), SpawnTransform));
	//if(TetrominoCurrent != nullptr)
	//{
	//	MyDeferredActor->Init(ShootDir);
	//	UGameplayStatics::FinishSpawningActor(TetrominoCurrent, SpawnTransform);
	//}
}

