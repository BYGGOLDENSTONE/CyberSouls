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
		EnemyAttributes->OnDeath.AddDynamic(this, &ACybersoulsNetrunner::OnDeath);
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
	if (UHackingEnemyAttributeComponent* HackingAttributes = Cast<UHackingEnemyAttributeComponent>(EnemyAttributes))
	{
		HackingAttributes->Integrity = 100.0f; // Dies in one hit
		HackingAttributes->MaxIntegrity = 100.0f;
		HackingAttributes->HackRate = 5.0f; // Netrunner: 5 hackprogress per second
		HackingAttributes->HackRange = 1500.0f;
	}
}

