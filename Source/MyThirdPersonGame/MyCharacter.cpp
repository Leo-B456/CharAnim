// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"


#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->GetCharacterMovement()->JumpZVelocity = 600.0f;
	this->GetCharacterMovement()->AirControl = 0.2f;
	this->JumpMaxHoldTime = 0.8f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Do not rotate when the controller rotates. Controller rotates just the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Rotate the camera boom based on the controller
	CameraBoom->bUsePawnControlRotation = true;

	// Character moves in the direction of the camera
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MyContext, 0);
		}
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
	}
}

void AMyCharacter::Jump()
{
	GEngine->AddOnScreenDebugMessage(-1, 12, FColor::White, "Jump");
	Super::Jump();
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); //Yaw ist Rotation um Achse nach oben (in UE ist es die Z-Achse)

	const FRotationMatrix RotationMatrix(YawRotation);
	const FVector ForwardDirection = RotationMatrix.GetUnitAxis(EAxis::X); //X im Weltkoordinatensystem ist vorwärts
	const FVector RightDirection = RotationMatrix.GetUnitAxis(EAxis::Y);

	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X); //X im Bewegungsvektor ist seitwärts

	if (MovementVector.X < -0.1f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 12, FColor::White, "Left");
	}
	else if (MovementVector.X > 0.1f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 12, FColor::White, "Right");
	}
	if (MovementVector.Y < -0.1f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 12, FColor::White, "Backward");
	}
	else if (MovementVector.Y > 0.1f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 12, FColor::White, "Forward");
	}
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxisVector.Y); //Pitch ist Rotation um Achse nach rechts (also Schauen von oben nach unten)
	AddControllerYawInput(LookAxisVector.X);
}