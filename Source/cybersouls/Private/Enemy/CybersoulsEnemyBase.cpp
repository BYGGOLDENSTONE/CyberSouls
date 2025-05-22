// CybersoulsEnemyBase.cpp
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/AI/PhysicalEnemyAIController.h"
#include "cybersouls/Public/AI/HackingEnemyAIController.h"
#include "cybersouls/Public/Game/cybersoulsGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
	// Set default AI controller based on enemy type
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// Enable movement replication for multiplayer
	SetReplicateMovement(true);
	
	// Configure movement component
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	}
}

void ACybersoulsEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeEnemy();
	
	// Death event binding happens in derived classes
	
	// Start behavior timers
	StartBehaviorTimers();
	
	// Register with game mode for quest tracking
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (IsValid(GameMode))
	{
		if (AcybersoulsGameMode* CybersoulsGameMode = Cast<AcybersoulsGameMode>(GameMode))
		{
			CybersoulsGameMode->RegisterEnemy(this);
			UE_LOG(LogTemp, Warning, TEXT("Enemy %s registered with GameMode"), *GetName());
		}
	}
	
	// Log AI controller setup
	if (GetController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy %s has AI Controller: %s"), 
			*GetName(), *GetController()->GetClass()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy %s has NO AI Controller!"), *GetName());
	}
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

	// Create shattered pieces instead of ragdoll
	CreateShatteredPieces();

	// Destroy actor after 3 seconds (extra time for dramatic effect)
	GetWorldTimerManager().SetTimer(DeathTimerHandle, [this]()
	{
		Destroy();
	}, 3.0f, false);
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

void ACybersoulsEnemyBase::CreateShatteredPieces()
{
	if (!GetMesh())
	{
		return;
	}

	// Hide the original mesh
	GetMesh()->SetVisibility(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Get the bounds and location of the enemy
	FVector MeshLocation = GetMesh()->GetComponentLocation();
	FVector MeshBounds = GetMesh()->Bounds.BoxExtent;
	
	// For shatter effect, we'll create multiple skeletal mesh components
	// Each will represent a piece of the shattered enemy
	USkeletalMeshComponent* OriginalMesh = GetMesh();
	
	// Create shattered pieces as multiple skeletal mesh components
	for (int32 i = 0; i < ShatterPieceCount; i++)
	{
		// Create skeletal mesh component for each piece
		USkeletalMeshComponent* Piece = NewObject<USkeletalMeshComponent>(this, *FString::Printf(TEXT("ShatterPiece_%d"), i));
		Piece->SetSkeletalMesh(OriginalMesh->GetSkeletalMeshAsset());
		Piece->SetAnimInstanceClass(nullptr); // No animation
		Piece->RegisterComponent();
		Piece->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		
		// Copy materials
		for (int32 MatIdx = 0; MatIdx < OriginalMesh->GetNumMaterials(); MatIdx++)
		{
			Piece->SetMaterial(MatIdx, OriginalMesh->GetMaterial(MatIdx));
		}
		
		// Scale down each piece for shatter effect - smaller pieces for more pieces
		float RandomScale = FMath::RandRange(0.15f, 0.35f);
		Piece->SetWorldScale3D(FVector(RandomScale));
		
		// Position pieces randomly around the original location
		FVector RandomOffset = FVector(
			FMath::RandRange(-MeshBounds.X * 0.3f, MeshBounds.X * 0.3f),
			FMath::RandRange(-MeshBounds.Y * 0.3f, MeshBounds.Y * 0.3f),
			FMath::RandRange(-MeshBounds.Z * 0.2f, MeshBounds.Z * 0.5f)
		);
		Piece->SetWorldLocation(MeshLocation + RandomOffset);
		
		// Random rotation for variety
		Piece->SetWorldRotation(FRotator(
			FMath::RandRange(0.0f, 360.0f),
			FMath::RandRange(0.0f, 360.0f),
			FMath::RandRange(0.0f, 360.0f)
		));
		
		// Enable physics
		Piece->SetSimulatePhysics(true);
		Piece->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Piece->SetCollisionResponseToAllChannels(ECR_Block);
		Piece->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		
		// Calculate explosion direction from center
		FVector ExplosionDirection = RandomOffset.GetSafeNormal();
		if (ExplosionDirection.IsNearlyZero())
		{
			// Random direction if piece spawned at center
			ExplosionDirection = FVector(
				FMath::RandRange(-1.0f, 1.0f),
				FMath::RandRange(-1.0f, 1.0f),
				FMath::RandRange(0.5f, 1.0f)
			).GetSafeNormal();
		}
		
		// Apply explosive force
		float ExplosionForce = FMath::RandRange(300.0f, 400.0f);
		FVector Impulse = ExplosionDirection * ExplosionForce;
		Impulse.Z += FMath::RandRange(200.0f, 300.0f); // Add upward force
		
		Piece->AddImpulse(Impulse * 10.0f, NAME_None, true); // Mass multiplier
		
		// Add random angular velocity for tumbling effect
		FVector AngularImpulse = FVector(
			FMath::RandRange(-6.0f, 6.0f),
			FMath::RandRange(-6.0f, 6.0f),
			FMath::RandRange(-6.0f, 6.0f)
		);
		Piece->AddAngularImpulseInRadians(AngularImpulse * 200.0f, NAME_None, true);
		
		// Optionally hide some bones to create different piece shapes
		// This would require bone hiding setup in the skeletal mesh
		
		// Store reference (but since we're using USkeletalMeshComponent, we can't store in TArray<UStaticMeshComponent*>
		// So pieces will be cleaned up when actor is destroyed
	}
	
	// Optional: Add particle effects at shatter point
	// UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShatterParticleSystem, MeshLocation);
}