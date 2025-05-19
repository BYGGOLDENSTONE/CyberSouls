// PlayerAttributeComponent.cpp
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "Engine/Engine.h"

UPlayerAttributeComponent::UPlayerAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerAttributeComponent::TakeDamage(float Damage)
{
	float OldIntegrity = Integrity;
	Integrity = FMath::Clamp(Integrity - Damage, 0.0f, MaxIntegrity);
	
	if (OldIntegrity != Integrity)
	{
		OnIntegrityChanged.Broadcast(Integrity);
		UE_LOG(LogTemp, Warning, TEXT("Player took %f damage. Integrity: %f/%f"), 
			Damage, Integrity, MaxIntegrity);
	}
	
	// Player doesn't die from integrity loss
}

void UPlayerAttributeComponent::IncreaseHackProgress(float Amount)
{
	// Firewall blocks hack progress
	if (bHasFirewall)
	{
		UE_LOG(LogTemp, Warning, TEXT("Firewall blocked hack attempt!"));
		return;
	}
	
	float OldHackProgress = HackProgress;
	HackProgress = FMath::Clamp(HackProgress + Amount, 0.0f, MaxHackProgress);
	
	if (OldHackProgress != HackProgress)
	{
		OnHackProgressChanged.Broadcast(HackProgress);
		UE_LOG(LogTemp, Warning, TEXT("Player hack progress increased by %f. Progress: %f/%f"), 
			Amount, HackProgress, MaxHackProgress);
	}
	
	CheckDeath();
}

void UPlayerAttributeComponent::RestoreIntegrity(float Amount)
{
	float OldIntegrity = Integrity;
	Integrity = FMath::Clamp(Integrity + Amount, 0.0f, MaxIntegrity);
	
	if (OldIntegrity != Integrity)
	{
		OnIntegrityChanged.Broadcast(Integrity);
		UE_LOG(LogTemp, Warning, TEXT("Player restored %f integrity. Integrity: %f/%f"), 
			Amount, Integrity, MaxIntegrity);
	}
}

bool UPlayerAttributeComponent::IsAlive() const
{
	return HackProgress < MaxHackProgress;
}

void UPlayerAttributeComponent::CheckDeath()
{
	if (HackProgress >= MaxHackProgress)
	{
		UE_LOG(LogTemp, Error, TEXT("Player has been fully hacked! Game Over!"));
		OnDeath.Broadcast();
	}
}