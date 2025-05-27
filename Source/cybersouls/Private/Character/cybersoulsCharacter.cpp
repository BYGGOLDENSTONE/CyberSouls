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
#include "cybersouls/Public/Abilities/QuickHackManagerComponent.h"
#include "cybersouls/Public/Abilities/PassiveAbilityComponent.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "cybersouls/Public/Components/TargetingComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "cybersouls/Public/Game/cybersoulsGameMode.h"
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
	QuickHackManager = CreateDefaultSubobject<UQuickHackManagerComponent>(TEXT("QuickHackManager"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
}

void AcybersoulsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	// Bind player death to game mode
	if (PlayerAttributes)
	{
		PlayerAttributes->OnDeath.AddDynamic(this, &AcybersoulsCharacter::HandlePlayerDeath);
	}
	
	// QuickHacks are now managed by QuickHackManager
	
	// Setup targeting component
	if (TargetingComponent && FollowCamera)
	{
		TargetingComponent->SetCameraComponent(FollowCamera);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AcybersoulsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// Add Input Mapping Context when possessed
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				UE_LOG(LogTemp, Warning, TEXT("cybersoulsCharacter: Adding DefaultMappingContext on possession"));
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AcybersoulsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Input mapping context is now added in PossessedBy to prevent conflicts
	
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

		// Character Switch
		if (SwitchCharacterAction)
		{
			UE_LOG(LogTemp, Warning, TEXT("DEFAULT CHARACTER: Binding SwitchCharacterAction"));
			EnhancedInputComponent->BindAction(SwitchCharacterAction, ETriggerEvent::Started, this, &AcybersoulsCharacter::OnSwitchCharacter);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DEFAULT CHARACTER: SwitchCharacterAction is NULL!"));
		}
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

void AcybersoulsCharacter::OnSwitchCharacter()
{
	UE_LOG(LogTemp, Warning, TEXT("DEFAULT CHARACTER: OnSwitchCharacter() called!"));
	
	if (ACyberSoulsPlayerController* CyberController = Cast<ACyberSoulsPlayerController>(GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("DEFAULT CHARACTER: Found CyberSoulsPlayerController, calling SwitchCharacter()"));
		CyberController->SwitchCharacter();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DEFAULT CHARACTER: Controller is NOT CyberSoulsPlayerController! Type: %s"), 
			GetController() ? *GetController()->GetClass()->GetName() : TEXT("NULL"));
	}
}

void AcybersoulsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Crosshair targeting is now handled by TargetingComponent
}

// UpdateCrosshairTarget removed - now handled by TargetingComponent



void AcybersoulsCharacter::UseQuickHack1()
{
	if (QuickHackManager)
	{
		QuickHackManager->ActivateQuickHack(1);
	}
}

void AcybersoulsCharacter::UseQuickHack2()
{
	if (QuickHackManager)
	{
		QuickHackManager->ActivateQuickHack(2);
	}
}

void AcybersoulsCharacter::UseQuickHack3()
{
	if (QuickHackManager)
	{
		QuickHackManager->ActivateQuickHack(3);
	}
}

void AcybersoulsCharacter::UseQuickHack4()
{
	if (QuickHackManager)
	{
		QuickHackManager->ActivateQuickHack(4);
	}
}

// DetermineCrosshairBodyPart removed - now handled by TargetingComponent

AActor* AcybersoulsCharacter::GetCrosshairTarget() const
{
	return TargetingComponent ? TargetingComponent->GetCurrentTarget() : nullptr;
}

EBodyPart AcybersoulsCharacter::GetCrosshairBodyPart() const
{
	return TargetingComponent ? TargetingComponent->GetTargetBodyPart() : EBodyPart::None;
}

void AcybersoulsCharacter::HandlePlayerDeath()
{
	// Notify game mode of player death
	AcybersoulsGameMode* GameMode = Cast<AcybersoulsGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnPlayerDeath();
	}
	
	// Disable player input
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		DisableInput(PC);
		
		// Keep only restart input enabled
		ACyberSoulsPlayerController* CyberPC = Cast<ACyberSoulsPlayerController>(PC);
		if (CyberPC)
		{
			// Re-enable just the restart action
			EnableInput(PC);
		}
	}
}