// PlayerProgressionComponent.cpp

#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "Engine/Engine.h"

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
	// TODO: Implement save system when needed
	UE_LOG(LogTemp, Log, TEXT("Progression saved - Integrity XP: %f, Hacking XP: %f"), IntegrityXP, HackingXP);
}

void UPlayerProgressionComponent::LoadProgression()
{
	// TODO: Implement load system when needed
	UE_LOG(LogTemp, Log, TEXT("Progression loaded - Integrity XP: %f, Hacking XP: %f"), IntegrityXP, HackingXP);
}