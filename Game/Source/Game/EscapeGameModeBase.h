// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/KismetStringLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "EscapeGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AEscapeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	int32 PlayerCount = 0;

	UFUNCTION(BlueprintCallable, Category = "I fucking give up")
		TArray<AActor*> GetPlayerSpawnsSorted() const;

};
