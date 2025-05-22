// CybersoulsBasicEnemy.cpp
#include "cybersouls/Public/Enemy/CybersoulsBasicEnemy.h"
#include "cybersouls/Public/Attributes/PhysicalEnemyAttributeComponent.h"
#include "cybersouls/Public/Abilities/AttackAbilityComponent.h"
#include "cybersouls/Public/AI/PhysicalEnemyAIController.h"

ACybersoulsBasicEnemy::ACybersoulsBasicEnemy()
{
	// Set enemy type
	EnemyType = EEnemyType::Basic;
	
	// Set AI controller class for physical enemy
	AIControllerClass = APhysicalEnemyAIController::StaticClass();
	
	// Create components
	EnemyAttributes = CreateDefaultSubobject<UPhysicalEnemyAttributeComponent>(TEXT("EnemyAttributes"));
	AttackAbility = CreateDefaultSubobject<UAttackAbilityComponent>(TEXT("AttackAbility"));
}

void ACybersoulsBasicEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind death event
	if (EnemyAttributes)
	{
		EnemyAttributes->OnDeath.AddDynamic(this, &ACybersoulsBasicEnemy::OnEnemyDeath);
	}
}

void ACybersoulsBasicEnemy::InitializeEnemy()
{
	Super::InitializeEnemy();
	
	// Basic enemy specific initialization
	if (EnemyAttributes)
	{
		EnemyAttributes->Integrity = 100.0f; // 100 HP
		EnemyAttributes->MaxIntegrity = 100.0f;
	}
	
	if (AttackAbility)
	{
		AttackAbility->AttackDamage = 10.0f;    // Deal 10 damage to player integrity
		AttackAbility->AttackCooldown = 2.0f;  // 2 second cooldown between attacks
		AttackAbility->AttackRange = 200.0f;   // Attack range
	}
}

void ACybersoulsBasicEnemy::OnEnemyDeath()
{
	// Use base class death handling
	OnDeath();
}