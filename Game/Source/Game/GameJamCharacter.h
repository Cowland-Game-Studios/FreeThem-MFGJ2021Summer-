// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SpotLightComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
//last
#include "GameJamCharacter.generated.h"

class UCameraComponent;
class USpotLightComponent;
class CharacterMovementComponent;
class UPhysicsHandleComponent;
UCLASS()
class GAME_API AGameJamCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGameJamCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* FlashLightMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpotLightComponent* Light = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UArrowComponent* Arrow = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* Camera = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* LineTraceHitActor;

private:

	//movements
	void MoveForwards(float AxisValue);
	void MoveSideToSide(float AxisValue);
	void LookLeftRight(float AxisValue);
	void LookUpDown(float AxisValue);
	void MoveJump();
	FHitResult StartDefaultLineTrace(ECollisionChannel Channel);

	UFUNCTION(Server, Reliable)
		void DragStart();
	UFUNCTION(Server, Reliable)
		void DragEnd();
	UFUNCTION(Server, Reliable)
		void DragProgress(FHitResult Hit);

	//components
	UPROPERTY(EditAnywhere)
		float ReachRange = 250;

	ECollisionResponse PickUpObjectCollision;

};