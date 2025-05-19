// HackingEnemyAttributeComponent.cpp
#include "cybersouls/Public/Attributes/HackingEnemyAttributeComponent.h"

UHackingEnemyAttributeComponent::UHackingEnemyAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Hacking enemies have 1 HP and die in one hit
	Integrity = 1.0f;
	MaxIntegrity = 1.0f;
	
	// Default hacking stats
	HackRate = 2.0f;
	HackRange = 1500.0f;
	bHasQuickHacks = false;
	QuickHackChance = 0.3f;
}

void UHackingEnemyAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	Integrity = MaxIntegrity;
}

void UHackingEnemyAttributeComponent::TakeDamage(float Damage)
{
	float OldIntegrity = Integrity;
	Integrity = FMath::Clamp(Integrity - Damage, 0.0f, MaxIntegrity);
	
	if (Integrity < OldIntegrity)
	{
		OnDamaged.Broadcast(Damage);
		CheckDeath();
	}
}

bool UHackingEnemyAttributeComponent::IsAlive() const
{
	return Integrity > 0.0f;
}

void UHackingEnemyAttributeComponent::CheckDeath()
{
	if (Integrity <= 0.0f)
	{
		OnDeath.Broadcast();
	}
}