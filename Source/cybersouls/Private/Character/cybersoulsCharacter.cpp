// Copyright Epic Games, Inc. All Rights Reserved.

#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "cybersouls/Public/Abilities/SlashAbilityComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AcybersoulsCharacter

AcybersoulsCharacter::AcybersoulsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Create game system components
	PlayerAttributes = CreateDefaultSubobject<UPlayerAttributeComponent>(TEXT("PlayerAttributes"));
	PlayerProgression = CreateDefaultSubobject<UPlayerProgressionComponent>(TEXT("PlayerProgression"));
	SlashAbility = CreateDefaultSubobject<USlashAbilityComponent>(TEXT("SlashAbility"));
	InterruptProtocolAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("InterruptProtocolAbility"));
	SystemFreezeAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("SystemFreezeAbility"));
	FirewallAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("FirewallAbility"));
	KillAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("KillAbility"));
}

void AcybersoulsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	// Initialize QuickHack types
	if (InterruptProtocolAbility)
	{
		InterruptProtocolAbility->QuickHackType = EQuickHackType::InterruptProtocol;
	}
	
	if (SystemFreezeAbility)
	{
		SystemFreezeAbility->QuickHackType = EQuickHackType::SystemFreeze;
		SystemFreezeAbility->bIsSelfTargeted = false;
	}
	
	if (FirewallAbility)
	{
		FirewallAbility->QuickHackType = EQuickHackType::Firewall;
		FirewallAbility->bIsSelfTargeted = true;
	}
	
	if (KillAbility)
	{
		KillAbility->QuickHackType = EQuickHackType::Kill;
		KillAbility->bIsSelfTargeted = false;
		KillAbility->CastTime = 0.1f; // Very fast cast time for instant kill
		KillAbility->Cooldown = 1.0f; // Short cooldown
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AcybersoulsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AcybersoulsCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AcybersoulsCharacter::Look);


		// Slash
		EnhancedInputComponent->BindAction(SlashAction, ETriggerEvent::Started, this, &AcybersoulsCharacter::PerformSlash);

		// QuickHacks
		EnhancedInputComponent->BindAction(QuickHack1Action, ETriggerEvent::Started, this, &AcybersoulsCharacter::UseQuickHack1);
		EnhancedInputComponent->BindAction(QuickHack2Action, ETriggerEvent::Started, this, &AcybersoulsCharacter::UseQuickHack2);
		EnhancedInputComponent->BindAction(QuickHack3Action, ETriggerEvent::Started, this, &AcybersoulsCharacter::UseQuickHack3);
		EnhancedInputComponent->BindAction(QuickHack4Action, ETriggerEvent::Started, this, &AcybersoulsCharacter::UseQuickHack4);

		// Camera Toggle
		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &AcybersoulsCharacter::ToggleCameraView);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AcybersoulsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AcybersoulsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void AcybersoulsCharacter::PerformSlash()
{
	UE_LOG(LogTemp, Warning, TEXT("PerformSlash called!"));
	
	if (!SlashAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("SlashAbility is null!"));
		return;
	}

	// Check if player abilities are disabled
	if (PlayerAttributes && !PlayerAttributes->bCanUseAbilities)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player abilities are disabled!"));
		return;
	}

	// Check if player is immobilized
	if (PlayerAttributes && PlayerAttributes->bIsImmobilized)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is immobilized!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Activating slash ability!"));
	SlashAbility->ActivateAbility();
}

void AcybersoulsCharacter::ToggleCameraView()
{
	bIsFirstPersonView = !bIsFirstPersonView;
	
	if (bIsFirstPersonView)
	{
		// Switch to first person view
		CameraBoom->TargetArmLength = 0.0f;
		CameraBoom->SetRelativeLocation(FVector(0, 0, 60)); // Move camera to head level
		
		// Hide character mesh in first person
		GetMesh()->SetOwnerNoSee(true);
		
		UE_LOG(LogTemp, Warning, TEXT("Switched to First Person View"));
	}
	else
	{
		// Switch to third person view
		CameraBoom->TargetArmLength = 400.0f;
		CameraBoom->SetRelativeLocation(FVector(0, 0, 0)); // Reset camera position
		
		// Show character mesh in third person
		GetMesh()->SetOwnerNoSee(false);
		
		UE_LOG(LogTemp, Warning, TEXT("Switched to Third Person View"));
	}
}

void AcybersoulsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Update crosshair targeting
	UpdateCrosshairTarget();
}

