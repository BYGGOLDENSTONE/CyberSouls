// BaseAbilityComponent.cpp
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"

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
	// Don't allow ability activation if owner is invalid or dead
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return false;
	}
	
	// Check if owner has a dead state
	if (const ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(Owner))
	{
		if (Enemy->IsDead())
		{
			return false;
		}
	}
	
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