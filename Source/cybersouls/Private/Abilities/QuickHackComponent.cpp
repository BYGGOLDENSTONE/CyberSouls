// QuickHackComponent.cpp
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Abilities/BlockAbilityComponent.h"
#include "cybersouls/Public/Abilities/DodgeAbilityComponent.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

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
			CastTime = 2.0f;
			Cooldown = 8.0f;
			break;
		case EQuickHackType::SystemFreeze:
			AbilityName = "SystemFreeze";
			CastTime = 2.0f;
			Cooldown = 14.0f;
			EffectDuration = 3.0f;
			break;
		case EQuickHackType::Firewall:
			AbilityName = "Firewall";
			CastTime = 3.0f;
			Cooldown = 12.0f;
			EffectDuration = 3.0f;
			bIsSelfTargeted = true;
			break;
		case EQuickHackType::Kill:
			AbilityName = "Kill";
			CastTime = 4.0f;
			Cooldown = 10.0f;
			break;
		case EQuickHackType::CascadeVirus:
			AbilityName = "CascadeVirus";
			CastTime = 2.0f;
			Cooldown = 15.0f;
			EffectDuration = 5.0f;
			break;
		case EQuickHackType::GhostProtocol:
			AbilityName = "GhostProtocol";
			CastTime = 2.0f;
			Cooldown = 20.0f;
			EffectDuration = 5.0f;
			bIsSelfTargeted = true;
			break;
		case EQuickHackType::ChargeDrain:
			AbilityName = "ChargeDrain";
			CastTime = 2.0f;
			Cooldown = 12.0f;
			break;
		case EQuickHackType::GravityFlip:
			AbilityName = "GravityFlip";
			CastTime = 2.0f;
			Cooldown = 18.0f;
			EffectDuration = 2.0f;
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
	// If no target is set and this is a player-owned component, get target from crosshair
	if (!CurrentTarget && !bIsSelfTargeted)
	{
		if (AActor* Owner = GetOwner())
		{
			if (AcybersoulsCharacter* PlayerChar = Cast<AcybersoulsCharacter>(Owner))
			{
				CurrentTarget = PlayerChar->GetCrosshairTarget();
				
				// Special case for InterruptProtocol - can target any enemy casting QuickHack
				if (QuickHackType == EQuickHackType::InterruptProtocol && !CurrentTarget)
				{
					// Find all enemies in the world
					TArray<AActor*> FoundEnemies;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACybersoulsEnemyBase::StaticClass(), FoundEnemies);
					
					for (AActor* Enemy : FoundEnemies)
					{
						TArray<UQuickHackComponent*> QuickHacks;
						Enemy->GetComponents<UQuickHackComponent>(QuickHacks);
						
						for (UQuickHackComponent* QH : QuickHacks)
						{
							if (QH->IsQuickHackActive())
							{
								CurrentTarget = Enemy;
								UE_LOG(LogTemp, Warning, TEXT("Found enemy casting QuickHack - using as InterruptProtocol target"));
								break;
							}
						}
						
						if (CurrentTarget) break;
					}
				}
			}
		}
	}
	
	// For self-targeted abilities, use the owner
	if (bIsSelfTargeted)
	{
		CurrentTarget = GetOwner();
	}
	
	// Only activate if we have a valid target
	if (CurrentTarget)
	{
		CurrentCastTime = 0.0f;
		Super::ActivateAbility();
		UE_LOG(LogTemp, Warning, TEXT("Starting QuickHack: %s on %s"), *AbilityName, *CurrentTarget->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot activate QuickHack: %s - No valid target"), *AbilityName);
	}
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
			
		case EQuickHackType::CascadeVirus:
			// Mark target for cascade effect
			{
				ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(CurrentTarget);
				if (Enemy)
				{
					MarkEnemyForCascade(Enemy);
					UE_LOG(LogTemp, Warning, TEXT("Cascade Virus: Enemy marked for %f seconds"), EffectDuration);
				}
			}
			break;
			
		case EQuickHackType::GhostProtocol:
			// Make player invisible to hack enemies
			if (PlayerAttributes)
			{
				PlayerAttributes->bIsInvisibleToHackers = true;
				
				// Set timer to remove effect
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [PlayerAttributes]()
				{
					PlayerAttributes->bIsInvisibleToHackers = false;
					UE_LOG(LogTemp, Warning, TEXT("Ghost Protocol: Effect ended"));
				}, EffectDuration, false);
				
				UE_LOG(LogTemp, Warning, TEXT("Ghost Protocol: Player invisible to hackers for %f seconds"), EffectDuration);
			}
			break;
			
		case EQuickHackType::ChargeDrain:
			// Deplete target's block/dodge charges
			{
				ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(CurrentTarget);
				if (Enemy)
				{
					UBlockAbilityComponent* BlockComp = Enemy->FindComponentByClass<UBlockAbilityComponent>();
					if (BlockComp)
					{
						BlockComp->CurrentBlockCharges = 0;
						UE_LOG(LogTemp, Warning, TEXT("Charge Drain: Block charges depleted"));
					}
					
					UDodgeAbilityComponent* DodgeComp = Enemy->FindComponentByClass<UDodgeAbilityComponent>();
					if (DodgeComp)
					{
						DodgeComp->CurrentDodgeCharges = 0;
						UE_LOG(LogTemp, Warning, TEXT("Charge Drain: Dodge charges depleted"));
					}
				}
			}
			break;
			
		case EQuickHackType::GravityFlip:
			// Reverse gravity for target
			{
				ACharacter* TargetChar = Cast<ACharacter>(CurrentTarget);
				if (TargetChar && TargetChar->GetCharacterMovement())
				{
					TargetChar->GetCharacterMovement()->GravityScale = -1.0f;
					
					// Set timer to restore gravity
					FTimerHandle TimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, [TargetChar]()
					{
						if (TargetChar && TargetChar->GetCharacterMovement())
						{
							TargetChar->GetCharacterMovement()->GravityScale = 1.0f;
							UE_LOG(LogTemp, Warning, TEXT("Gravity Flip: Effect ended"));
						}
					}, EffectDuration, false);
					
					UE_LOG(LogTemp, Warning, TEXT("Gravity Flip: Target gravity reversed for %f seconds"), EffectDuration);
				}
			}
			break;
			
		default:
			break;
	}
}

