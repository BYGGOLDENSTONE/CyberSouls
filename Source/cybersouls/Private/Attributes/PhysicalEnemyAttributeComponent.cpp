// PhysicalEnemyAttributeComponent.cpp
#include "cybersouls/Public/Attributes/PhysicalEnemyAttributeComponent.h"

UPhysicalEnemyAttributeComponent::UPhysicalEnemyAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Physical enemies have 1 HP and die in one hit
	Integrity = 1.0f;
	MaxIntegrity = 1.0f;
	
	// Default combat stats
	AttackDamage = 1.0f;
	AttackSpeed = 2.0f;
	AttackRange = 300.0f;
}

void UPhysicalEnemyAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	Integrity = MaxIntegrity;
}

void UPhysicalEnemyAttributeComponent::TakeDamage(float Damage)
{
	float OldIntegrity = Integrity;
	Integrity = FMath::Clamp(Integrity - Damage, 0.0f, MaxIntegrity);
	
	if (Integrity < OldIntegrity)
	{
		OnDamaged.Broadcast(Damage);
		CheckDeath();
	}
}

bool UPhysicalEnemyAttributeComponent::IsAlive() const
{
	return Integrity > 0.0f;
}

void UPhysicalEnemyAttributeComponent::CheckDeath()
{
	if (Integrity <= 0.0f)
	{
		OnDeath.Broadcast();
	}
}