// Fill out your copyright notice in the Description page of Project Settings.

#include "GameJamCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGameJamCharacter::AGameJamCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->AirControl = 1.0f;
	bUseControllerRotationPitch = true;

	//objects
	FlashLightMesh = CreateDefaultSubobject<UChildActorComponent>(TEXT("FlashLight"));
	FlashLightMesh->CreateChildActor();
	FlashLightMesh->SetupAttachment(GetMesh());
	FlashLightMesh->SetMobility(EComponentMobility::Movable);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh());
	Camera->bUsePawnControlRotation = true;

	Light = CreateDefaultSubobject<USpotLightComponent >(TEXT("Light"));
	Light->SetupAttachment(Camera);
	Light->SetMobility(EComponentMobility::Movable);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("EyeLevel Arrow"));
	Arrow->SetupAttachment(Camera);

	//handles
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandle->InterpolationSpeed = 2.f;
}

// Called when the game starts or when spawned
void AGameJamCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(GetControlRotation());

}

// Called every frame
void AGameJamCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult Hit = StartDefaultLineTrace(ECC_Visibility);

	DrawDebugSphere
	(
		GetWorld(),
		Hit.Location,
		2.f,
		1,
		FColor::Red,
		false,
		0.1,
		1,
		1.f
	);

	if (HasAuthority())
	{
		DragProgress(Hit);
	}

	/*if ((!IsLocallyControlled()) && GetWorld()->IsServer()) 
	{
		GetMesh()->SetWorldRotation(GetControlRotation());
	}*/
	
}

void AGameJamCharacter::DragProgress_Implementation(FHitResult Hit)
{
	if (PhysicsHandle)
	{
		if (FVector::Dist(Hit.Location, GetActorLocation()) <= ReachRange)
		{
			PhysicsHandle->SetTargetLocation(Hit.Location);
		}
		else
		{
			PhysicsHandle->SetTargetLocation(GetActorLocation());
		}
	}
}

FHitResult AGameJamCharacter::StartDefaultLineTrace(ECollisionChannel Channel)
{

	FHitResult OutHit;
	FVector Start = Camera->GetComponentLocation();
	FVector End = (Camera->GetForwardVector() * ReachRange) + Start;
	FCollisionQueryParams CollisionParams;
	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
		CollisionParams.AddIgnoredComponent(PhysicsHandle->GrabbedComponent);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, Channel, CollisionParams);

	return OutHit;
}

// Called to bind functionality to input
void AGameJamCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!PlayerInputComponent)
	{
		return;
	}

	PlayerInputComponent->BindAxis
	(
		TEXT("MoveForwards"),
		this,
		&AGameJamCharacter::MoveForwards
	);

	PlayerInputComponent->BindAxis
	(
		TEXT("MoveSideways"),
		this,
		&AGameJamCharacter::MoveSideToSide
	);

	PlayerInputComponent->BindAxis
	(
		TEXT("LookLeftRight"),
		this,
		&AGameJamCharacter::LookLeftRight
	);

	PlayerInputComponent->BindAxis
	(
		TEXT("LookUpDown"),
		this,
		&AGameJamCharacter::LookUpDown
	);

	PlayerInputComponent->BindAction
	(
		"Jump",
		IE_Pressed,
		this,
		&AGameJamCharacter::MoveJump
	);

	PlayerInputComponent->BindAction
	(
		"Drag",
		IE_Pressed,
		this,
		&AGameJamCharacter::DragAndInteract
	);

	PlayerInputComponent->BindAction
	(
		"Drag",
		IE_Released,
		this,
		&AGameJamCharacter::DragEnd
	);

}

void AGameJamCharacter::MoveForwards(float AxisValue)
{
	AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), AxisValue, false);
}

void AGameJamCharacter::MoveSideToSide(float AxisValue)
{
	AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), AxisValue, false);
}


void AGameJamCharacter::LookLeftRight(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void AGameJamCharacter::LookUpDown(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void AGameJamCharacter::MoveJump()
{
	if (!CanJump())
	{
		return;
	}
	Jump();
}

void 	AGameJamCharacter::InteractStart_Implementation()
{
	FHitResult OutHit = StartDefaultLineTrace(ECC_Visibility);

	AActor* Interactable = OutHit.GetActor();

	if (!Interactable)
	{
		return;
	}

	OnHitDraggableActor(OutHit);
}

void AGameJamCharacter::DragStart_Implementation()
{
	FHitResult OutHit = StartDefaultLineTrace(ECC_GameTraceChannel1);

	UPrimitiveComponent* ToBeGrabbed = OutHit.GetComponent();

	if (!ToBeGrabbed)
	{
		return;
	}

	ToBeGrabbed->SetSimulatePhysics(true);

	PickUpObjectCollision = ToBeGrabbed->GetCollisionResponseToChannel(ECC_Pawn);

	ToBeGrabbed->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	PhysicsHandle->GrabComponentAtLocation(ToBeGrabbed, TEXT(""), OutHit.Location);

}

void AGameJamCharacter::DragAndInteract_Implementation()
{
	DragStart();
	InteractStart();
}

void AGameJamCharacter::DragEnd_Implementation()
{

	if (!PhysicsHandle)
	{
		return;
	}

	UPrimitiveComponent* Component = PhysicsHandle->GetGrabbedComponent();

	if (!Component)
	{
		return;
	}

	Component->SetCollisionResponseToChannel(ECC_Pawn, PickUpObjectCollision);

	PhysicsHandle->ReleaseComponent();
}