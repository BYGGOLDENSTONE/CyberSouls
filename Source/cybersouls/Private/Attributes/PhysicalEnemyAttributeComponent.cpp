// PhysicalEnemyAttributeComponent.cpp
#include "cybersouls/Public/Attributes/PhysicalEnemyAttributeComponent.h"

UPhysicalEnemyAttributeComponent::UPhysicalEnemyAttributeComponent()
{
	// Physical enemies have 100 HP
	Integrity = 100.0f;
	MaxIntegrity = 100.0f;
	
	// Default combat stats
	AttackDamage = 1.0f;
	AttackSpeed = 2.0f;
	AttackRange = 300.0f;
}