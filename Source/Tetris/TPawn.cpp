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
	
	SpawnNewTetronimo();
	
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
	// TODO(NINJIN42): Replace -32 by making SpriteSize public or implementing a FORCEINLINE Getter
	TetronimoCurrent->AddActorWorldOffset(FVector(32.0f, 0.0f, 0.0f));
}

void ATPawn::MoveLeft()
{
	// TODO(NINJIN42): Replace -32 by making SpriteSize public or implementing a FORCEINLINE Getter
	TetronimoCurrent->AddActorWorldOffset(FVector(-32.0f, 0.0f, 0.0f));
}

void ATPawn::RotateClockwise()
{
	TetronimoCurrent->RotateClockwise();
}

void ATPawn::MoveDown()
{

}
#pragma endregion 

void ATPawn::OnDropTetrominoOneUnit()
{
	// TODO(NINJIN42): Replace -32 by making SpriteSize public or implementing a FORCEINLINE Getter
	TetronimoCurrent->AddActorWorldOffset(FVector(0.0f, 0.0f, -32.0f));
}

void ATPawn::SpawnNewTetronimo()
{
	FVector Location = FVector(0.0f);
	FRotator Rotation = FRotator(0.0f);
	
	// TODO(NINJIN42): Use Deferred Spawning to wait for spawning until InitCustom is finished
	TetronimoCurrent = GetWorld()->SpawnActor<ATTetromino>(Location, Rotation);
	TetronimoCurrent->InitCustom(PaperSprite, Color);

	// The Deferred Spawning functions
	//FTransform SpawnTransform(Rotation, Origin);
	//UGameplayStatics::BeginDeferredActorSpawnFromClass(this, TetronimoCurrent::StaticClass(), SpawnTransform));
	//if(TetronimoCurrent != nullptr)
	//{
	//	MyDeferredActor->Init(ShootDir);
	//	UGameplayStatics::FinishSpawningActor(TetronimoCurrent, SpawnTransform);
	//}
}

