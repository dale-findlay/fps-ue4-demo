// Fill out your copyright notice in the Description page of Project Settings.


#include "ShiftPlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"

AShiftPlayerCharacter::AShiftPlayerCharacter()
{
	//Setup the rate of change multipliers for analog joysticks.
	TurnRateMultiplier = 45.f;
	LookUpRateMultiplier = 45.f;

	//Setup the first person camera.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	Camera->bUsePawnControlRotation = true;

	//Setup the arms mesh.
	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	ArmsMesh->SetOnlyOwnerSee(true);
	ArmsMesh->SetupAttachment(Camera);
	ArmsMesh->bCastDynamicShadow = false;
	ArmsMesh->CastShadow = false;
	ArmsMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	ArmsMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
 	
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	//Setup gun.
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	GunMesh->SetupAttachment(RootComponent);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

void AShiftPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GunMesh->AttachToComponent(ArmsMesh, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), TEXT("GripPoint"));
}

void AShiftPlayerCharacter::HandleCrouch()
{
	Crouch();
}

void AShiftPlayerCharacter::HandleStopCrouching()
{
	UnCrouch();
}

void AShiftPlayerCharacter::HandleToggleCrouch()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AShiftPlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AShiftPlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AShiftPlayerCharacter::Controller_TurnAtRate(float Rate)
{
	float CurrentRate = Rate * TurnRateMultiplier;
	AddControllerYawInput(CurrentRate * GetWorld()->GetDeltaSeconds());
}

void AShiftPlayerCharacter::Controller_LookUpAtRate(float Rate)
{
	float CurrentRate = Rate * LookUpRateMultiplier;
	AddControllerPitchInput(CurrentRate * GetWorld()->GetDeltaSeconds());
}

void AShiftPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump & crouch events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShiftPlayerCharacter::HandleCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShiftPlayerCharacter::HandleStopCrouching);
	PlayerInputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &AShiftPlayerCharacter::HandleToggleCrouch);

	//Bind all the main axes and two additional axes for controller support with analog joysticks.	
	PlayerInputComponent->BindAxis("MoveForward", this, &AShiftPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShiftPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShiftPlayerCharacter::Controller_TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShiftPlayerCharacter::Controller_LookUpAtRate);
}

void AShiftPlayerCharacter::EnableMovement()
{
	GetCharacterMovement()->Activate();
}

void AShiftPlayerCharacter::DisableMovement()
{
	GetCharacterMovement()->Deactivate();
}
