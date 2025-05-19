// BlockAbilityComponent.cpp
#include "cybersouls/Public/Abilities/BlockAbilityComponent.h"
#include "Engine/Engine.h"

UBlockAbilityComponent::UBlockAbilityComponent()
{
	AbilityName = "Block";
	// Block doesn't have a traditional cooldown, it uses charges
	Cooldown = 0.0f;
}

void UBlockAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentBlockCharges = MaxBlockCharges;
}

bool UBlockAbilityComponent::TryBlock(EBodyPart AttackedBodyPart)
{
	if (CanBlock(AttackedBodyPart) && CurrentBlockCharges > 0)
	{
		ConsumeBlockCharge();
		UE_LOG(LogTemp, Warning, TEXT("Blocked attack! Charges remaining: %d"), CurrentBlockCharges);
		return true;
	}
	
	return false;
}

bool UBlockAbilityComponent::CanBlock(EBodyPart BodyPart) const
{
	return BlockableBodyParts.Contains(BodyPart);
}

void UBlockAbilityComponent::ConsumeBlockCharge()
{
	if (CurrentBlockCharges > 0)
	{
		CurrentBlockCharges--;
	}
}