// Copyright 2020, Elhanan Flesch


#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateGrabbedItemPosition();
}

void UGrabber::Grab()
{
	if (!PhysicsHandle) return;

	FHitResult HitResult;
	HitResult = GetFirstPhysicsBodyInReach();

	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* GrabbedActor = HitResult.GetActor();

	if (GrabbedActor)
	{
		PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, GetPlayerReach());
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle) return;

	PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find physics handle for actor %s"), *GetOwner()->GetName());
	}
	
}

FVector UGrabber::GetPlayerWorldPosition() const
{
	FVector PlayerWorldPosition;
	FRotator PlayerRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerWorldPosition, PlayerRotation);

	return PlayerWorldPosition;
}

FVector UGrabber::GetPlayerReach() const
{
	FVector PlayerWorldPosition;
	FRotator PlayerRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerWorldPosition, PlayerRotation);

	return PlayerWorldPosition + PlayerRotation.Vector() * GrabbingRange;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	// ...
	FHitResult HitResult;
	FCollisionQueryParams TraceParams("", false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		GetPlayerWorldPosition(),
		GetPlayerReach(),
		FCollisionObjectQueryParams(ECC_PhysicsBody),
		TraceParams
	);

	return HitResult;
}

void UGrabber::UpdateGrabbedItemPosition()
{
	if (!PhysicsHandle) return;

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
}
