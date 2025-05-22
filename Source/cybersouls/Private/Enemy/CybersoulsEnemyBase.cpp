// CybersoulsEnemyBase.cpp
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/AI/PhysicalEnemyAIController.h"
#include "cybersouls/Public/AI/HackingEnemyAIController.h"
#include "cybersouls/Public/Game/cybersoulsGameMode.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ACybersoulsEnemyBase::ACybersoulsEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Components are now created in derived classes instead

	// Create body part components
	UpperBodyPart = CreateDefaultSubobject<UBodyPartComponent>(TEXT("UpperBodyPart"));
	UpperBodyPart->SetupAttachment(GetMesh());
	UpperBodyPart->BodyPartType = EBodyPart::UpperBody;

	LeftLegPart = CreateDefaultSubobject<UBodyPartComponent>(TEXT("LeftLegPart"));
	LeftLegPart->SetupAttachment(GetMesh());
	LeftLegPart->BodyPartType = EBodyPart::LeftLeg;

	RightLegPart = CreateDefaultSubobject<UBodyPartComponent>(TEXT("RightLegPart"));
	RightLegPart->SetupAttachment(GetMesh());
	RightLegPart->BodyPartType = EBodyPart::RightLeg;
}

void ACybersoulsEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeEnemy();
	
	// Death event binding happens in derived classes
	
	// Start behavior timers
	StartBehaviorTimers();
}

void ACybersoulsEnemyBase::InitializeEnemy()
{
	SetupBodyParts();
	SetupAttributesForType();
	SetupAbilitiesForType();
}

void ACybersoulsEnemyBase::SetupBodyParts()
{
	// Set body part positions based on mesh
	if (GetMesh())
	{
		// Upper body at chest height
		UpperBodyPart->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
		
		// Legs at lower positions
		LeftLegPart->SetRelativeLocation(FVector(0.0f, -30.0f, 40.0f));
		RightLegPart->SetRelativeLocation(FVector(0.0f, 30.0f, 40.0f));
	}
}

void ACybersoulsEnemyBase::SetupAttributesForType()
{
	// Attributes are now set up in derived classes
}

void ACybersoulsEnemyBase::SetupAbilitiesForType()
{
	// Abilities are now set up in derived classes
}

bool ACybersoulsEnemyBase::TryBlock(EBodyPart TargetedPart)
{
	// Override in derived classes
	return false;
}

bool ACybersoulsEnemyBase::TryDodge(EBodyPart TargetedPart)
{
	// Override in derived classes
	return false;
}

void ACybersoulsEnemyBase::PerformAttack(AActor* Target)
{
	// Override in derived classes
}

void ACybersoulsEnemyBase::PerformHack(AActor* Target)
{
	// Override in derived classes
}

void ACybersoulsEnemyBase::OnReceivedDamage(float Damage, EBodyPart HitPart)
{
	// Override in derived classes
}

void ACybersoulsEnemyBase::OnDeath()
{
	if (bIsDead)
	{
		return; // Already dead
	}

	bIsDead = true;

	// Notify game mode of death for quest tracking
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (IsValid(GameMode))
	{
		if (AcybersoulsGameMode* CybersoulsGameMode = Cast<AcybersoulsGameMode>(GameMode))
		{
			CybersoulsGameMode->OnEnemyDeath(this);
		}
	}

	// Stop all timers
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	GetWorldTimerManager().ClearTimer(HackTimerHandle);
	GetWorldTimerManager().ClearTimer(DeathTimerHandle);

	// Start death sequence (ragdoll, then destroy after 2 seconds)
	StartDeathSequence();
}

void ACybersoulsEnemyBase::StartDeathSequence()
{
	// Stop all movement
	if (Controller)
	{
		Controller->StopMovement();
	}

	// Disable collision with player (keeps physics collision)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Enable ragdoll physics
	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionResponseToAllChannels(ECR_Block);
	}

	// Destroy actor after 2 seconds
	GetWorldTimerManager().SetTimer(DeathTimerHandle, [this]()
	{
		Destroy();
	}, 2.0f, false);
}

TSubclassOf<AController> ACybersoulsEnemyBase::GetDefaultControllerClass() const
{
	// Use different AI controllers based on enemy type
	switch (EnemyType)
	{
		case EEnemyType::Basic:
		case EEnemyType::Block:
		case EEnemyType::Dodge:
			return APhysicalEnemyAIController::StaticClass();
			
		case EEnemyType::Netrunner:
		case EEnemyType::BuffNetrunner:
		case EEnemyType::DebuffNetrunner:
			return AHackingEnemyAIController::StaticClass();
			
		default:
			return nullptr;
	}
}

void ACybersoulsEnemyBase::StartBehaviorTimers()
{
	// Override in derived classes
}

void ACybersoulsEnemyBase::OnAttackTimer()
{
	// Override in derived classes
}

void ACybersoulsEnemyBase::OnHackTimer()
{
	// Override in derived classes
}

bool ACybersoulsEnemyBase::IsTargetInRange(AActor* Target, float Range) const
{
	if (!Target)
	{
		return false;
	}
	
	float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
	return Distance <= Range;
}