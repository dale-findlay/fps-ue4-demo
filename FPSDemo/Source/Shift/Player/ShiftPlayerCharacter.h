// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShiftPlayerCharacter.generated.h"

UCLASS()
class SHIFT_API AShiftPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* ArmsMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

public:
	AShiftPlayerCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Enable character movement. **/
	void EnableMovement();

	/** Disable character movement. **/
	void DisableMovement();

protected:
	virtual void BeginPlay() override;

	/** Action bindings **/
	void HandleCrouch();
	void HandleStopCrouching();
	void HandleToggleCrouch();

	/** Axis bindings. **/
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Controller_TurnAtRate(float Rate);
	void Controller_LookUpAtRate(float Rate);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float TurnRateMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float LookUpRateMultiplier;
};
