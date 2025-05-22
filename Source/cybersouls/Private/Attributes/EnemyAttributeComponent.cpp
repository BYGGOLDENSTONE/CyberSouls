// EnemyAttributeComponent.cpp
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "Engine/Engine.h"

UEnemyAttributeComponent::UEnemyAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Enemies have 100 HP
	Integrity = 100.0f;
	MaxIntegrity = 100.0f;
}

void UEnemyAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemyAttributeComponent::TakeDamage(float Damage)
{
	if (!IsAlive())
	{
		return;
	}
	
	float OldIntegrity = Integrity;
	Integrity = FMath::Clamp(Integrity - Damage, 0.0f, MaxIntegrity);
	
	if (OldIntegrity != Integrity)
	{
		OnDamaged.Broadcast(Damage);
		UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage. Integrity: %f/%f"), 
			Damage, Integrity, MaxIntegrity);
	}
	
	CheckDeath();
}

bool UEnemyAttributeComponent::IsAlive() const
{
	return Integrity > 0.0f;
}

void UEnemyAttributeComponent::CheckDeath()
{
	if (!IsAlive())
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy destroyed!"));
		OnDeath.Broadcast();
	}
}