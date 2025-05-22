// PhysicalEnemyAIController.cpp
#include "cybersouls/Public/AI/PhysicalEnemyAIController.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Abilities/AttackAbilityComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

APhysicalEnemyAIController::APhysicalEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Default values for close combat
	AttackRange = 300.0f;
	ChaseSpeed = 400.0f;
	AcceptanceRadius = 50.0f;
	SightRange = 1500.0f;
}

void APhysicalEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Find player target
	PlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void APhysicalEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledEnemy = Cast<ACybersoulsEnemyBase>(InPawn);
	if (!ControlledEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAIController possessed non-enemy pawn!"));
	}
}

void APhysicalEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdateCombatBehavior();
}

void APhysicalEnemyAIController::UpdateCombatBehavior()
{
	if (!ControlledEnemy || !PlayerTarget || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Check if player is in sight
	if (CanSeeTarget(PlayerTarget))
	{
		// Move towards player if not in attack range
		if (!IsInAttackRange())
		{
			MoveToTarget();
		}
		else
		{
			// Stop movement and attack
			StopMovement();
			
			// Face the target
			FVector Direction = (PlayerTarget->GetActorLocation() - ControlledEnemy->GetActorLocation()).GetSafeNormal();
			ControlledEnemy->SetActorRotation(FRotator(0.0f, Direction.Rotation().Yaw, 0.0f));
			
			// Attack if not already attacking
			if (!GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
			{
				PerformAttack();
			}
		}
	}
	else
	{
		// Lost sight of player, stop movement
		StopMovement();
	}
}

bool APhysicalEnemyAIController::CanSeeTarget(AActor* Target) const
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

float APhysicalEnemyAIController::GetDistanceToTarget(AActor* Target) const
{
	if (!Target || !ControlledEnemy)
	{
		return FLT_MAX;
	}
	
	return FVector::Dist(ControlledEnemy->GetActorLocation(), Target->GetActorLocation());
}

void APhysicalEnemyAIController::MoveToTarget()
{
	if (!PlayerTarget || !ControlledEnemy)
	{
		return;
	}
	
	// Use navigation system to move to player
	MoveToActor(PlayerTarget, AcceptanceRadius);
}

void APhysicalEnemyAIController::PerformAttack()
{
	if (!ControlledEnemy || !PlayerTarget || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Get attack ability component
	UAttackAbilityComponent* AttackAbility = ControlledEnemy->FindComponentByClass<UAttackAbilityComponent>();
	if (AttackAbility && AttackAbility->CanActivateAbility())
	{
		AttackAbility->ActivateAbility();
		
		// Set attack timer based on attack cooldown
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APhysicalEnemyAIController::PerformAttack, AttackAbility->AttackCooldown, false);
	}
}

bool APhysicalEnemyAIController::IsInAttackRange() const
{
	return GetDistanceToTarget(PlayerTarget) <= AttackRange;
}