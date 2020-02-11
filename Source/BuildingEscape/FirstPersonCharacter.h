// Copyright 2020, Elhanan Flesch

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FirstPersonCharacter.generated.h"

UCLASS()
class BUILDINGESCAPE_API AFirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;
public:
	// Sets default values for this character's properties
	AFirstPersonCharacter();

protected:
	// Walking speeds
	UPROPERTY(EditAnywhere)
	float RegularWalkingSpeed = 600.0f;

	UPROPERTY(EditAnywhere)
	float RunningSpeed = 1200.0f;

	UPROPERTY(EditAnywhere)
	float CrouchedWalkingSpeed = 300.0f;

	// Running
	bool bIsRunning = false;

	// Crouching
	UPROPERTY(EditAnywhere)
	float CrouchTransitionTime = 4.0f;
	bool bWantsToCrouch = false;
	bool bIsCrouching = false;
	bool bCanUncrouch;
	FVector StandingCameraLocation;
	float StandingCapsuleHalfHeight;

	bool IsBlockedCrouching();
	void UpdateCrouch(float DeltaTime);
	void StartCrouch();
	void EndCrouch();

	// Running
	void StartRunning();
	void StopRunning();

	// Grabbing
	UPROPERTY()
	class UGrabber* GrabberComponent = nullptr;
	void FindGrabberComponent();
	void Grab();
	void Release();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveRight(float Val);
	void MoveForward(float Val);
	void UpdateWalkingSpeed();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
