// Copyright Epic Games, Inc. All Rights Reserved.

#include "cybersouls/Public/Game/cybersoulsGameMode.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"

AcybersoulsGameMode::AcybersoulsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set default HUD class
	HUDClass = ACybersoulsHUD::StaticClass();
}

void AcybersoulsGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Find and register all enemies in the level
	FindAndRegisterAllEnemies();
	
	UE_LOG(LogTemp, Warning, TEXT("GameMode initialized with %d enemies"), AliveEnemies.Num());
}

void AcybersoulsGameMode::FindAndRegisterAllEnemies()
{
	AliveEnemies.Empty();
	
	for (TActorIterator<ACybersoulsEnemyBase> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		ACybersoulsEnemyBase* Enemy = *ActorIterator;
		if (IsValid(Enemy))
		{
			RegisterEnemy(Enemy);
		}
	}
}

void AcybersoulsGameMode::RegisterEnemy(ACybersoulsEnemyBase* Enemy)
{
	if (IsValid(Enemy) && !AliveEnemies.Contains(Enemy))
	{
		AliveEnemies.Add(Enemy);
		UE_LOG(LogTemp, Log, TEXT("Registered enemy: %s"), *Enemy->GetName());
	}
}

void AcybersoulsGameMode::OnEnemyDeath(ACybersoulsEnemyBase* Enemy)
{
	if (IsValid(Enemy))
	{
		AliveEnemies.Remove(Enemy);
		UE_LOG(LogTemp, Warning, TEXT("Enemy died: %s. Remaining: %d"), *Enemy->GetName(), AliveEnemies.Num());
		
		if (AreAllEnemiesDead())
		{
			CompleteQuest();
		}
	}
}

bool AcybersoulsGameMode::AreAllEnemiesDead() const
{
	return AliveEnemies.Num() == 0;
}

void AcybersoulsGameMode::CompleteQuest()
{
	UE_LOG(LogTemp, Warning, TEXT("QUEST COMPLETED! All enemies defeated!"));
	
	// Get player and convert remaining stats to XP
	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (IsValid(PlayerPawn))
	{
		UPlayerAttributeComponent* PlayerAttributes = PlayerPawn->FindComponentByClass<UPlayerAttributeComponent>();
		UPlayerProgressionComponent* PlayerProgression = PlayerPawn->FindComponentByClass<UPlayerProgressionComponent>();
		
		if (IsValid(PlayerAttributes) && IsValid(PlayerProgression))
		{
			PlayerProgression->ConvertStatsToXP(
				PlayerAttributes->Integrity,
				PlayerAttributes->HackProgress,
				PlayerAttributes->MaxHackProgress
			);
		}
	}
	
	OnQuestCompleted.Broadcast();
}
