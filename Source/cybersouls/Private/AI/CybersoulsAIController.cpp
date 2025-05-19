// CybersoulsAIController.cpp
#include "cybersouls/Public/AI/CybersoulsAIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ACybersoulsAIController::ACybersoulsAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACybersoulsAIController::BeginPlay()
{
	Super::BeginPlay();
	
	// Get reference to player character
	PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ACybersoulsAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACybersoulsAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter && GetPawn())
	{
		MoveToPlayer();
	}
}

void ACybersoulsAIController::MoveToPlayer()
{
	if (!PlayerCharacter || !GetPawn())
	{
		return;
	}

	// Simple movement towards player
	MoveToActor(PlayerCharacter, AcceptanceRadius);
}