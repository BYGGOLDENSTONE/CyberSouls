// Copyright Epic Games, Inc. All Rights Reserved.

#include "cybersouls/Public/Game/cybersoulsGameMode.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

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
	
	// Set custom player controller
	PlayerControllerClass = ACyberSoulsPlayerController::StaticClass();
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
	
	// Show XP display in HUD
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		ACybersoulsHUD* CybersoulsHUD = Cast<ACybersoulsHUD>(PC->GetHUD());
		if (CybersoulsHUD)
		{
			CybersoulsHUD->ShowXPDisplay();
		}
	}
	
	OnQuestCompleted.Broadcast();
}

void AcybersoulsGameMode::RestartLevel(bool bResetXP)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("RestartLevel: World is null"));
		return;
	}
	
	// If resetting XP, clear player progression
	if (bResetXP)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (IsValid(PC) && IsValid(PC->GetPawn()))
		{
			UPlayerProgressionComponent* PlayerProgression = PC->GetPawn()->FindComponentByClass<UPlayerProgressionComponent>();
			if (IsValid(PlayerProgression))
			{
				PlayerProgression->ResetProgression();
			}
		}
	}
	
	// Use UGameplayStatics for level restart
	UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetMapName()), false);
}

void AcybersoulsGameMode::OnPlayerDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Player has died!"));
	
	// Show death screen in HUD
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		ACybersoulsHUD* CybersoulsHUD = Cast<ACybersoulsHUD>(PC->GetHUD());
		if (CybersoulsHUD)
		{
			CybersoulsHUD->ShowDeathScreen();
		}
	}
}
