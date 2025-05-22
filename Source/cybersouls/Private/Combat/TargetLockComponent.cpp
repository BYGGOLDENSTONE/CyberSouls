// TargetLockComponent.cpp
#include "cybersouls/Public/Combat/TargetLockComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UTargetLockComponent::UTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetLockComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update lock validity
	if (CurrentTarget)
	{
		UpdateLockValidity();
	}
}

void UTargetLockComponent::ToggleTargetLock()
{
	if (CurrentTarget)
	{
		UnlockTarget();
	}
	else
	{
		AActor* BestTarget = FindBestTarget();
		if (BestTarget)
		{
			LockTarget(BestTarget);
		}
	}
}

void UTargetLockComponent::LockTarget(AActor* Target)
{
	if (!Target || !IsValidTarget(Target))
	{
		return;
	}

	CurrentTarget = Target;
	TargetedBodyPart = EBodyPart::UpperBody;

	OnTargetLocked.Broadcast(CurrentTarget, TargetedBodyPart);
}

void UTargetLockComponent::UnlockTarget()
{
	if (CurrentTarget)
	{
		CurrentTarget = nullptr;
		OnTargetUnlocked.Broadcast();
	}
}

void UTargetLockComponent::ChangeTargetLeft()
{
	TArray<AActor*> ValidTargets = GetValidTargets();
	if (ValidTargets.Num() <= 1)
	{
		return;
	}

	int32 CurrentIndex = ValidTargets.IndexOfByKey(CurrentTarget);
	if (CurrentIndex != INDEX_NONE)
	{
		int32 NewIndex = (CurrentIndex - 1 + ValidTargets.Num()) % ValidTargets.Num();
		LockTarget(ValidTargets[NewIndex]);
	}
}

void UTargetLockComponent::ChangeTargetRight()
{
	TArray<AActor*> ValidTargets = GetValidTargets();
	if (ValidTargets.Num() <= 1)
	{
		return;
	}

	int32 CurrentIndex = ValidTargets.IndexOfByKey(CurrentTarget);
	if (CurrentIndex != INDEX_NONE)
	{
		int32 NewIndex = (CurrentIndex + 1) % ValidTargets.Num();
		LockTarget(ValidTargets[NewIndex]);
	}
}

void UTargetLockComponent::ChangeBodyPart(EBodyPart NewBodyPart)
{
	if (!CurrentTarget || TargetedBodyPart == NewBodyPart)
	{
		return;
	}

	UBodyPartComponent* BodyPart = GetBodyPartComponent(CurrentTarget, NewBodyPart);
	if (BodyPart && BodyPart->CanBeTargeted())
	{
		TargetedBodyPart = NewBodyPart;
		OnTargetLocked.Broadcast(CurrentTarget, TargetedBodyPart);
	}
}

AActor* UTargetLockComponent::FindBestTarget() const
{
	TArray<AActor*> ValidTargets = GetValidTargets();
	if (ValidTargets.Num() == 0)
	{
		return nullptr;
	}

	AActor* BestTarget = nullptr;
	float BestScore = -1.0f;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return nullptr;
	}

	FVector OwnerLocation = OwnerPawn->GetActorLocation();
	FVector OwnerForward = OwnerPawn->GetActorForwardVector();

	for (AActor* Target : ValidTargets)
	{
		FVector ToTarget = Target->GetActorLocation() - OwnerLocation;
		float Distance = ToTarget.Size();
		ToTarget.Normalize();

		float DotProduct = FVector::DotProduct(OwnerForward, ToTarget);
		float AngleScore = (DotProduct + 1.0f) * 0.5f; // Convert to 0-1 range
		float DistanceScore = 1.0f - (Distance / MaxLockDistance);

		float Score = (AngleScore * 0.7f) + (DistanceScore * 0.3f);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Target;
		}
	}

	return BestTarget;
}

TArray<AActor*> UTargetLockComponent::GetValidTargets() const
{
	TArray<AActor*> ValidTargets;

	if (!EnemyClass)
	{
		return ValidTargets;
	}

	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, AllEnemies);

	for (AActor* Enemy : AllEnemies)
	{
		if (IsValidTarget(Enemy))
		{
			ValidTargets.Add(Enemy);
		}
	}

	// Sort by angle from left to right
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		FVector OwnerLocation = OwnerPawn->GetActorLocation();
		FVector OwnerRight = OwnerPawn->GetActorRightVector();

		ValidTargets.Sort([&](const AActor& A, const AActor& B)
		{
			FVector ToA = A.GetActorLocation() - OwnerLocation;
			FVector ToB = B.GetActorLocation() - OwnerLocation;
			ToA.Normalize();
			ToB.Normalize();

			float DotA = FVector::DotProduct(OwnerRight, ToA);
			float DotB = FVector::DotProduct(OwnerRight, ToB);

			return DotA < DotB;
		});
	}

	return ValidTargets;
}

bool UTargetLockComponent::IsValidTarget(AActor* Target) const
{
	if (!Target || !Target->IsValidLowLevel() || Target->IsActorBeingDestroyed())
	{
		return false;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return false;
	}

	// Check distance
	float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), Target->GetActorLocation());
	if (Distance > MaxLockDistance)
	{
		return false;
	}

	// Check angle
	FVector ToTarget = Target->GetActorLocation() - OwnerPawn->GetActorLocation();
	ToTarget.Normalize();
	float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(OwnerPawn->GetActorForwardVector(), ToTarget)));
	if (Angle > MaxLockAngle)
	{
		return false;
	}

	// Check line of sight
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn);
	QueryParams.AddIgnoredActor(Target);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		OwnerPawn->GetActorLocation(),
		Target->GetActorLocation(),
		ECC_Visibility,
		QueryParams
	);

	return !bHit;
}

void UTargetLockComponent::UpdateLockValidity()
{
	if (!IsValidTarget(CurrentTarget))
	{
		UnlockTarget();
	}
}

UBodyPartComponent* UTargetLockComponent::GetBodyPartComponent(AActor* Actor, EBodyPart BodyPart) const
{
	if (!Actor)
	{
		return nullptr;
	}

	TArray<UBodyPartComponent*> BodyParts;
	Actor->GetComponents<UBodyPartComponent>(BodyParts);

	for (UBodyPartComponent* Component : BodyParts)
	{
		if (Component->BodyPartType == BodyPart)
		{
			return Component;
		}
	}

	return nullptr;
}