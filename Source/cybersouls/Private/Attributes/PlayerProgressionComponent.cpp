// PlayerProgressionComponent.cpp

#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "cybersouls/Public/SaveGame/CybersoulsSaveGame.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

UPlayerProgressionComponent::UPlayerProgressionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerProgressionComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadProgression();
}

void UPlayerProgressionComponent::ConvertStatsToXP(float RemainingIntegrity, float RemainingHackProgress, float MaxHackProgress)
{
	// Convert remaining integrity directly to XP
	float IntegrityXPGained = RemainingIntegrity * IntegrityToXPRatio;
	
	// Convert hack resistance (inverse of hack progress) to XP
	float HackResistance = MaxHackProgress - RemainingHackProgress;
	float HackingXPGained = HackResistance * HackResistanceToXPRatio;
	
	AddIntegrityXP(IntegrityXPGained);
	AddHackingXP(HackingXPGained);
	
	UE_LOG(LogTemp, Warning, TEXT("Quest Complete! Converted %f integrity and %f hack resistance to XP"), 
		RemainingIntegrity, HackResistance);
}

void UPlayerProgressionComponent::AddIntegrityXP(float Amount)
{
	if (Amount > 0.0f)
	{
		IntegrityXP += Amount;
		OnIntegrityXPChanged.Broadcast(IntegrityXP);
		UE_LOG(LogTemp, Log, TEXT("Gained %f Integrity XP. Total: %f"), Amount, IntegrityXP);
	}
}

void UPlayerProgressionComponent::AddHackingXP(float Amount)
{
	if (Amount > 0.0f)
	{
		HackingXP += Amount;
		OnHackingXPChanged.Broadcast(HackingXP);
		UE_LOG(LogTemp, Log, TEXT("Gained %f Hacking XP. Total: %f"), Amount, HackingXP);
	}
}

void UPlayerProgressionComponent::SaveProgression()
{
	// Safety check
	if (!IsValid(this))
	{
		UE_LOG(LogTemp, Error, TEXT("[XP SAVE] SaveProgression called on invalid component"));
		return;
	}
	
	// Create or get existing save game object
	UCybersoulsSaveGame* SaveGameInstance = Cast<UCybersoulsSaveGame>(UGameplayStatics::CreateSaveGameObject(UCybersoulsSaveGame::StaticClass()));
	
	if (SaveGameInstance)
	{
		// Store current XP values with validation
		SaveGameInstance->IntegrityXP = FMath::Max(0.0f, IntegrityXP);
		SaveGameInstance->HackingXP = FMath::Max(0.0f, HackingXP);
		
		// Save to disk
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, UCybersoulsSaveGame::SaveSlotName, UCybersoulsSaveGame::UserIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("[XP SAVE] Progression saved successfully - Integrity XP: %f, Hacking XP: %f"), IntegrityXP, HackingXP);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[XP SAVE] Failed to save progression to disk"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[XP SAVE] Failed to create save game instance"));
	}
}

void UPlayerProgressionComponent::LoadProgression()
{
	// Try to load existing save game
	UCybersoulsSaveGame* LoadGameInstance = Cast<UCybersoulsSaveGame>(UGameplayStatics::LoadGameFromSlot(UCybersoulsSaveGame::SaveSlotName, UCybersoulsSaveGame::UserIndex));
	
	if (LoadGameInstance)
	{
		// Restore XP values
		IntegrityXP = LoadGameInstance->IntegrityXP;
		HackingXP = LoadGameInstance->HackingXP;
		
		// Broadcast events to update UI
		OnIntegrityXPChanged.Broadcast(IntegrityXP);
		OnHackingXPChanged.Broadcast(HackingXP);
		
		UE_LOG(LogTemp, Warning, TEXT("[XP LOAD] Progression loaded successfully - Integrity XP: %f, Hacking XP: %f"), IntegrityXP, HackingXP);
		
		// Force UI update
		if (IntegrityXP > 0.0f || HackingXP > 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("[XP LOAD] XP values restored from save!"));
		}
	}
	else
	{
		// No save game exists yet, start with 0 XP
		UE_LOG(LogTemp, Warning, TEXT("[XP LOAD] No save game found, starting with 0 XP"));
	}
}

void UPlayerProgressionComponent::ResetProgression()
{
	IntegrityXP = 0.0f;
	HackingXP = 0.0f;
	OnIntegrityXPChanged.Broadcast(IntegrityXP);
	OnHackingXPChanged.Broadcast(HackingXP);
	UE_LOG(LogTemp, Warning, TEXT("Progression reset to zero"));
}