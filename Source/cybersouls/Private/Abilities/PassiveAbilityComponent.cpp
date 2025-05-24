// PassiveAbilityComponent.cpp
#include "cybersouls/Public/Abilities/PassiveAbilityComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

UPassiveAbilityComponent::UPassiveAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPassiveAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Set ability name based on passive type
	switch (PassiveType)
	{
		case EPassiveAbilityType::ExecutionChains:
			AbilityName = "ExecutionChains";
			break;
		case EPassiveAbilityType::SystemOvercharge:
			AbilityName = "SystemOvercharge";
			bSystemOverchargeActive = true; // Always active when equipped
			break;
		default:
			break;
	}
}

void UPassiveAbilityComponent::OnEnemyKilled()
{
	if (PassiveType == EPassiveAbilityType::ExecutionChains)
	{
		ActivateExecutionChains();
		UE_LOG(LogTemp, Warning, TEXT("Execution Chains: Activated for %f seconds"), ExecutionChainsWindow);
	}
}

bool UPassiveAbilityComponent::ShouldIgnoreBlock() const
{
	// Execution Chains ignores block when active
	if (PassiveType == EPassiveAbilityType::ExecutionChains && bExecutionChainsActive)
	{
		return true;
	}
	
	// System Overcharge ignores all defenses
	if (PassiveType == EPassiveAbilityType::SystemOvercharge && bSystemOverchargeActive)
	{
		return true;
	}
	
	return false;
}

bool UPassiveAbilityComponent::ShouldIgnoreAllDefenses() const
{
	// System Overcharge ignores all defenses (block, dodge, etc.)
	return (PassiveType == EPassiveAbilityType::SystemOvercharge && bSystemOverchargeActive);
}

bool UPassiveAbilityComponent::CanUseQuickHacks() const
{
	// System Overcharge prevents QuickHack usage
	if (PassiveType == EPassiveAbilityType::SystemOvercharge && bSystemOverchargeActive)
	{
		return false;
	}
	
	return true;
}

void UPassiveAbilityComponent::ActivateExecutionChains()
{
	bExecutionChainsActive = true;
	
	// Set timer to deactivate after window expires
	GetWorld()->GetTimerManager().SetTimer(ExecutionChainsTimerHandle, this, &UPassiveAbilityComponent::DeactivateExecutionChains, ExecutionChainsWindow, false);
}

void UPassiveAbilityComponent::DeactivateExecutionChains()
{
	bExecutionChainsActive = false;
	UE_LOG(LogTemp, Warning, TEXT("Execution Chains: Deactivated"));
}