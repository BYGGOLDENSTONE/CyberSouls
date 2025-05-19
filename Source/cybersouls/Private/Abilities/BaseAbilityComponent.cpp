// BaseAbilityComponent.cpp
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"

UBaseAbilityComponent::UBaseAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBaseAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBaseAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Update cooldown
	if (CurrentCooldown > 0.0f)
	{
		CurrentCooldown -= DeltaTime;
		if (CurrentCooldown < 0.0f)
		{
			CurrentCooldown = 0.0f;
		}
	}
}

bool UBaseAbilityComponent::CanActivateAbility()
{
	return CurrentCooldown <= 0.0f && !bIsAbilityActive;
}

void UBaseAbilityComponent::ActivateAbility()
{
	if (CanActivateAbility())
	{
		bIsAbilityActive = true;
		CurrentCooldown = Cooldown;
	}
}

void UBaseAbilityComponent::DeactivateAbility()
{
	bIsAbilityActive = false;
}