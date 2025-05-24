// CybersoulsDebuffNetrunner.cpp
#include "cybersouls/Public/Enemy/CybersoulsDebuffNetrunner.h"
#include "cybersouls/Public/Attributes/HackingEnemyAttributeComponent.h"
#include "cybersouls/Public/Abilities/HackAbilityComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "Kismet/GameplayStatics.h"

ACybersoulsDebuffNetrunner::ACybersoulsDebuffNetrunner()
{
	// Set enemy type
	EnemyType = EEnemyType::DebuffNetrunner;
	
	// Create components
	EnemyAttributes = CreateDefaultSubobject<UHackingEnemyAttributeComponent>(TEXT("EnemyAttributes"));
	HackAbility = CreateDefaultSubobject<UHackAbilityComponent>(TEXT("HackAbility"));
	SystemFreezeAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("SystemFreezeAbility"));
	InterruptProtocolAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("InterruptProtocolAbility"));
	
	// Enable tick
	PrimaryActorTick.bCanEverTick = true;
}

void ACybersoulsDebuffNetrunner::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind death event
	if (EnemyAttributes)
	{
		EnemyAttributes->OnDeath.AddDynamic(this, &ACybersoulsDebuffNetrunner::OnDeath);
	}
	
	// Activate hacking ability
	if (HackAbility)
	{
		HackAbility->ActivateAbility();
	}
}

void ACybersoulsDebuffNetrunner::InitializeEnemy()
{
	Super::InitializeEnemy();
	
	// DebuffNetrunner specific initialization
	if (UHackingEnemyAttributeComponent* HackingAttributes = Cast<UHackingEnemyAttributeComponent>(EnemyAttributes))
	{
		HackingAttributes->Integrity = 100.0f; // Dies in one hit
		HackingAttributes->MaxIntegrity = 100.0f;
		HackingAttributes->bHasQuickHacks = true; // This enemy has QuickHack abilities
		HackingAttributes->QuickHackChance = 0.4f; // 40% chance to use QuickHack when available
		HackingAttributes->HackRate = 2.0f; // DebuffNetrunner: 2 hackprogress per second
		HackingAttributes->HackRange = 1500.0f;
	}
	
	if (SystemFreezeAbility)
	{
		SystemFreezeAbility->QuickHackType = EQuickHackType::SystemFreeze;
		SystemFreezeAbility->bIsSelfTargeted = false;
		SystemFreezeAbility->CastTime = 7.0f;
		SystemFreezeAbility->Cooldown = 14.0f;
		SystemFreezeAbility->EffectDuration = 3.0f;
		SystemFreezeAbility->Range = 1200.0f;
	}
	
	if (InterruptProtocolAbility)
	{
		InterruptProtocolAbility->QuickHackType = EQuickHackType::InterruptProtocol;
		InterruptProtocolAbility->bIsSelfTargeted = false;
		InterruptProtocolAbility->CastTime = 5.0f;
		InterruptProtocolAbility->Cooldown = 8.0f;
		InterruptProtocolAbility->EffectDuration = 0.0f; // Instant effect
		InterruptProtocolAbility->Range = 1200.0f;
	}
}

void ACybersoulsDebuffNetrunner::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
	
	// QuickHack logic is now handled by HackingEnemyAIController
}

