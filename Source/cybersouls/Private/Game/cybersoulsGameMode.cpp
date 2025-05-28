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
	
	// Get player controller first with safety checks
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		UE_LOG(LogTemp, Error, TEXT("CompleteQuest: No valid PlayerController found"));
		return;
	}
	
	// Get player pawn with safety checks
	APawn* PlayerPawn = PC->GetPawn();
	if (!IsValid(PlayerPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("CompleteQuest: No valid PlayerPawn found"));
		return;
	}
	
	// Try to find components - handle cases where they might not exist
	UPlayerAttributeComponent* PlayerAttributes = PlayerPawn->FindComponentByClass<UPlayerAttributeComponent>();
	UPlayerProgressionComponent* PlayerProgression = PlayerPawn->FindComponentByClass<UPlayerProgressionComponent>();
	
	if (IsValid(PlayerAttributes) && IsValid(PlayerProgression))
	{
		// Safe XP conversion with additional validation
		float Integrity = FMath::Clamp(PlayerAttributes->Integrity, 0.0f, 1000.0f);
		float HackProgress = FMath::Clamp(PlayerAttributes->HackProgress, 0.0f, 1000.0f);
		float MaxHackProgress = FMath::Clamp(PlayerAttributes->MaxHackProgress, 1.0f, 1000.0f);
		
		UE_LOG(LogTemp, Warning, TEXT("Converting to XP: Integrity=%f, HackProgress=%f, MaxHackProgress=%f"), 
			Integrity, HackProgress, MaxHackProgress);
		
		PlayerProgression->ConvertStatsToXP(Integrity, HackProgress, MaxHackProgress);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CompleteQuest: PlayerAttributes or PlayerProgression component not found on %s"), 
			*PlayerPawn->GetName());
		
		// Still show XP display even if conversion fails
	}
	
	// Show XP display in HUD with safety checks
	ACybersoulsHUD* CybersoulsHUD = Cast<ACybersoulsHUD>(PC->GetHUD());
	if (IsValid(CybersoulsHUD))
	{
		CybersoulsHUD->ShowXPDisplay();
		UE_LOG(LogTemp, Warning, TEXT("CompleteQuest: Showing XP display"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CompleteQuest: CybersoulsHUD not found or invalid"));
	}
	
	// Broadcast quest completion
	OnQuestCompleted.Broadcast();
}

void AcybersoulsGameMode::RestartLevel(bool bResetXP)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("RestartLevel: World is null"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("RestartLevel called with bResetXP=%s"), bResetXP ? TEXT("true") : TEXT("false"));
	
	// Clean up any widgets before restart
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (IsValid(PC))
	{
		// Force close inventory if open
		if (ACybersoulsHUD* HUD = Cast<ACybersoulsHUD>(PC->GetHUD()))
		{
			HUD->ForceCloseInventory();
		}
		
		// Save progression if player pawn exists
		if (IsValid(PC->GetPawn()))
		{
			UPlayerProgressionComponent* PlayerProgression = PC->GetPawn()->FindComponentByClass<UPlayerProgressionComponent>();
			if (IsValid(PlayerProgression))
			{
				if (bResetXP)
				{
					// Reset XP to 0
					PlayerProgression->ResetProgression();
				}
				
				// Always save progression (either current values or reset values)
				PlayerProgression->SaveProgression();
				
				// Force a small delay to ensure save completes
				FPlatformProcess::Sleep(0.1f);
			}
		}
		
		// Clear input mode
		FInputModeGameOnly GameOnlyMode;
		PC->SetInputMode(GameOnlyMode);
		PC->bShowMouseCursor = false;
	}
	
	// Use UGameplayStatics for level restart
	FString MapName = GetWorld()->GetMapName();
	UE_LOG(LogTemp, Warning, TEXT("Restarting level: %s"), *MapName);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName), false);
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
