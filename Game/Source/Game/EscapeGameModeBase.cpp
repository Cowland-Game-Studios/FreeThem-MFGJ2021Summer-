// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeGameModeBase.h"

TArray<AActor*> AEscapeGameModeBase::GetPlayerSpawnsSorted() const
{
	TArray<AActor*> UncastedPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), UncastedPlayers);

	UncastedPlayers.Sort
	(
		[&](const AActor& A, const AActor& B)
		{
			return A.GetName().Compare(B.GetName()) < 0;
		}
	);

	return UncastedPlayers;
}

AActor* AEscapeGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> Spawns = GetPlayerSpawnsSorted();

	AActor* ToReturn = Spawns[(PlayerCount++) % (Spawns.Num() - 1)];

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Name: %s, Num: %i, len: %i"), *ToReturn->GetName(), PlayerCount, Spawns.Num()));

	return ToReturn;
}
