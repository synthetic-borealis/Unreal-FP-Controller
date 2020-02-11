// Copyright 2020, Elhanan Flesch

// TODO make IsBlockedCrouching run only when uncrouching

#include "FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "CollisionShape.h"
#include "Grabber.h"

// Sets default values
AFirstPersonCharacter::AFirstPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a CameraComponent
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	CameraComponent->bUsePawnControlRotation = true;

	// Setup crouching
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	StandingCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	StandingCameraLocation = CameraComponent->GetRelativeLocation();
}

// Called when the game starts or when spawned
void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	FindGrabberComponent();
}

// Called every frame
void AFirstPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCrouch(DeltaTime);
	UpdateWalkingSpeed();
}

void AFirstPersonCharacter::FindGrabberComponent()
{
	GrabberComponent = this->FindComponentByClass<UGrabber>();
	if (!GrabberComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No grabber component found on character %s"), *this->GetName());
	}
}

// Called to bind functionality to input
void AFirstPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Bind actions
	// Jumping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::Jump);

	// Crouching
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFirstPersonCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFirstPersonCharacter::EndCrouch);

	// Running
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AFirstPersonCharacter::StartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AFirstPersonCharacter::StopRunning);

	// Grabbing
	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AFirstPersonCharacter::Grab);
	PlayerInputComponent->BindAction("Grab", IE_Released, this, &AFirstPersonCharacter::Release);

	// Keyboard axes bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &AFirstPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFirstPersonCharacter::MoveRight);

	// Mouse axes bindings
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
}

void AFirstPersonCharacter::Grab()
{
	if (GrabberComponent)
	{
		GrabberComponent->Grab();
	}
}

void AFirstPersonCharacter::Release()
{
	if (GrabberComponent)
	{
		GrabberComponent->Release();
	}
}

void AFirstPersonCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void AFirstPersonCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void AFirstPersonCharacter::UpdateCrouch(float DeltaTime)
{
	if (bWantsToCrouch)
	{
		const FVector NewCameraPosition = FMath::Lerp(CameraComponent->GetRelativeLocation(),
			FVector(0.0f, 0.0f, 30.0f), CrouchTransitionTime * DeltaTime);
		
		const float NewCapsuleHalfHeight = FMath::Lerp(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),
			StandingCapsuleHalfHeight / 2.0f, CrouchTransitionTime * DeltaTime);
		
		CameraComponent->SetRelativeLocation(NewCameraPosition);
		GetCapsuleComponent()->SetCapsuleHalfHeight(NewCapsuleHalfHeight);

		bIsCrouching = true;
	}
	else
	{
		bCanUncrouch = !IsBlockedCrouching();
		if (bCanUncrouch)
		{
			const FVector NewCameraPosition = FMath::Lerp(CameraComponent->GetRelativeLocation(),
				StandingCameraLocation, CrouchTransitionTime * DeltaTime);
			
			const float NewCapsuleHalfHeight = FMath::Lerp(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),
				StandingCapsuleHalfHeight, CrouchTransitionTime * DeltaTime);
			
			CameraComponent->SetRelativeLocation(NewCameraPosition);
			GetCapsuleComponent()->SetCapsuleHalfHeight(NewCapsuleHalfHeight);

			bIsCrouching = false;
		}
	}
	
}

void AFirstPersonCharacter::StartCrouch()
{
	bWantsToCrouch = true;
}

void AFirstPersonCharacter::EndCrouch()
{
	bWantsToCrouch = false;
}

bool AFirstPersonCharacter::IsBlockedCrouching()
{
	FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(GetCapsuleComponent()->GetUnscaledCapsuleRadius());

	FHitResult Hit;
	float RayLength = StandingCapsuleHalfHeight - GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() +
		GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	return GetWorld()->SweepSingleByChannel(Hit, GetActorLocation(), GetActorLocation() + GetActorUpVector() * RayLength,
		FQuat::Identity, ECollisionChannel::ECC_Visibility, CollisionSphere, FCollisionQueryParams("", false, this),
		FCollisionResponseParams()
	);
}

void AFirstPersonCharacter::StartRunning()
{
	bIsRunning = true;
}

void AFirstPersonCharacter::StopRunning()
{
	bIsRunning = false;
}

void AFirstPersonCharacter::UpdateWalkingSpeed()
{
	if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchedWalkingSpeed;
	}
	else if (bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RegularWalkingSpeed;
	}
}
