// CybersoulsBuffNetrunner.cpp
#include "cybersouls/Public/Enemy/CybersoulsBuffNetrunner.h"
#include "cybersouls/Public/Attributes/HackingEnemyAttributeComponent.h"
#include "cybersouls/Public/Abilities/HackAbilityComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"

ACybersoulsBuffNetrunner::ACybersoulsBuffNetrunner()
{
	// Set enemy type
	EnemyType = EEnemyType::BuffNetrunner;
	
	// Create components
	EnemyAttributes = CreateDefaultSubobject<UHackingEnemyAttributeComponent>(TEXT("EnemyAttributes"));
	HackAbility = CreateDefaultSubobject<UHackAbilityComponent>(TEXT("HackAbility"));
	FirewallAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("FirewallAbility"));
	
	// Enable tick
	PrimaryActorTick.bCanEverTick = true;
}

void ACybersoulsBuffNetrunner::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind death event
	if (EnemyAttributes)
	{
		EnemyAttributes->OnDeath.AddDynamic(this, &ACybersoulsBuffNetrunner::OnDeath);
	}
	
	// Activate hacking ability
	if (HackAbility)
	{
		HackAbility->ActivateAbility();
	}
}

void ACybersoulsBuffNetrunner::InitializeEnemy()
{
	Super::InitializeEnemy();
	
	// BuffNetrunner specific initialization
	if (UHackingEnemyAttributeComponent* HackingAttributes = Cast<UHackingEnemyAttributeComponent>(EnemyAttributes))
	{
		HackingAttributes->Integrity = 100.0f; // Dies in one hit
		HackingAttributes->MaxIntegrity = 100.0f;
		HackingAttributes->bHasQuickHacks = true; // This enemy has QuickHack abilities
		HackingAttributes->QuickHackChance = 0.3f; // 30% chance to use QuickHack when available
		HackingAttributes->HackRate = 3.0f; // BuffNetrunner: 3 hackprogress per second
		HackingAttributes->HackRange = 1500.0f;
	}
	
	if (FirewallAbility)
	{
		FirewallAbility->QuickHackType = EQuickHackType::Firewall;
		FirewallAbility->bIsSelfTargeted = true;
		FirewallAbility->CastTime = 6.0f;
		FirewallAbility->Cooldown = 12.0f;
		FirewallAbility->EffectDuration = 3.0f;  // Shields against QuickHacks for 3s
		FirewallAbility->Range = 0.0f; // Self-targeted
	}
}

void ACybersoulsBuffNetrunner::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	
	// QuickHack logic is now handled by HackingEnemyAIController
}

