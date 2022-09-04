// Copyright OurPixel,2021

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
}

// Find Physics Handle
void UGrabber::SetupInputComponent()
{
	// Set Up Input Component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component is Found!"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::FindPhysicsHandle()
{
	// Checking Physics Handle (Get Owner + Get Class)
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Error no Component Found on %s"), *GetOwner()->GetName());
	}
}

// Input Grabber Set Up
void UGrabber::Grab()
{

	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();
	
	if (ActorHit)
	{ if(!PhysicsHandle) {return;}
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayerReach()
		);
	}
}

void UGrabber::Release()
{
	if(!PhysicsHandle) {return;}
	UE_LOG(LogTemp, Warning, TEXT("Grabber Release"));
	PhysicsHandle->ReleaseComponent();
}

// Called every frame

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(!PhysicsHandle) {return;}
	if (PhysicsHandle->GrabbedComponent)
	{
		
		PhysicsHandle->SetTargetLocation(GetPlayerReach());
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	// Ray Cast
	FHitResult Hit;

	// Ignoring Actor Ray Cast
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner()
		);
	
	// Set Up Ray Casting for Grabbing
	GetWorld()->LineTraceSingleByObjectType(
	OUT Hit,
	GetPlayerWorldPos(),
	GetPlayerReach(),
	FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
	TraceParams
	);
	return Hit;
}

FVector UGrabber::GetPlayerWorldPos() const
{
	// GET PLAYER VIEWPOINT
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation;
}

FVector UGrabber::GetPlayerReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	
}
