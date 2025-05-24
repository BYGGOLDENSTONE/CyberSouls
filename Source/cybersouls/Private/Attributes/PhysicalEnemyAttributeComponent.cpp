// PhysicalEnemyAttributeComponent.cpp
#include "cybersouls/Public/Attributes/PhysicalEnemyAttributeComponent.h"

UPhysicalEnemyAttributeComponent::UPhysicalEnemyAttributeComponent()
{
	// Physical enemies have 100 HP
	Integrity = 100.0f;
	MaxIntegrity = 100.0f;
}