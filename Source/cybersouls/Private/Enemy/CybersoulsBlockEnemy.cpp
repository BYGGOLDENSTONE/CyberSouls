// CybersoulsBlockEnemy.cpp
#include "cybersouls/Public/Enemy/CybersoulsBlockEnemy.h"
#include "cybersouls/Public/Attributes/PhysicalEnemyAttributeComponent.h"
#include "cybersouls/Public/Abilities/AttackAbilityComponent.h"
#include "cybersouls/Public/Abilities/BlockAbilityComponent.h"

ACybersoulsBlockEnemy::ACybersoulsBlockEnemy()
{
	// Set enemy type
	EnemyType = EEnemyType::Block;
	
	// Create components
	EnemyAttributes = CreateDefaultSubobject<UPhysicalEnemyAttributeComponent>(TEXT("EnemyAttributes"));
	AttackAbility = CreateDefaultSubobject<UAttackAbilityComponent>(TEXT("AttackAbility"));
	BlockAbility = CreateDefaultSubobject<UBlockAbilityComponent>(TEXT("BlockAbility"));
}

void ACybersoulsBlockEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind death event
	if (EnemyAttributes)
	{
		EnemyAttributes->OnDeath.AddDynamic(this, &ACybersoulsBlockEnemy::OnEnemyDeath);
	}
}

void ACybersoulsBlockEnemy::InitializeEnemy()
{
	Super::InitializeEnemy();
	
	// Block enemy specific initialization
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
	
	if (BlockAbility)
	{
		BlockAbility->MaxBlockCharges = 3;
		// BlockableBodyParts already defaults to UpperBody
	}
}

void ACybersoulsBlockEnemy::OnEnemyDeath()
{
	// Use base class death handling
	OnDeath();
}