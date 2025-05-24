// HackAbilityComponent.cpp
#include "cybersouls/Public/Abilities/HackAbilityComponent.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/HackingEnemyAttributeComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UHackAbilityComponent::UHackAbilityComponent()
{
	AbilityName = "Hack";
	Cooldown = 0.0f; // Continuous ability
	PrimaryComponentTick.bCanEverTick = true;
}

void UHackAbilityComponent::ActivateAbility()
{
	if (CanActivateAbility())
	{
		Super::ActivateAbility();
		// Hack is continuous, stays active
	}
}

bool UHackAbilityComponent::CanActivateAbility()
{
	if (!Super::CanActivateAbility())
	{
		return false;
	}
	
	// Check if we have a valid target in range
	AActor* Target = GetTarget();
	return Target != nullptr;
}

void UHackAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bIsAbilityActive)
	{
		PerformContinuousHack(DeltaTime);
	}
}

void UHackAbilityComponent::PerformContinuousHack(float DeltaTime)
{
	// Check if owner is still alive
	if (ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(GetOwner()))
	{
		if (Enemy->IsDead())
		{
			DeactivateAbility();
			return;
		}
	}
	
	AActor* Target = GetTarget();
	if (!Target)
	{
		DeactivateAbility();
		return;
	}
	
	// Increase hack progress on player
	UPlayerAttributeComponent* PlayerAttributes = Target->FindComponentByClass<UPlayerAttributeComponent>();
	if (PlayerAttributes)
	{
		float HackAmount = GetHackRate() * DeltaTime;
		PlayerAttributes->IncreaseHackProgress(HackAmount);
		
		UE_LOG(LogTemp, Verbose, TEXT("%s hacking player - Progress increased by %f"), 
			*GetOwner()->GetName(), HackAmount);
	}
}

AActor* UHackAbilityComponent::GetTarget() const
{
	// Find player character
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter)
	{
		return nullptr;
	}
	
	// Check range
	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (Distance <= GetHackRange())
	{
		return PlayerCharacter;
	}
	
	return nullptr;
}

float UHackAbilityComponent::GetHackRate() const
{
	UHackingEnemyAttributeComponent* EnemyAttributes = GetEnemyAttributes();
	if (EnemyAttributes)
	{
		return EnemyAttributes->HackRate;
	}
	// Fallback to basic netrunner rate
	return 2.0f;
}

float UHackAbilityComponent::GetHackRange() const
{
	UHackingEnemyAttributeComponent* EnemyAttributes = GetEnemyAttributes();
	if (EnemyAttributes)
	{
		return EnemyAttributes->HackRange;
	}
	// Fallback to CLAUDE.md spec: 1500 hack range
	return 1500.0f;
}

UHackingEnemyAttributeComponent* UHackAbilityComponent::GetEnemyAttributes() const
{
	if (GetOwner())
	{
		return GetOwner()->FindComponentByClass<UHackingEnemyAttributeComponent>();
	}
	return nullptr;
}