void AcybersoulsCharacter::UpdateCrosshairTarget()
{
	// Get camera and controller
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !FollowCamera)
	{
		CrosshairTarget = nullptr;
		return;
	}

	// Get screen center for crosshair position
	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D CrosshairLocation(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

	// Convert screen position to world ray
	FVector WorldLocation, WorldDirection;
	if (PC->DeprojectScreenPositionToWorld(CrosshairLocation.X, CrosshairLocation.Y, WorldLocation, WorldDirection))
	{
		// Perform line trace from camera
		FVector TraceStart = FollowCamera->GetComponentLocation();
		FVector TraceEnd = TraceStart + (WorldDirection * CrosshairRange);

		FHitResult HitResult;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Pawn, TraceParams))
		{
			// Check if hit actor is an enemy
			if (ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(HitResult.GetActor()))
			{
				CrosshairTarget = Enemy;
				
				// Determine body part based on hit location
				DetermineCrosshairBodyPart(HitResult);
			}
			else
			{
				CrosshairTarget = nullptr;
			}
		}
		else
		{
			CrosshairTarget = nullptr;
		}
	}
}



void AcybersoulsCharacter::UseQuickHack1()
{
	// Interrupt Protocol - Can be used without lock if enemies are casting
	if (InterruptProtocolAbility)
	{
		AActor* Target = nullptr;
		
		// First check if we have a crosshair target
		Target = GetCrosshairTarget();
		
		// If no crosshair target, check for any enemy casting QuickHack
		if (!Target)
		{
			// Find all enemies in the world
			TArray<AActor*> FoundEnemies;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACybersoulsEnemyBase::StaticClass(), FoundEnemies);
			
			for (AActor* Enemy : FoundEnemies)
			{
				TArray<UQuickHackComponent*> QuickHacks;
				Enemy->GetComponents<UQuickHackComponent>(QuickHacks);
				
				for (UQuickHackComponent* QH : QuickHacks)
				{
					if (QH->IsQuickHackActive())
					{
						Target = Enemy;
						UE_LOG(LogTemp, Warning, TEXT("Found enemy casting QuickHack - using as InterruptProtocol target"));
						break;
					}
				}
				
				if (Target) break;
			}
		}
		
		if (Target)
		{
			InterruptProtocolAbility->StartQuickHack(Target);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InterruptProtocol: No target found (need crosshair target or enemy casting QuickHack)"));
		}
	}
}

void AcybersoulsCharacter::UseQuickHack2()
{
	// System Freeze
	if (SystemFreezeAbility)
	{
		AActor* Target = GetCrosshairTarget();
		SystemFreezeAbility->StartQuickHack(Target);
	}
}

void AcybersoulsCharacter::UseQuickHack3()
{
	// Firewall
	if (FirewallAbility)
	{
		FirewallAbility->StartQuickHack(this);
	}
}

void AcybersoulsCharacter::UseQuickHack4()
{
	// Kill
	if (KillAbility)
	{
		AActor* Target = GetCrosshairTarget();
		KillAbility->StartQuickHack(Target);
	}
}

void AcybersoulsCharacter::DetermineCrosshairBodyPart(const FHitResult& HitResult)
{
	if (!HitResult.GetActor())
	{
		return;
	}

	ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(HitResult.GetActor());
	if (!Enemy)
	{
		return;
	}

	// Get hit location and actor center
	FVector HitLocation = HitResult.Location;
	FVector ActorLocation = Enemy->GetActorLocation();
	FVector Origin, BoxExtent;
	Enemy->GetActorBounds(false, Origin, BoxExtent);

	// Determine body part based on hit height relative to actor
	float RelativeHeight = HitLocation.Z - ActorLocation.Z;
	
	if (RelativeHeight > BoxExtent.Z * 0.3f)
	{
		CrosshairBodyPart = EBodyPart::UpperBody;
	}
	else
	{
		// Determine left or right leg based on Y position
		float RelativeY = HitLocation.Y - ActorLocation.Y;
		if (RelativeY > 0)
		{
			CrosshairBodyPart = EBodyPart::RightLeg;
		}
		else
		{
			CrosshairBodyPart = EBodyPart::LeftLeg;
		}
	}
}

AActor* AcybersoulsCharacter::GetCrosshairTarget() const
{
	return CrosshairTarget;
}

EBodyPart AcybersoulsCharacter::GetCrosshairBodyPart() const
{
	return CrosshairBodyPart;
}