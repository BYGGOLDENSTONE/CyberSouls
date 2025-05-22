// CybersoulsNetrunner.cpp
#include "cybersouls/Public/Enemy/CybersoulsNetrunner.h"
#include "cybersouls/Public/Attributes/HackingEnemyAttributeComponent.h"
#include "cybersouls/Public/Abilities/HackAbilityComponent.h"

ACybersoulsNetrunner::ACybersoulsNetrunner()
{
	// Set enemy type
	EnemyType = EEnemyType::Netrunner;
	
	// Create components
	EnemyAttributes = CreateDefaultSubobject<UHackingEnemyAttributeComponent>(TEXT("EnemyAttributes"));
	HackAbility = CreateDefaultSubobject<UHackAbilityComponent>(TEXT("HackAbility"));
}

void ACybersoulsNetrunner::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind death event
	if (EnemyAttributes)
	{
		EnemyAttributes->OnDeath.AddDynamic(this, &ACybersoulsNetrunner::OnEnemyDeath);
	}
	
	// Activate hacking ability
	if (HackAbility)
	{
		HackAbility->ActivateAbility();
	}
}

void ACybersoulsNetrunner::InitializeEnemy()
{
	Super::InitializeEnemy();
	
	// Netrunner specific initialization
	if (EnemyAttributes)
	{
		EnemyAttributes->Integrity = 100.0f; // Dies in one hit
		EnemyAttributes->MaxIntegrity = 100.0f;
	}
	
	if (HackAbility)
	{
		HackAbility->HackRate = 5.0f; // Netrunner: 5 hackprogress per second
		HackAbility->HackRange = 1500.0f;
	}
}

void ACybersoulsNetrunner::OnEnemyDeath()
{
	// Use base class death handling
	OnDeath();
}