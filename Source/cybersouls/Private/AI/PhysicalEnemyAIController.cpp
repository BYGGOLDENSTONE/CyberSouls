// PhysicalEnemyAIController.cpp
#include "cybersouls/Public/AI/PhysicalEnemyAIController.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Abilities/AttackAbilityComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

APhysicalEnemyAIController::APhysicalEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Default values for close combat
	ChaseSpeed = 600.0f;       // Faster chase speed
	AcceptanceRadius = 80.0f;  // Stop closer to player
}

APhysicalEnemyAIController::~APhysicalEnemyAIController()
{
	// Clean up timer if still active
	if (GetWorld() && GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	}
}

void APhysicalEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Enable movement updates
	SetActorTickEnabled(true);
	
	// Log AI controller setup
	if (PlayerTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: Found player target!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicalEnemyAI: No player target found!"));
	}
}

void APhysicalEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	static int TickCounter = 0;
	TickCounter++;
	
	// Log every 30 ticks (roughly every 3 seconds at 0.1s tick interval)
	if (TickCounter % 30 == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI Tick: Enemy=%s, PlayerTarget=%s, Pawn=%s"), 
			ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("NULL"),
			PlayerTarget ? *PlayerTarget->GetName() : TEXT("NULL"),
			GetPawn() ? *GetPawn()->GetName() : TEXT("NULL"));
	}
	
	if (!IsValid(PlayerTarget) || !IsValid(GetPawn()))
	{
		return;
	}
	
	// Check if we can see the player
	if (CanSeeTarget(PlayerTarget))
	{
		bHasSeenPlayer = true;
		LastKnownPlayerLocation = PlayerTarget->GetActorLocation();
		LastSeenTime = GetWorld()->GetTimeSeconds();
		
		// Alert nearby allies
		AlertNearbyEnemies(PlayerTarget);
		
		// Check distance to player
		float DistanceToPlayer = FVector::Distance(GetPawn()->GetActorLocation(), PlayerTarget->GetActorLocation());
		
		// Log when we first see the player
		static bool bWasVisible = false;
		if (!bWasVisible)
		{
			float ActualAttackRange = 150.0f; // Default
			if (ControlledEnemy)
			{
				UAttackAbilityComponent* AttackComponent = ControlledEnemy->FindComponentByClass<UAttackAbilityComponent>();
				if (AttackComponent)
				{
					ActualAttackRange = AttackComponent->GetAttackRange();
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: %s spotted player at distance %.1f (AttackRange=%.1f)"), 
				*GetPawn()->GetName(), DistanceToPlayer, ActualAttackRange);
		}
		bWasVisible = true;
		
		if (IsInAttackRange())
		{
			// Stop moving and attack
			StopMovement();
			PerformAttack();
		}
		else
		{
			// Chase the player
			MoveToLocation(PlayerTarget->GetActorLocation());
		}
	}
	else if (bHasSeenPlayer)
	{
		// Lost sight of player, search last known location
		float TimeSinceSeen = GetWorld()->GetTimeSeconds() - LastSeenTime;
		if (TimeSinceSeen < SearchTime)
		{
			// Move to last known location
			MoveToLocation(LastKnownPlayerLocation);
		}
		else
		{
			// Give up searching
			bHasSeenPlayer = false;
			StopMovement();
		}
	}
}

void APhysicalEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAIController::OnPossess called for pawn: %s"), InPawn ? *InPawn->GetName() : TEXT("NULL"));
	
	if (!ControlledEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicalEnemyAIController possessed non-enemy pawn!"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: Successfully possessed enemy %s"), *ControlledEnemy->GetName());
	
	// Set movement speed
	if (ControlledEnemy->GetCharacterMovement())
	{
		ControlledEnemy->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: Set movement speed to %.1f for %s"), ChaseSpeed, *ControlledEnemy->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicalEnemyAI: No movement component found for %s"), *ControlledEnemy->GetName());
	}
	
	// Setup path following for smoother movement
	if (UPathFollowingComponent* PathFollowing = GetPathFollowingComponent())
	{
		PathFollowing->OnRequestFinished.AddUObject(this, &APhysicalEnemyAIController::OnMoveCompleted);
		UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: Path following component setup complete"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicalEnemyAI: No path following component found"));
	}
	
	// Log player target status
	if (PlayerTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: Player target found: %s"), *PlayerTarget->GetName());
		
		// Start pursuing player immediately if visible
		if (CanSeeTarget(PlayerTarget))
		{
			UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: Can see player, starting movement"));
			MoveToTarget();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PhysicalEnemyAI: Cannot see player initially"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicalEnemyAI: No player target found!"));
	}
}

