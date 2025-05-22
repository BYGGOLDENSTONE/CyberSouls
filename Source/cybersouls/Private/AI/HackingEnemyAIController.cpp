// HackingEnemyAIController.cpp
#include "cybersouls/Public/AI/HackingEnemyAIController.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Abilities/HackAbilityComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "cybersouls/Public/Attributes/HackingEnemyAttributeComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/ActorComponent.h"

AHackingEnemyAIController::AHackingEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Default values for ranged combat
	HackRange = 1500.0f;
	SafeDistance = 800.0f;
	RetreatSpeed = 300.0f;
	SightRange = 2000.0f;
}

void AHackingEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Find player target
	PlayerTarget = Cast<AcybersoulsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AHackingEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledEnemy = Cast<ACybersoulsEnemyBase>(InPawn);
	if (!ControlledEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("HackingEnemyAIController possessed non-enemy pawn!"));
	}
	else
	{
		// Cache QuickHack components
		CacheQuickHackComponents();
	}
}

void AHackingEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateHackingBehavior();
}

void AHackingEnemyAIController::UpdateHackingBehavior()
{
	if (!ControlledEnemy || !PlayerTarget || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Check if player is in sight
	if (CanSeeTarget(PlayerTarget))
	{
		// Maintain optimal distance
		MaintainDistance();
		
		// Face the target
		FVector Direction = (PlayerTarget->GetActorLocation() - ControlledEnemy->GetActorLocation()).GetSafeNormal();
		ControlledEnemy->SetActorRotation(FRotator(0.0f, Direction.Rotation().Yaw, 0.0f));
		
		// Perform hacking if in range
		if (IsInHackRange())
		{
			PerformHacking();
			
			// Attempt QuickHack if available
			if (!GetWorldTimerManager().IsTimerActive(QuickHackTimerHandle))
			{
				AttemptQuickHack();
			}
		}
	}
	else
	{
		// Lost sight of player, stop movement
		StopMovement();
	}
}

bool AHackingEnemyAIController::CanSeeTarget(AActor* Target) const
{
	if (!Target || !ControlledEnemy)
	{
		return false;
	}
	
	// Check distance
	float Distance = GetDistanceToTarget(Target);
	if (Distance > SightRange)
	{
		return false;
	}
	
	// Line of sight check
	FVector StartLocation = ControlledEnemy->GetActorLocation();
	FVector EndLocation = Target->GetActorLocation();
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ControlledEnemy);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);
	
	return !bHit || HitResult.GetActor() == Target;
}

float AHackingEnemyAIController::GetDistanceToTarget(AActor* Target) const
{
	if (!Target || !ControlledEnemy)
	{
		return FLT_MAX;
	}
	
	return FVector::Dist(ControlledEnemy->GetActorLocation(), Target->GetActorLocation());
}

void AHackingEnemyAIController::MaintainDistance()
{
	if (!PlayerTarget || !ControlledEnemy)
	{
		return;
	}
	
	float CurrentDistance = GetDistanceToTarget(PlayerTarget);
	
	if (IsTooClose())
	{
		// Retreat from player
		FVector RetreatDirection = (ControlledEnemy->GetActorLocation() - PlayerTarget->GetActorLocation()).GetSafeNormal();
		FVector RetreatTarget = ControlledEnemy->GetActorLocation() + RetreatDirection * (SafeDistance - CurrentDistance + 100.0f);
		
		// Navigate to retreat position
		MoveToLocation(RetreatTarget);
	}
	else if (CurrentDistance > HackRange * 0.9f) // Stay within hack range
	{
		// Move closer to player
		MoveToActor(PlayerTarget, HackRange * 0.8f);
	}
	else
	{
		// In optimal range, stop moving
		StopMovement();
	}
}

