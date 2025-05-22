// DodgeAbilityComponent.cpp
#include "cybersouls/Public/Abilities/DodgeAbilityComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"

UDodgeAbilityComponent::UDodgeAbilityComponent()
{
	AbilityName = "Dodge";
	// Dodge doesn't have a traditional cooldown, it uses charges
	Cooldown = 0.0f;
}

void UDodgeAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentDodgeCharges = MaxDodgeCharges;
}

bool UDodgeAbilityComponent::TryDodge(EBodyPart AttackedBodyPart, AActor* Attacker)
{
	// Check if owner is dead
	if (ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(GetOwner()))
	{
		if (Enemy->IsDead())
		{
			return false;
		}
	}
	
	if (CanDodge(AttackedBodyPart) && CurrentDodgeCharges > 0)
	{
		ConsumeDodgeCharge();
		PerformDodgeMovement(Attacker);
		UE_LOG(LogTemp, Warning, TEXT("Dodged attack! Charges remaining: %d"), CurrentDodgeCharges);
		return true;
	}
	
	return false;
}

bool UDodgeAbilityComponent::CanDodge(EBodyPart BodyPart) const
{
	return DodgeableBodyParts.Contains(BodyPart);
}

void UDodgeAbilityComponent::ConsumeDodgeCharge()
{
	if (CurrentDodgeCharges > 0)
	{
		CurrentDodgeCharges--;
	}
}

void UDodgeAbilityComponent::PerformDodgeMovement(AActor* Attacker)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement())
	{
		return;
	}
	
	// Calculate dodge direction (away from attacker)
	FVector DodgeDirection;
	if (Attacker)
	{
		DodgeDirection = GetOwner()->GetActorLocation() - Attacker->GetActorLocation();
	}
	else
	{
		// If no attacker specified, dodge backward
		DodgeDirection = -GetOwner()->GetActorForwardVector();
	}
	
	DodgeDirection.Z = 0; // Keep horizontal
	DodgeDirection.Normalize();
	
	// Apply dodge movement
	FVector DodgeVelocity = DodgeDirection * DodgeSpeed;
	OwnerCharacter->GetCharacterMovement()->Velocity = DodgeVelocity;
	
	// Optionally trigger dodge animation here
}