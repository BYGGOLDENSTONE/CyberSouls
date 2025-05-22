// SlashAbilityComponent.cpp
#include "cybersouls/Public/Abilities/SlashAbilityComponent.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
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
	UE_LOG(LogTemp, Warning, TEXT("Slash ability activated!"));
	
	if (CanActivateAbility())
	{
		Super::ActivateAbility();
		PerformSlash();
		
		// Auto deactivate after slash
		DeactivateAbility();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Slash ability cannot be activated!"));
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
	
	UE_LOG(LogTemp, Warning, TEXT("Slash: Found %d targets in range"), Targets.Num());
	
	for (AActor* Target : Targets)
	{
		ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(Target);
		if (!Enemy || Enemy->IsDead())
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
				UE_LOG(LogTemp, Warning, TEXT("Slash dealing %f damage to enemy with %f HP"), SlashDamage, EnemyAttributes->GetIntegrity());
				EnemyAttributes->TakeDamage(SlashDamage);
				UE_LOG(LogTemp, Warning, TEXT("Slash hit enemy for %f damage! Enemy HP now: %f"), SlashDamage, EnemyAttributes->GetIntegrity());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Enemy has no EnemyAttributeComponent!"));
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
	
	// Filter by crosshair target if we have one
	AcybersoulsCharacter* PlayerChar = Cast<AcybersoulsCharacter>(GetOwner());
	if (PlayerChar)
	{
		AActor* CrosshairTarget = PlayerChar->GetCrosshairTarget();
		if (CrosshairTarget && FoundTargets.Contains(CrosshairTarget))
		{
			// If we have a crosshair target, prioritize it
			TArray<AActor*> FilteredTargets;
			FilteredTargets.Add(CrosshairTarget);
			return FilteredTargets;
		}
	}
	
	return FoundTargets;
}

EBodyPart USlashAbilityComponent::GetTargetedBodyPart() const
{
	AcybersoulsCharacter* PlayerChar = Cast<AcybersoulsCharacter>(GetOwner());
	if (PlayerChar)
	{
		return PlayerChar->GetCrosshairBodyPart();
	}
	
	return EBodyPart::UpperBody; // Default
}