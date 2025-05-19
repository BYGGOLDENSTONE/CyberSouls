// CybersoulsEnemyAIController.cpp
#include "cybersouls/Public/Enemy/CybersoulsEnemyAIController.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ACybersoulsEnemyAIController::ACybersoulsEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACybersoulsEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ACybersoulsEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ControlledEnemy = Cast<ACybersoulsEnemyBase>(InPawn);
}

void ACybersoulsEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateBehavior();
}

void ACybersoulsEnemyAIController::UpdateBehavior()
{
	if (!ControlledEnemy)
	{
		return;
	}
	
	// Check if enemy is still alive (depends on enemy type)
	UEnemyAttributeComponent* EnemyAttrib = ControlledEnemy->FindComponentByClass<UEnemyAttributeComponent>();
	if (!EnemyAttrib || !EnemyAttrib->IsAlive())
	{
		return;
	}

	// Find player
	AcybersoulsCharacter* Player = Cast<AcybersoulsCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Player)
	{
		return;
	}
	
	// Check if player is alive
	UPlayerAttributeComponent* PlayerAttrib = Player->FindComponentByClass<UPlayerAttributeComponent>();
	if (!PlayerAttrib || !PlayerAttrib->IsAlive())
	{
		return;
	}

	// Check if we can see the player
	if (!CanSeeTarget(Player))
	{
		CurrentTarget = nullptr;
		return;
	}

	CurrentTarget = Player;
	float Distance = GetDistanceToTarget(Player);

	// Behavior based on enemy type
	switch (ControlledEnemy->EnemyType)
	{
		case EEnemyType::Basic:
		case EEnemyType::Block:
		case EEnemyType::Dodge:
			// Physical enemies - move to attack range and attack
			if (Distance > AttackRange)
			{
				MoveToActor(Player, AttackRange * 0.8f);
			}
			else
			{
				StopMovement();
				ControlledEnemy->PerformAttack(Player);
			}
			break;

		case EEnemyType::Netrunner:
		case EEnemyType::BuffNetrunner:
		case EEnemyType::DebuffNetrunner:
			// Hacking enemies - stay at range and hack
			if (Distance < AttackRange * 0.5f)
			{
				// Too close, back away
				FVector AwayFromPlayer = GetPawn()->GetActorLocation() - Player->GetActorLocation();
				AwayFromPlayer.Normalize();
				FVector Destination = GetPawn()->GetActorLocation() + AwayFromPlayer * AttackRange;
				MoveToLocation(Destination);
			}
			else if (Distance > HackRange)
			{
				// Too far, get closer
				MoveToActor(Player, HackRange * 0.8f);
			}
			else
			{
				// In range, perform hack
				StopMovement();
				ControlledEnemy->PerformHack(Player);
			}
			break;
	}
}

bool ACybersoulsEnemyAIController::CanSeeTarget(AActor* Target) const
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

	// Check line of sight
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ControlledEnemy);
	QueryParams.AddIgnoredActor(Target);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ControlledEnemy->GetActorLocation() + FVector(0, 0, 50),
		Target->GetActorLocation() + FVector(0, 0, 50),
		ECC_Visibility,
		QueryParams
	);

	return !bHit;
}

float ACybersoulsEnemyAIController::GetDistanceToTarget(AActor* Target) const
{
	if (!Target || !ControlledEnemy)
	{
		return FLT_MAX;
	}

	return FVector::Dist(ControlledEnemy->GetActorLocation(), Target->GetActorLocation());
}