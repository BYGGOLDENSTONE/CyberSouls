#include "cybersouls/Public/AI/BaseEnemyAIController.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Character/PlayerCyberState.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/CybersoulsUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

ABaseEnemyAIController::ABaseEnemyAIController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.1f; // Tick every 0.1 seconds for performance
}

void ABaseEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    ControlledEnemy = Cast<ACybersoulsEnemyBase>(InPawn);
    
    // Initialize player target using the new logic that respects character types
    UpdatePlayerTarget();
}

void ABaseEnemyAIController::OnUnPossess()
{
    StopAlertingAllies();
    Super::OnUnPossess();
}

bool ABaseEnemyAIController::CanSeeTarget(AActor* Target) const
{
    if (!Target || !GetPawn())
    {
        return false;
    }

    FVector StartLocation = GetPawn()->GetActorLocation() + FVector(0, 0, EyeHeight);
    FVector EndLocation = Target->GetActorLocation();
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetPawn());
    QueryParams.AddIgnoredActor(Target);
    
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    if (bDebugDrawSightLine && GEngine)
    {
        FColor LineColor = bHit ? FColor::Red : FColor::Green;
        DrawDebugLine(
            GetWorld(),
            StartLocation,
            EndLocation,
            LineColor,
            false,
            0.5f,
            0,
            2.0f
        );
    }

    return !bHit;
}

float ABaseEnemyAIController::GetDistanceToTarget(AActor* Target) const
{
    return UCybersoulsUtils::GetDistanceBetweenActors(GetPawn(), Target);
}

void ABaseEnemyAIController::AlertNearbyEnemies(AActor* Target)
{
    if (!Target || !GetPawn())
    {
        return;
    }

    CurrentTarget = Target;
    bIsAlertingAllies = true;
    
    // Initial alert
    UpdateAlliesWithPlayerLocation();
    
    // Set up timer for continuous updates
    GetWorld()->GetTimerManager().SetTimer(
        AlertUpdateTimerHandle,
        this,
        &ABaseEnemyAIController::UpdateAlliesWithPlayerLocation,
        AlertUpdateInterval,
        true
    );
}

void ABaseEnemyAIController::UpdateAlliesWithPlayerLocation()
{
    if (!CurrentTarget || !GetPawn() || !bIsAlertingAllies)
    {
        return;
    }

    TArray<AActor*> FoundEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACybersoulsEnemyBase::StaticClass(), FoundEnemies);
    
    for (AActor* Enemy : FoundEnemies)
    {
        if (Enemy && Enemy != GetPawn())
        {
            if (UCybersoulsUtils::IsActorWithinRange(GetPawn(), Enemy, AlertRadius))
            {
                APawn* EnemyPawn = Cast<APawn>(Enemy);
                if (EnemyPawn && EnemyPawn->GetController())
                {
                    if (ABaseEnemyAIController* BaseAI = Cast<ABaseEnemyAIController>(EnemyPawn->GetController()))
                    {
                        BaseAI->ReceivePlayerLocationUpdate(CurrentTarget, CurrentTarget->GetActorLocation());
                    }
                }
            }
        }
    }
}

void ABaseEnemyAIController::StopAlertingAllies()
{
    bIsAlertingAllies = false;
    CurrentTarget = nullptr;
    GetWorld()->GetTimerManager().ClearTimer(AlertUpdateTimerHandle);
}

void ABaseEnemyAIController::UpdatePlayerTarget()
{
    // Get the current player character
    AActor* NewPlayerTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    
    // Check if the new target is a CyberState character - if so, ignore it
    if (NewPlayerTarget)
    {
        // Only chase default character (cybersoulsCharacter), ignore CyberState
        APlayerCyberState* CyberStateChar = Cast<APlayerCyberState>(NewPlayerTarget);
        if (CyberStateChar)
        {
            // Player is using CyberState - enemies should not chase
            PlayerTarget = nullptr;
            UE_LOG(LogTemp, Warning, TEXT("BaseEnemyAI: Player using CyberState - %s will not chase"), 
                ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"));
            return;
        }
        
        // Player is using default character - update target normally
        if (NewPlayerTarget != PlayerTarget)
        {
            PlayerTarget = NewPlayerTarget;
            UE_LOG(LogTemp, Warning, TEXT("BaseEnemyAI: Updated player target to %s for %s"), 
                *PlayerTarget->GetName(), 
                ControlledEnemy ? *ControlledEnemy->GetName() : TEXT("Unknown"));
        }
    }
}