void UQuickHackComponent::OnEnemyKilled(AActor* KilledEnemy)
{
	// Check if this enemy was marked for cascade
	if (MarkedEnemies.Contains(KilledEnemy))
	{
		TriggerCascadeEffect(KilledEnemy);
		RemoveMarkFromEnemy(KilledEnemy);
	}
}

void UQuickHackComponent::MarkEnemyForCascade(AActor* Enemy)
{
	if (Enemy && !MarkedEnemies.Contains(Enemy))
	{
		MarkedEnemies.Add(Enemy);
		
		// Set timer to remove mark after duration
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Enemy]()
		{
			RemoveMarkFromEnemy(Enemy);
		}, EffectDuration, false);
	}
}

void UQuickHackComponent::RemoveMarkFromEnemy(AActor* Enemy)
{
	MarkedEnemies.Remove(Enemy);
}

void UQuickHackComponent::TriggerCascadeEffect(AActor* KilledEnemy)
{
	TArray<AActor*> NearbyEnemies = GetNearbyEnemies(KilledEnemy);
	
	// Kill up to 2 nearby enemies
	int32 KillCount = 0;
	for (AActor* Enemy : NearbyEnemies)
	{
		if (KillCount >= 2) break;
		
		ACybersoulsEnemyBase* EnemyBase = Cast<ACybersoulsEnemyBase>(Enemy);
		if (EnemyBase && !EnemyBase->IsDead() && Enemy != KilledEnemy)
		{
			// Set timer to kill this enemy after 2 seconds
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [EnemyBase]()
			{
				if (EnemyBase && !EnemyBase->IsDead())
				{
					UEnemyAttributeComponent* EnemyAttributes = EnemyBase->FindComponentByClass<UEnemyAttributeComponent>();
					if (EnemyAttributes)
					{
						EnemyAttributes->TakeDamage(100.0f);
						UE_LOG(LogTemp, Warning, TEXT("Cascade Virus: Delayed kill triggered"));
					}
				}
			}, 2.0f, false);
			
			KillCount++;
			UE_LOG(LogTemp, Warning, TEXT("Cascade Virus: Enemy marked for delayed death"));
		}
	}
}

TArray<AActor*> UQuickHackComponent::GetNearbyEnemies(AActor* CenterEnemy, float Radius)
{
	TArray<AActor*> FoundEnemies;
	if (!CenterEnemy) return FoundEnemies;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	IgnoreActors.Add(CenterEnemy);
	
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		CenterEnemy->GetActorLocation(),
		Radius,
		ObjectTypes,
		ACybersoulsEnemyBase::StaticClass(),
		IgnoreActors,
		FoundEnemies
	);
	
	return FoundEnemies;
}