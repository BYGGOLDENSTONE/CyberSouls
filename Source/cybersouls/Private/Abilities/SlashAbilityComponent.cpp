// SlashAbilityComponent.cpp
#include "cybersouls/Public/Abilities/SlashAbilityComponent.h"
#include "cybersouls/Public/Combat/TargetLockComponent.h"
#include "cybersouls/Public/Abilities/BlockAbilityComponent.h"
#include "cybersouls/Public/Abilities/DodgeAbilityComponent.h"
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"

USlashAbilityComponent::USlashAbilityComponent()
{
	AbilityName = "Slash";
	Cooldown = 0.5f;
}

void USlashAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USlashAbilityComponent::ActivateAbility()
{
	if (CanActivateAbility())
	{
		Super::ActivateAbility();
		PerformSlash();
		
		// Auto deactivate after slash
		DeactivateAbility();
	}
}

bool USlashAbilityComponent::CanActivateAbility()
{
	return Super::CanActivateAbility();
}

void USlashAbilityComponent::PerformSlash()
{
	TArray<AActor*> Targets = GetTargetsInRange();
	EBodyPart TargetedPart = GetTargetedBodyPart();
	
	for (AActor* Target : Targets)
	{
		ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(Target);
		if (!Enemy)
		{
			continue;
		}
		
		bool bWasBlocked = false;
		bool bWasDodged = false;
		
		// Check for block ability
		UBlockAbilityComponent* BlockComp = Enemy->FindComponentByClass<UBlockAbilityComponent>();
		if (BlockComp && BlockComp->TryBlock(TargetedPart))
		{
			bWasBlocked = true;
			UE_LOG(LogTemp, Warning, TEXT("Enemy blocked slash!"));
		}
		
		// Check for dodge ability if not blocked
		if (!bWasBlocked)
		{
			UDodgeAbilityComponent* DodgeComp = Enemy->FindComponentByClass<UDodgeAbilityComponent>();
			if (DodgeComp && DodgeComp->TryDodge(TargetedPart, GetOwner()))
			{
				bWasDodged = true;
				UE_LOG(LogTemp, Warning, TEXT("Enemy dodged slash!"));
			}
		}
		
		// Deal damage if not blocked or dodged
		if (!bWasBlocked && !bWasDodged)
		{
			UEnemyAttributeComponent* EnemyAttributes = Enemy->FindComponentByClass<UEnemyAttributeComponent>();
			if (EnemyAttributes)
			{
				EnemyAttributes->TakeDamage(SlashDamage);
				UE_LOG(LogTemp, Warning, TEXT("Slash hit enemy for %f damage!"), SlashDamage);
			}
		}
	}
}

TArray<AActor*> USlashAbilityComponent::GetTargetsInRange() const
{
	TArray<AActor*> FoundTargets;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	
	// Get all enemies in range
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		SlashRange,
		ObjectTypes,
		ACybersoulsEnemyBase::StaticClass(),
		IgnoreActors,
		FoundTargets
	);
	
	// Filter by angle if we have a target lock
	UTargetLockComponent* TargetLock = GetOwner()->FindComponentByClass<UTargetLockComponent>();
	if (TargetLock && TargetLock->IsLocked())
	{
		AActor* LockedTarget = TargetLock->GetCurrentTarget();
		if (LockedTarget && FoundTargets.Contains(LockedTarget))
		{
			// If we have a locked target, prioritize it
			TArray<AActor*> FilteredTargets;
			FilteredTargets.Add(LockedTarget);
			return FilteredTargets;
		}
	}
	
	return FoundTargets;
}

EBodyPart USlashAbilityComponent::GetTargetedBodyPart() const
{
	UTargetLockComponent* TargetLock = GetOwner()->FindComponentByClass<UTargetLockComponent>();
	if (TargetLock)
	{
		return TargetLock->GetTargetedBodyPart();
	}
	
	return EBodyPart::UpperBody; // Default
}