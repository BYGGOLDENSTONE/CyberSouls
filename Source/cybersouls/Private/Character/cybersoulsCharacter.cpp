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
#include "cybersouls/Public/Abilities/SlashAbilityComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "cybersouls/Public/Combat/TargetLockComponent.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "TimerManager.h"

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
	SlashAbility = CreateDefaultSubobject<USlashAbilityComponent>(TEXT("SlashAbility"));
	InterruptProtocolAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("InterruptProtocolAbility"));
	SystemFreezeAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("SystemFreezeAbility"));
	FirewallAbility = CreateDefaultSubobject<UQuickHackComponent>(TEXT("FirewallAbility"));
	TargetLockComponent = CreateDefaultSubobject<UTargetLockComponent>(TEXT("TargetLockComponent"));
}

void AcybersoulsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Delay initialization to ensure all components are ready
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		// Set enemy class for target lock
		if (TargetLockComponent && IsValid(TargetLockComponent))
		{
			TargetLockComponent->EnemyClass = ACybersoulsEnemyBase::StaticClass();
			
			// TEMPORARILY COMMENT OUT TO DEBUG
			// TODO: Re-enable after blueprint is properly set up
			/*
			// Bind target lock events with additional safety checks
			if (!TargetLockComponent->OnTargetLocked.IsBound())
			{
				TargetLockComponent->OnTargetLocked.AddDynamic(this, &AcybersoulsCharacter::OnTargetLocked);
			}
			
			if (!TargetLockComponent->OnTargetUnlocked.IsBound())
			{
				TargetLockComponent->OnTargetUnlocked.AddDynamic(this, &AcybersoulsCharacter::OnTargetUnlocked);
			}
			*/
			
			UE_LOG(LogTemp, Warning, TEXT("TargetLockComponent successfully initialized"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("TargetLockComponent is null or invalid in BeginPlay!"));
		}
	});
	
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

		// Target Lock
		EnhancedInputComponent->BindAction(TargetLockAction, ETriggerEvent::Started, this, &AcybersoulsCharacter::ToggleTargetLock);

		// Target Change
		EnhancedInputComponent->BindAction(TargetChangeLeftAction, ETriggerEvent::Started, this, &AcybersoulsCharacter::ChangeTargetLeft);
		EnhancedInputComponent->BindAction(TargetChangeRightAction, ETriggerEvent::Started, this, &AcybersoulsCharacter::ChangeTargetRight);

		// Slash
		EnhancedInputComponent->BindAction(SlashAction, ETriggerEvent::Started, this, &AcybersoulsCharacter::PerformSlash);

		// QuickHacks
		EnhancedInputComponent->BindAction(QuickHack1Action, ETriggerEvent::Started, this, &AcybersoulsCharacter::UseQuickHack1);
		EnhancedInputComponent->BindAction(QuickHack2Action, ETriggerEvent::Started, this, &AcybersoulsCharacter::UseQuickHack2);
		EnhancedInputComponent->BindAction(QuickHack3Action, ETriggerEvent::Started, this, &AcybersoulsCharacter::UseQuickHack3);
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
		
		// Update target part based on mouse movement when locked
		UpdateTargetPart(Value);
	}
}

void AcybersoulsCharacter::ToggleTargetLock()
{
	if (TargetLockComponent)
	{
		TargetLockComponent->ToggleTargetLock();
	}
}

void AcybersoulsCharacter::ChangeTargetLeft()
{
	if (TargetLockComponent)
	{
		TargetLockComponent->ChangeTargetLeft();
	}
}

void AcybersoulsCharacter::ChangeTargetRight()
{
	if (TargetLockComponent)
	{
		TargetLockComponent->ChangeTargetRight();
	}
}

void AcybersoulsCharacter::PerformSlash()
{
	if (!SlashAbility)
	{
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

	SlashAbility->ActivateAbility();
}

void AcybersoulsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Update camera lock
	if (bIsCameraLocked)
	{
		UpdateCameraLock(DeltaTime);
	}
}