void APhysicalEnemyAIController::OnUnPossess()
{
	// Clear attack timer when unpossessing
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	
	Super::OnUnPossess();
}


void APhysicalEnemyAIController::UpdateCombatBehavior()
{
	if (!ControlledEnemy || !PlayerTarget || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Player target is set in base class OnPossess
	
	// Check if player is in sight
	if (CanSeeTarget(PlayerTarget))
	{
		// Update last known location when we can see the player
		LastKnownPlayerLocation = PlayerTarget->GetActorLocation();
		bIsSearching = false; // Stop searching if we found the player
		bIsAlerted = false; // Clear alert status when we see the player
		
		// Alert nearby enemies when we first spot the player
		static bool bWasPlayerVisible = false;
		if (!bWasPlayerVisible)
		{
			HandlePlayerVisibility();
		}
		bWasPlayerVisible = true;
		
		// Move towards player if not in attack range
		if (!IsInAttackRange())
		{
			// Only start new movement if not already moving
			FAIRequestID CurrentRequestID = GetCurrentMoveRequestID();
			if (!CurrentRequestID.IsValid())
			{
				MoveToTarget();
			}
			UE_LOG(LogTemp, Verbose, TEXT("%s: Chasing player, distance: %.1f"), *ControlledEnemy->GetName(), GetDistanceToTarget(PlayerTarget));
		}
		else
		{
			// Stop movement and attack
			StopMovement();
			
			// Face the target smoothly
			if (PlayerTarget)
			{
				FVector Direction = (PlayerTarget->GetActorLocation() - ControlledEnemy->GetActorLocation()).GetSafeNormal();
				FRotator TargetRotation = FRotator(0.0f, Direction.Rotation().Yaw, 0.0f);
				ControlledEnemy->SetActorRotation(FMath::RInterpTo(ControlledEnemy->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 10.0f));
			}
			
			// Attack if not already attacking
			if (!GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
			{
				PerformAttack();
				UE_LOG(LogTemp, Log, TEXT("%s: Attacking player!"), *ControlledEnemy->GetName());
			}
		}
	}
	else
	{
		// Update visibility tracking
		static bool bWasPlayerVisible = true;
		if (bWasPlayerVisible)
		{
			// Just lost sight
			HandlePlayerLostVisibility();
		}
		bWasPlayerVisible = false;
		
		// Check if we should respond to an alert
		if (bIsAlerted && !bIsSearching)
		{
			// Move to alert location
			LastKnownPlayerLocation = AlertLocation;
			StartSearchBehavior();
			bIsAlerted = false; // Clear the alert
		}
		else if (!bIsSearching)
		{
			// Lost sight of player, start search behavior
			StartSearchBehavior();
		}
	}
}

void APhysicalEnemyAIController::MoveToTarget()
{
	if (!PlayerTarget || !ControlledEnemy || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Configure movement request
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(PlayerTarget);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	MoveRequest.SetNavigationFilter(nullptr);
	MoveRequest.SetAllowPartialPath(true);
	MoveRequest.SetProjectGoalLocation(true);
	MoveRequest.SetReachTestIncludesAgentRadius(true);
	MoveRequest.SetUsePathfinding(true);
	
	// Move to target
	FNavPathSharedPtr NavPath;
	MoveTo(MoveRequest, &NavPath);
	
	// Debug logging
	if (NavPath.IsValid())
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: Moving to player"), *ControlledEnemy->GetName());
	}
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
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APhysicalEnemyAIController::PerformAttack, AttackAbility->GetAttackCooldown(), false);
	}
}

bool APhysicalEnemyAIController::IsInAttackRange() const
{
	// Use the same attack range as the AttackAbilityComponent
	if (ControlledEnemy)
	{
		UAttackAbilityComponent* AttackComponent = ControlledEnemy->FindComponentByClass<UAttackAbilityComponent>();
		if (AttackComponent)
		{
			return GetDistanceToTarget(PlayerTarget) <= AttackComponent->GetAttackRange();
		}
	}
	// Fallback to hardcoded value
	return GetDistanceToTarget(PlayerTarget) <= 150.0f;
}

void APhysicalEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	// Log movement completion
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: Reached destination"), 
			ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"));
			
		// If we're searching and reached the last known location, stop searching after timeout
		if (bIsSearching && FVector::Dist(ControlledEnemy->GetActorLocation(), LastKnownPlayerLocation) < AcceptanceRadius)
		{
			UE_LOG(LogTemp, Verbose, TEXT("%s: Reached last known player location, will search for %.1f more seconds"), 
				ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"), SearchTimeRemaining);
		}
	}
	else if (Result.IsInterrupted())
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: Movement interrupted"), 
			ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"));
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: Movement failed"), 
			ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"));
	}
}

