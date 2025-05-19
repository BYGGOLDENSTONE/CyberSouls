// AttributeComponent.cpp
#include "cybersouls/Public/Attributes/AttributeComponent.h"
#include "Engine/Engine.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::ModifyIntegrity(float Delta)
{
	float OldIntegrity = Integrity;
	Integrity = FMath::Clamp(Integrity + Delta, 0.0f, MaxIntegrity);
	
	if (Integrity != OldIntegrity)
	{
		OnIntegrityChanged.Broadcast(Integrity);
		
		// Check for integrity milestones
		float IntegrityPercentage = (Integrity / MaxIntegrity) * 100.0f;
		if (OldIntegrity > 75.0f && Integrity <= 75.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Integrity at 75%%"));
		}
		else if (OldIntegrity > 50.0f && Integrity <= 50.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Integrity at 50%%"));
		}
		else if (OldIntegrity > 25.0f && Integrity <= 25.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Integrity at 25%%"));
		}
		
		CheckIntegrityStatus();
	}
}

void UAttributeComponent::ModifyHackProgress(float Delta)
{
	float OldHackProgress = HackProgress;
	HackProgress = FMath::Clamp(HackProgress + Delta, 0.0f, MaxHackProgress);
	
	if (HackProgress != OldHackProgress)
	{
		OnHackProgressChanged.Broadcast(HackProgress);
		CheckHackStatus();
	}
}

bool UAttributeComponent::ConsumeBlockCharge()
{
	if (BlockCharge > 0)
	{
		BlockCharge--;
		return true;
	}
	return false;
}

bool UAttributeComponent::ConsumeDodgeCharge()
{
	if (DodgeCharge > 0)
	{
		DodgeCharge--;
		return true;
	}
	return false;
}

void UAttributeComponent::CheckIntegrityStatus()
{
	if (Integrity <= 0.0f && !IsHacked())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player died from integrity loss"));
		OnDeath.Broadcast();
	}
}

void UAttributeComponent::CheckHackStatus()
{
	if (HackProgress >= MaxHackProgress && IsAlive())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player died from hack completion"));
		OnDeath.Broadcast();
	}
}