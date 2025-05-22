// AttackAbilityComponent.cpp
#include "cybersouls/Public/Abilities/AttackAbilityComponent.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UAttackAbilityComponent::UAttackAbilityComponent()
{
	AbilityName = "Attack";
	// Default values are set in header
}

void UAttackAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Set the ability cooldown
	Cooldown = AttackCooldown;
}

void UAttackAbilityComponent::ActivateAbility()
{
	if (CanActivateAbility())
	{
		Super::ActivateAbility();
		PerformAttack();
		
		// Auto deactivate after attack
		DeactivateAbility();
	}
}

bool UAttackAbilityComponent::CanActivateAbility()
{
	if (!Super::CanActivateAbility())
	{
		return false;
	}
	
	// Check if we have a valid target in range
	AActor* Target = GetTarget();
	return Target != nullptr;
}

void UAttackAbilityComponent::PerformAttack()
{
	AActor* Target = GetTarget();
	if (!Target)
	{
		return;
	}
	
	// Deal damage to player's integrity
	UPlayerAttributeComponent* PlayerAttributes = Target->FindComponentByClass<UPlayerAttributeComponent>();
	if (PlayerAttributes)
	{
		PlayerAttributes->TakeDamage(AttackDamage);
		
		UE_LOG(LogTemp, Warning, TEXT("%s attacked player for %f damage"), 
			*GetOwner()->GetName(), AttackDamage);
	}
}

AActor* UAttackAbilityComponent::GetTarget() const
{
	// Find player character
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter)
	{
		return nullptr;
	}
	
	// Check range
	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), PlayerCharacter->GetActorLocation());
	if (Distance <= AttackRange)
	{
		return Cast<AActor>(PlayerCharacter);
	}
	
	return nullptr;
}