// CybersoulsDodgeEnemy.cpp
#include "cybersouls/Public/Enemy/CybersoulsDodgeEnemy.h"
#include "cybersouls/Public/Attributes/PhysicalEnemyAttributeComponent.h"
#include "cybersouls/Public/Abilities/AttackAbilityComponent.h"
#include "cybersouls/Public/Abilities/DodgeAbilityComponent.h"
#include "cybersouls/Public/AI/PhysicalEnemyAIController.h"

ACybersoulsDodgeEnemy::ACybersoulsDodgeEnemy()
{
	// Set enemy type
	EnemyType = EEnemyType::Dodge;
	
	// Set AI controller class for physical enemy
	AIControllerClass = APhysicalEnemyAIController::StaticClass();
	
	// Create components
	EnemyAttributes = CreateDefaultSubobject<UPhysicalEnemyAttributeComponent>(TEXT("EnemyAttributes"));
	AttackAbility = CreateDefaultSubobject<UAttackAbilityComponent>(TEXT("AttackAbility"));
	DodgeAbility = CreateDefaultSubobject<UDodgeAbilityComponent>(TEXT("DodgeAbility"));
}

void ACybersoulsDodgeEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind death event
	if (EnemyAttributes)
	{
		EnemyAttributes->OnDeath.AddDynamic(this, &ACybersoulsDodgeEnemy::OnDeath);
	}
}

void ACybersoulsDodgeEnemy::InitializeEnemy()
{
	Super::InitializeEnemy();
	
	// Dodge enemy specific initialization
	if (EnemyAttributes)
	{
		EnemyAttributes->Integrity = 100.0f; // Dies in one hit if not dodged
		EnemyAttributes->MaxIntegrity = 100.0f;
	}
	
	if (AttackAbility)
	{
		AttackAbility->AttackDamage = 10.0f;    // Deal 10 damage to player integrity
		AttackAbility->AttackCooldown = 2.0f;  // 2 second cooldown between attacks
		AttackAbility->AttackRange = 200.0f;   // Attack range
	}
	
	if (DodgeAbility)
	{
		DodgeAbility->MaxDodgeCharges = 3;
		DodgeAbility->DodgeDistance = 300.0f;
		DodgeAbility->DodgeSpeed = 800.0f;
		// DodgeableBodyParts already defaults to legs
	}
}

