#include "cybersouls/Public/Character/PlayerCyberState.h"
#include "cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h"
#include "cybersouls/Public/Player/DoubleJumpAbilityComponent.h"
#include "cybersouls/Public/Player/DashAbilityComponent.h"
#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

APlayerCyberState::APlayerCyberState()
{
    PrimaryActorTick.bCanEverTick = false;

    AttributeComponent = CreateDefaultSubobject<UPlayerCyberStateAttributeComponent>(TEXT("AttributeComponent"));
    DoubleJumpComponent = CreateDefaultSubobject<UDoubleJumpAbilityComponent>(TEXT("DoubleJumpComponent"));
    DashComponent = CreateDefaultSubobject<UDashAbilityComponent>(TEXT("DashComponent"));

    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 400.0f;
    SpringArm->bUsePawnControlRotation = true;

    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    ConfigureEnhancedMovement();
}

void APlayerCyberState::BeginPlay()
{
    Super::BeginPlay();
    
    // Input mapping context will be added when possessed, not in BeginPlay
    // This prevents context conflicts during character switching
}

void APlayerCyberState::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    // Add Input Mapping Context when possessed
    if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (CyberStateMappingContext)
            {
                UE_LOG(LogTemp, Warning, TEXT("PlayerCyberState: Adding CyberStateMappingContext on possession"));
                Subsystem->AddMappingContext(CyberStateMappingContext, 0);
            }
        }
    }
    
    // Reset double jump count when possessed
    if (DoubleJumpComponent)
    {
        DoubleJumpComponent->ResetJumpCount();
    }
    
    // Reset dash charges when possessed
    if (DashComponent)
    {
        DashComponent->ResetCharges();
    }
}

void APlayerCyberState::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping - using double jump component
        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCyberState::TryJump);
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        }

        // Moving
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCyberState::Move);
        }

        // Looking
        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCyberState::Look);
        }

        // Dash - unique to cyber state character
        if (DashAction)
        {
            EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerCyberState::TryDash);
        }

        // Camera Toggle
        if (CameraToggleAction)
        {
            EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Triggered, this, &APlayerCyberState::ToggleCameraView);
        }

        // Character Switch
        if (SwitchCharacterAction)
        {
            EnhancedInputComponent->BindAction(SwitchCharacterAction, ETriggerEvent::Triggered, this, &APlayerCyberState::OnSwitchCharacter);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system."), *GetNameSafe(this));
    }
}

void APlayerCyberState::ConfigureEnhancedMovement()
{
    UCharacterMovementComponent* Movement = GetCharacterMovement();
    if (Movement)
    {
        Movement->MaxWalkSpeed = EnhancedMoveSpeed;
        Movement->AirControl = AirControlMultiplier;
        Movement->GravityScale = GravityScale;
        Movement->JumpZVelocity = 800.0f;
        Movement->FallingLateralFriction = 2.0f;
        Movement->bOrientRotationToMovement = true;
        Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    }
}

void APlayerCyberState::Move(const FInputActionValue& Value)
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

void APlayerCyberState::Look(const FInputActionValue& Value)
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

void APlayerCyberState::TryJump()
{
    if (DoubleJumpComponent && DoubleJumpComponent->CanPerformAbility())
    {
        DoubleJumpComponent->PerformAbility();
    }
}

void APlayerCyberState::TryDash()
{
    if (DashComponent && DashComponent->CanPerformAbility())
    {
        DashComponent->PerformAbility();
    }
}

void APlayerCyberState::OnSwitchCharacter()
{
    if (ACyberSoulsPlayerController* CyberController = Cast<ACyberSoulsPlayerController>(GetController()))
    {
        CyberController->SwitchCharacter();
    }
}

void APlayerCyberState::ToggleCameraView()
{
    if (USpringArmComponent* SpringArm = FindComponentByClass<USpringArmComponent>())
    {
        if (bIsThirdPersonView)
        {
            // Switch to first person view
            SpringArm->TargetArmLength = 0.0f;
            SpringArm->bUsePawnControlRotation = true;
            SpringArm->SocketOffset = FVector(0.0f, 0.0f, 60.0f); // Eye level height
            bIsThirdPersonView = false;
            
            UE_LOG(LogTemp, Warning, TEXT("Switched to First Person View"));
        }
        else
        {
            // Switch to third person view
            SpringArm->TargetArmLength = 400.0f;
            SpringArm->bUsePawnControlRotation = true;
            SpringArm->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
            bIsThirdPersonView = true;
            
            UE_LOG(LogTemp, Warning, TEXT("Switched to Third Person View"));
        }
    }
}