void AcybersoulsCharacter::UpdateCameraLock(float DeltaTime)
{
	if (!LockedTarget || !CameraBoom || !FollowCamera)
	{
		return;
	}
	
	// Get the body part location
	FVector TargetLocation = GetBodyPartLocation(LockedTarget, LockedBodyPart);
	TargetLocation.Z += CameraLockOffsetZ; // Add offset to look slightly above
	
	// Calculate desired camera position
	FVector CameraLocation = FollowCamera->GetComponentLocation();
	FVector DirectionToTarget = (TargetLocation - CameraLocation).GetSafeNormal();
	
	// Calculate the desired look at rotation
	FRotator DesiredRotation = DirectionToTarget.Rotation();
	
	// Get current rotation
	FRotator CurrentRotation = GetController()->GetControlRotation();
	
	// Interpolate to desired rotation
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, CameraLockInterpSpeed);
	
	// Apply rotation to controller
	GetController()->SetControlRotation(NewRotation);
	
	// Optionally adjust spring arm length based on distance
	float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetLocation);
	float DesiredArmLength = FMath::Clamp(DistanceToTarget * 0.8f, 200.0f, CameraLockDistance);
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, DesiredArmLength, DeltaTime, CameraLockInterpSpeed);
}

void AcybersoulsCharacter::OnTargetLocked(AActor* Target, EBodyPart BodyPart)
{
	bIsCameraLocked = true;
	LockedTarget = Target;
	LockedBodyPart = BodyPart;
	
	// Disable normal camera input when locked
	if (CameraBoom)
	{
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritYaw = false;
		CameraBoom->bInheritRoll = false;
	}
}

void AcybersoulsCharacter::OnTargetUnlocked()
{
	bIsCameraLocked = false;
	LockedTarget = nullptr;
	
	// Re-enable normal camera input
	if (CameraBoom)
	{
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bInheritPitch = true;
		CameraBoom->bInheritYaw = true;
		CameraBoom->bInheritRoll = false;
		
		// Reset arm length
		CameraBoom->TargetArmLength = 400.0f;
	}
}

FVector AcybersoulsCharacter::GetBodyPartLocation(AActor* Target, EBodyPart BodyPart) const
{
	if (!Target)
	{
		return FVector::ZeroVector;
	}
	
	// Try to get body part component from enemy
	if (ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(Target))
	{
		UBodyPartComponent* BodyPartComp = nullptr;
		
		switch (BodyPart)
		{
			case EBodyPart::UpperBody:
				BodyPartComp = Enemy->UpperBodyPart;
				break;
			case EBodyPart::LeftLeg:
				BodyPartComp = Enemy->LeftLegPart;
				break;
			case EBodyPart::RightLeg:
				BodyPartComp = Enemy->RightLegPart;
				break;
		}
		
		if (BodyPartComp)
		{
			return BodyPartComp->GetComponentLocation();
		}
	}
	
	// Fallback to estimated positions based on target actor
	FVector BaseLocation = Target->GetActorLocation();
	
	switch (BodyPart)
	{
		case EBodyPart::UpperBody:
			return BaseLocation + FVector(0, 0, 120.0f);
		case EBodyPart::LeftLeg:
			return BaseLocation + FVector(0, -30.0f, 40.0f);
		case EBodyPart::RightLeg:
			return BaseLocation + FVector(0, 30.0f, 40.0f);
		default:
			return BaseLocation;
	}
}

void AcybersoulsCharacter::UseQuickHack1()
{
	// Interrupt Protocol
	if (InterruptProtocolAbility)
	{
		AActor* Target = TargetLockComponent ? TargetLockComponent->GetCurrentTarget() : nullptr;
		InterruptProtocolAbility->StartQuickHack(Target);
	}
}

void AcybersoulsCharacter::UseQuickHack2()
{
	// System Freeze
	if (SystemFreezeAbility)
	{
		AActor* Target = TargetLockComponent ? TargetLockComponent->GetCurrentTarget() : nullptr;
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

void AcybersoulsCharacter::UpdateTargetPart(const FInputActionValue& Value)
{
	if (!TargetLockComponent || !TargetLockComponent->IsLocked())
	{
		return;
	}

	FVector2D MouseDelta = Value.Get<FVector2D>();
	
	// Determine target part based on mouse movement
	EBodyPart NewBodyPart = LockedBodyPart;
	
	if (FMath::Abs(MouseDelta.Y) > FMath::Abs(MouseDelta.X))
	{
		// Vertical movement - target upper body
		NewBodyPart = EBodyPart::UpperBody;
	}
	else if (MouseDelta.X < -0.1f)
	{
		// Left movement - target left leg
		NewBodyPart = EBodyPart::LeftLeg;
	}
	else if (MouseDelta.X > 0.1f)
	{
		// Right movement - target right leg
		NewBodyPart = EBodyPart::RightLeg;
	}
	
	// Change body part if different
	if (NewBodyPart != LockedBodyPart)
	{
		TargetLockComponent->ChangeBodyPart(NewBodyPart);
		LockedBodyPart = NewBodyPart; // Update our local state
	}
}