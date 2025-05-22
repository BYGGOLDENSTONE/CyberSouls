// QuickHackComponent.cpp
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

UQuickHackComponent::UQuickHackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UQuickHackComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Set ability name based on hack type
	switch (QuickHackType)
	{
		case EQuickHackType::InterruptProtocol:
			AbilityName = "InterruptProtocol";
			CastTime = 5.0f;
			Cooldown = 8.0f;
			break;
		case EQuickHackType::SystemFreeze:
			AbilityName = "SystemFreeze";
			CastTime = 7.0f;
			Cooldown = 14.0f;
			EffectDuration = 3.0f;
			break;
		case EQuickHackType::Firewall:
			AbilityName = "Firewall";
			CastTime = 6.0f;
			Cooldown = 12.0f;
			EffectDuration = 3.0f;
			bIsSelfTargeted = true;
			break;
		case EQuickHackType::Kill:
			AbilityName = "Kill";
			CastTime = 3.0f;
			Cooldown = 10.0f;
			break;
		default:
			break;
	}
}

void UQuickHackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Check if owner is still alive
	if (ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(GetOwner()))
	{
		if (Enemy->IsDead())
		{
			InterruptQuickHack();
			return;
		}
	}
	
	if (bIsAbilityActive && CurrentCastTime < CastTime)
	{
		CurrentCastTime += DeltaTime;
		
		if (CurrentCastTime >= CastTime)
		{
			CompleteQuickHack();
		}
	}
}

void UQuickHackComponent::StartQuickHack(AActor* Target)
{
	if (bIsSelfTargeted)
	{
		CurrentTarget = GetOwner();
	}
	else
	{
		CurrentTarget = Target;
	}
	
	if (CanActivateAbility() && CurrentTarget)
	{
		ActivateAbility();
		CurrentCastTime = 0.0f;
		
		UE_LOG(LogTemp, Warning, TEXT("Starting QuickHack: %s"), *AbilityName);
	}
}

void UQuickHackComponent::InterruptQuickHack()
{
	if (bIsAbilityActive)
	{
		DeactivateAbility();
		CurrentCastTime = 0.0f;
		CurrentTarget = nullptr;
		
		UE_LOG(LogTemp, Warning, TEXT("QuickHack interrupted: %s"), *AbilityName);
	}
}

void UQuickHackComponent::ActivateAbility()
{
	Super::ActivateAbility();
}

bool UQuickHackComponent::CanActivateAbility()
{
	return Super::CanActivateAbility() && QuickHackType != EQuickHackType::None;
}

void UQuickHackComponent::CompleteQuickHack()
{
	ApplyQuickHackEffect();
	DeactivateAbility();
	CurrentCastTime = 0.0f;
	CurrentTarget = nullptr;
}

void UQuickHackComponent::ApplyQuickHackEffect()
{
	if (!CurrentTarget)
	{
		return;
	}
	
	UPlayerAttributeComponent* PlayerAttributes = CurrentTarget->FindComponentByClass<UPlayerAttributeComponent>();
	
	switch (QuickHackType)
	{
		case EQuickHackType::InterruptProtocol:
			// Interrupt all active QuickHacks on target
			{
				TArray<UQuickHackComponent*> QuickHacks;
				CurrentTarget->GetComponents<UQuickHackComponent>(QuickHacks);
				for (UQuickHackComponent* QH : QuickHacks)
				{
					if (QH->IsQuickHackActive())
					{
						QH->InterruptQuickHack();
					}
				}
				UE_LOG(LogTemp, Warning, TEXT("InterruptProtocol: Interrupted target's QuickHacks"));
			}
			break;
			
		case EQuickHackType::SystemFreeze:
			// Immobilize target
			if (PlayerAttributes)
			{
				PlayerAttributes->bIsImmobilized = true;
				
				// Set timer to remove effect
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [PlayerAttributes]()
				{
					PlayerAttributes->bIsImmobilized = false;
					UE_LOG(LogTemp, Warning, TEXT("SystemFreeze: Effect ended"));
				}, EffectDuration, false);
				
				UE_LOG(LogTemp, Warning, TEXT("SystemFreeze: Target immobilized for %f seconds"), EffectDuration);
			}
			break;
			
		case EQuickHackType::Firewall:
			// Apply firewall protection
			if (PlayerAttributes)
			{
				PlayerAttributes->bHasFirewall = true;
				
				// Set timer to remove effect
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [PlayerAttributes]()
				{
					PlayerAttributes->bHasFirewall = false;
					UE_LOG(LogTemp, Warning, TEXT("Firewall: Protection ended"));
				}, EffectDuration, false);
				
				UE_LOG(LogTemp, Warning, TEXT("Firewall: Protection active for %f seconds"), EffectDuration);
			}
			break;
			
		case EQuickHackType::Kill:
			// Instantly kill the target enemy
			{
				ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(CurrentTarget);
				if (Enemy)
				{
					// Trigger death directly
					UEnemyAttributeComponent* EnemyAttributes = Enemy->FindComponentByClass<UEnemyAttributeComponent>();
					if (EnemyAttributes)
					{
						EnemyAttributes->TakeDamage(100.0f); // Deal lethal damage
						UE_LOG(LogTemp, Warning, TEXT("Kill QuickHack: Enemy eliminated"));
					}
				}
			}
			break;
			
		default:
			break;
	}
}