void AHackingEnemyAIController::PerformHacking()
{
	if (!ControlledEnemy || !PlayerTarget || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Get hack ability component
	UHackAbilityComponent* HackAbility = ControlledEnemy->FindComponentByClass<UHackAbilityComponent>();
	if (HackAbility && HackAbility->CanActivateAbility())
	{
		HackAbility->ActivateAbility();
	}
}

void AHackingEnemyAIController::AttemptQuickHack()
{
	if (!ControlledEnemy || !PlayerTarget || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Check if this enemy has QuickHack abilities
	UHackingEnemyAttributeComponent* HackingAttributes = ControlledEnemy->FindComponentByClass<UHackingEnemyAttributeComponent>();
	if (!HackingAttributes || !HackingAttributes->bHasQuickHacks)
	{
		return;
	}
	
	// Decide which QuickHack to use
	EEnemyQuickHackType ChosenType = DecideQuickHackType();
	
	if (ChosenType != EEnemyQuickHackType::None)
	{
		ExecuteQuickHack(ChosenType);
	}
	
	// Set timer for next attempt
	GetWorldTimerManager().SetTimer(QuickHackTimerHandle, this, &AHackingEnemyAIController::AttemptQuickHack, QuickHackDecisionInterval, false);
}

bool AHackingEnemyAIController::IsInHackRange() const
{
	return GetDistanceToTarget(PlayerTarget) <= HackRange;
}

bool AHackingEnemyAIController::IsTooClose() const
{
	return GetDistanceToTarget(PlayerTarget) < SafeDistance;
}

bool AHackingEnemyAIController::IsPlayerCastingQuickHack() const
{
	if (!PlayerTarget)
	{
		return false;
	}
	
	// Check player's QuickHack components
	TArray<UQuickHackComponent*> PlayerQuickHacks;
	PlayerTarget->GetComponents<UQuickHackComponent>(PlayerQuickHacks);
	
	for (UQuickHackComponent* QuickHack : PlayerQuickHacks)
	{
		if (QuickHack && QuickHack->IsQuickHackActive())
		{
			return true;
		}
	}
	
	return false;
}

EEnemyQuickHackType AHackingEnemyAIController::DecideQuickHackType() const
{
	if (!ControlledEnemy)
	{
		return EEnemyQuickHackType::None;
	}
	
	// Check for InterruptProtocol first - only if player is casting
	if (InterruptProtocolComponent && CanUseQuickHack(InterruptProtocolComponent))
	{
		if (IsPlayerCastingQuickHack())
		{
			return EEnemyQuickHackType::InterruptProtocol;
		}
	}
	
	// For BuffNetrunner - use Firewall when not already protected
	if (FirewallComponent && CanUseQuickHack(FirewallComponent))
	{
		// Check if already has Firewall active
		if (!FirewallComponent->IsQuickHackActive())
		{
			return EEnemyQuickHackType::Firewall;
		}
	}
	
	// For DebuffNetrunner - use SystemFreeze to immobilize player
	if (SystemFreezeComponent && CanUseQuickHack(SystemFreezeComponent))
	{
		return EEnemyQuickHackType::SystemFreeze;
	}
	
	return EEnemyQuickHackType::None;
}

bool AHackingEnemyAIController::CanUseQuickHack(UQuickHackComponent* QuickHack) const
{
	if (!QuickHack)
	{
		return false;
	}
	
	// Check if not on cooldown and not already active
	return QuickHack->CurrentCooldown <= 0.0f && !QuickHack->IsQuickHackActive();
}

void AHackingEnemyAIController::ExecuteQuickHack(EEnemyQuickHackType Type)
{
	if (!PlayerTarget)
	{
		return;
	}
	
	UQuickHackComponent* QuickHackToUse = nullptr;
	
	switch (Type)
	{
		case EEnemyQuickHackType::Firewall:
			QuickHackToUse = FirewallComponent;
			break;
		case EEnemyQuickHackType::SystemFreeze:
			QuickHackToUse = SystemFreezeComponent;
			break;
		case EEnemyQuickHackType::InterruptProtocol:
			QuickHackToUse = InterruptProtocolComponent;
			break;
		default:
			return;
	}
	
	if (QuickHackToUse && CanUseQuickHack(QuickHackToUse))
	{
		// For self-targeted abilities like Firewall
		if (QuickHackToUse->bIsSelfTargeted)
		{
			QuickHackToUse->StartQuickHack(ControlledEnemy);
		}
		else
		{
			QuickHackToUse->StartQuickHack(PlayerTarget);
		}
	}
}

void AHackingEnemyAIController::CacheQuickHackComponents()
{
	if (!ControlledEnemy)
	{
		return;
	}
	
	// Get all QuickHack components
	TArray<UQuickHackComponent*> QuickHacks;
	ControlledEnemy->GetComponents<UQuickHackComponent>(QuickHacks);
	
	// Cache specific types
	for (UQuickHackComponent* QuickHack : QuickHacks)
	{
		if (QuickHack)
		{
			switch (QuickHack->QuickHackType)
			{
				case EQuickHackType::Firewall:
					FirewallComponent = QuickHack;
					break;
				case EQuickHackType::SystemFreeze:
					SystemFreezeComponent = QuickHack;
					break;
				case EQuickHackType::InterruptProtocol:
					InterruptProtocolComponent = QuickHack;
					break;
			}
		}
	}
}