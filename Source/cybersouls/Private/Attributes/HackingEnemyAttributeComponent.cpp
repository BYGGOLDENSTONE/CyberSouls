// HackingEnemyAttributeComponent.cpp
#include "cybersouls/Public/Attributes/HackingEnemyAttributeComponent.h"

UHackingEnemyAttributeComponent::UHackingEnemyAttributeComponent()
{
	// Hacking enemies have 100 HP
	Integrity = 100.0f;
	MaxIntegrity = 100.0f;
	
	// Default hacking stats
	HackRate = 2.0f;
	HackRange = 1500.0f;
	bHasQuickHacks = false;
	QuickHackChance = 0.3f;
}