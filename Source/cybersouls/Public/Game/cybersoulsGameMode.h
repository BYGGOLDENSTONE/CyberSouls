// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "cybersoulsGameMode.generated.h"

class ACybersoulsEnemyBase;
class UPlayerProgressionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestCompleted);

UCLASS(minimalapi)
class AcybersoulsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AcybersoulsGameMode();

	// Quest management
	UPROPERTY(BlueprintAssignable)
	FOnQuestCompleted OnQuestCompleted;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void RegisterEnemy(ACybersoulsEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void OnEnemyDeath(ACybersoulsEnemyBase* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AreAllEnemiesDead() const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void CompleteQuest();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	int32 GetAliveEnemyCount() const { return AliveEnemies.Num(); }

protected:
	virtual void BeginPlay() override;

private:
	// Enemy tracking for quest completion
	UPROPERTY()
	TArray<ACybersoulsEnemyBase*> AliveEnemies;

	void FindAndRegisterAllEnemies();
};