void APhysicalEnemyAIController::MoveToLocation(const FVector& Location)
{
	if (!ControlledEnemy || ControlledEnemy->IsDead())
	{
		return;
	}
	
	// Configure movement request for location
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Location);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	MoveRequest.SetNavigationFilter(nullptr);
	MoveRequest.SetAllowPartialPath(true);
	MoveRequest.SetProjectGoalLocation(true);
	MoveRequest.SetReachTestIncludesAgentRadius(true);
	MoveRequest.SetUsePathfinding(true);
	
	// Move to location
	FNavPathSharedPtr NavPath;
	MoveTo(MoveRequest, &NavPath);
}

void APhysicalEnemyAIController::StartSearchBehavior()
{
	if (!ControlledEnemy || !PlayerTarget)
	{
		return;
	}
	
	bIsSearching = true;
	SearchTimeRemaining = MaxSearchTime;
	
	// Move to last known location
	MoveToLocation(LastKnownPlayerLocation);
	
	UE_LOG(LogTemp, Warning, TEXT("%s: Lost sight of player, moving to last known location"), *ControlledEnemy->GetName());
}

void APhysicalEnemyAIController::UpdateSearchBehavior(float DeltaTime)
{
	if (!ControlledEnemy || !PlayerTarget || ControlledEnemy->IsDead())
	{
		bIsSearching = false;
		return;
	}
	
	// Check if we can see the player again
	if (CanSeeTarget(PlayerTarget))
	{
		bIsSearching = false;
		SearchTimeRemaining = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("%s: Found player during search!"), *ControlledEnemy->GetName());
		return;
	}
	
	// Update search timer
	SearchTimeRemaining -= DeltaTime;
	
	// If search time expired, stop searching
	if (SearchTimeRemaining <= 0.0f)
	{
		bIsSearching = false;
		StopMovement();
		UE_LOG(LogTemp, Warning, TEXT("%s: Search time expired, giving up"), *ControlledEnemy->GetName());
		return;
	}
	
	// If we're at the last known location and still searching, look around
	float DistanceToLastKnown = FVector::Dist(ControlledEnemy->GetActorLocation(), LastKnownPlayerLocation);
	if (DistanceToLastKnown < AcceptanceRadius * 2.0f)
	{
		// Rotate to look around
		FRotator CurrentRotation = ControlledEnemy->GetActorRotation();
		CurrentRotation.Yaw += 90.0f * DeltaTime; // Rotate 90 degrees per second
		ControlledEnemy->SetActorRotation(CurrentRotation);
	}
}

void APhysicalEnemyAIController::ReceiveAlert(AActor* AlertTarget, const FVector& InAlertLocation)
{
	// Don't respond to alerts if we can already see the player
	if (CanSeeTarget(PlayerTarget))
	{
		return;
	}
	
	// Update the alert location
	AlertLocation = InAlertLocation;
	LastKnownPlayerLocation = InAlertLocation;
	
	// If already searching, just update the location
	if (bIsSearching)
	{
		// Update search destination
		MoveToLocation(LastKnownPlayerLocation);
		SearchTimeRemaining = MaxSearchTime; // Reset search timer
		UE_LOG(LogTemp, Verbose, TEXT("%s: Updated search location to %s"), 
			ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"),
			*InAlertLocation.ToString());
	}
	else
	{
		// Store the alert and it will be processed in UpdateCombatBehavior
		bIsAlerted = true;
		UE_LOG(LogTemp, Warning, TEXT("%s: Received alert! Player spotted at %s"), 
			ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"),
			*InAlertLocation.ToString());
	}
}

void APhysicalEnemyAIController::ReceivePlayerLocationUpdate(AActor* Player, const FVector& PlayerLocation)
{
	// Update last known location if we're already tracking this player
	if (PlayerTarget == Player)
	{
		LastKnownPlayerLocation = PlayerLocation;
	}
}

void APhysicalEnemyAIController::HandlePlayerVisibility()
{
	// Alert nearby enemies when we spot the player
	if (CurrentTarget != PlayerTarget)
	{
		AlertNearbyEnemies(PlayerTarget);
	}
}

void APhysicalEnemyAIController::HandlePlayerLostVisibility()
{
	// Stop alerting when we lose sight
	StopAlertingAllies();
	
	// Start search behavior
	StartSearchBehavior();
}