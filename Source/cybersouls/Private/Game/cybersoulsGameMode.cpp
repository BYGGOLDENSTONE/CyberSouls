// Copyright Epic Games, Inc. All Rights Reserved.

#include "cybersouls/Public/Game/cybersoulsGameMode.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "UObject/ConstructorHelpers.h"

AcybersoulsGameMode::AcybersoulsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set default HUD class
	HUDClass = ACybersoulsHUD::StaticClass();
}
