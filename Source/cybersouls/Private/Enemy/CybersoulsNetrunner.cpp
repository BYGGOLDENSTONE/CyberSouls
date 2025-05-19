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
		EnemyAttributes->Integrity = 1.0f; // Dies in one hit
		EnemyAttributes->MaxIntegrity = 1.0f;
	}
	
	if (HackAbility)
	{
		HackAbility->HackRate = 2.0f;
		HackAbility->HackRange = 1500.0f;
	}
}

void ACybersoulsNetrunner::OnEnemyDeath()
{
	// Handle death
	Destroy();
}