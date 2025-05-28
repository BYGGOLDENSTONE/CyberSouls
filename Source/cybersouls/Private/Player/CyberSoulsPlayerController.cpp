#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "cybersouls/Public/Player/CharacterPoolManager.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Character/PlayerCyberState.h"
#include "cybersouls/Public/Input/CyberSoulsInputConfig.h"
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/Game/cybersoulsGameMode.h"
#include "cybersouls/Public/AI/BaseEnemyAIController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

ACyberSoulsPlayerController::ACyberSoulsPlayerController()
{
    bIsUsingCyberState = false;
    CharacterPool = nullptr;
    UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: Constructor called"));
}

void ACyberSoulsPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: BeginPlay called"));
    
    // Set initial input mode based on settings
    if (bEnableMouseInGameplay)
    {
        bShowMouseCursor = true;
        bEnableClickEvents = true;
        bEnableMouseOverEvents = true;
        
        if (bUseGameAndUIInputMode)
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
            InputMode.SetHideCursorDuringCapture(false);
            SetInputMode(InputMode);
        }
        else
        {
            FInputModeGameOnly InputMode;
            SetInputMode(InputMode);
        }
    }
    else
    {
        bShowMouseCursor = false;
        bEnableClickEvents = false;
        bEnableMouseOverEvents = false;
        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
    }
    
    // Add controller input mapping context
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (ControllerMappingContext)
        {
            UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: Adding ControllerMappingContext"));
            Subsystem->AddMappingContext(ControllerMappingContext, 1); // Higher priority than character contexts
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("CYBER SOULS PLAYER CONTROLLER: ControllerMappingContext is NULL!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CYBER SOULS PLAYER CONTROLLER: Failed to get Enhanced Input Subsystem"));
    }
    
    InitializeCharacterPool();
}

void ACyberSoulsPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: SetupInputComponent called"));
    
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        if (InputConfig)
        {
            if (InputConfig->SwitchCharacterAction)
            {
                UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: Binding SwitchCharacterAction"));
                EnhancedInputComponent->BindAction(InputConfig->SwitchCharacterAction, ETriggerEvent::Triggered, this, &ACyberSoulsPlayerController::SwitchCharacter);
            }
            
            if (InputConfig->RestartAction)
            {
                UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: Binding RestartAction"));
                EnhancedInputComponent->BindAction(InputConfig->RestartAction, ETriggerEvent::Triggered, this, &ACyberSoulsPlayerController::HandleRestartInput);
            }
            
            if (InputConfig->ShowXPAction)
            {
                UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: Binding ShowXPAction"));
                EnhancedInputComponent->BindAction(InputConfig->ShowXPAction, ETriggerEvent::Triggered, this, &ACyberSoulsPlayerController::HandleShowXPInput);
            }
            
            if (InputConfig->OpenInventoryAction)
            {
                UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: Binding OpenInventoryAction"));
                EnhancedInputComponent->BindAction(InputConfig->OpenInventoryAction, ETriggerEvent::Triggered, this, &ACyberSoulsPlayerController::HandleOpenInventoryInput);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("CYBER SOULS PLAYER CONTROLLER: InputConfig is NULL!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CYBER SOULS PLAYER CONTROLLER: Failed to get Enhanced Input Component"));
    }
}

void ACyberSoulsPlayerController::InitializeCharacterPool()
{
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: InitializeCharacterPool() called"));
    
    UWorld* World = GetWorld();
    if (!World) 
    {
        UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: World is NULL!"));
        return;
    }

    // Create the character pool manager
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    CharacterPool = World->SpawnActor<ACharacterPoolManager>(ACharacterPoolManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    
    if (!CharacterPool)
    {
        UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: Failed to create CharacterPoolManager!"));
        return;
    }
    
    // Initialize the pool with our character classes
    CharacterPool->InitializePool(DefaultCharacterClass, CyberStateCharacterClass);
    
    // Get and possess the default character
    if (ACharacter* DefaultChar = CharacterPool->GetCharacter(false))
    {
        UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Got Default Character from pool"));
        
        // Get player start location
        FVector InitialLocation = FVector::ZeroVector;
        FRotator InitialRotation = FRotator::ZeroRotator;
        
        if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass()))
        {
            InitialLocation = PlayerStart->GetActorLocation();
            InitialRotation = PlayerStart->GetActorRotation();
        }
        
        DefaultChar->SetActorLocation(InitialLocation);
        DefaultChar->SetActorRotation(InitialRotation);
        
        Possess(DefaultChar);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: Failed to get Default Character from pool!"));
    }
    
    // Initialize state structs
    DefaultCharacterState.bIsValid = false;
    CyberStateCharacterState.bIsValid = false;
    
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Character pool initialization complete"));
}

void ACyberSoulsPlayerController::SwitchCharacter()
{
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: SwitchCharacter() called!"));
    
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Current state: %s"), 
        bIsUsingCyberState ? TEXT("CyberState") : TEXT("Default"));
    
    if (bIsUsingCyberState)
    {
        SwitchToDefaultCharacter();
    }
    else
    {
        SwitchToCyberStateCharacter();
    }
}

void ACyberSoulsPlayerController::SwitchToDefaultCharacter()
{
    if (!CharacterPool)
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToDefaultCharacter: CharacterPool is NULL"));
        return;
    }
    
    APawn* CurrentPawn = GetPawn();
    if (!CurrentPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToDefaultCharacter: No current pawn"));
        return;
    }
    
    // Store current state
    StoreCharacterState(CurrentPawn);
    FVector CharacterLocation = CurrentPawn->GetActorLocation();
    FRotator CharacterRotation = CurrentPawn->GetActorRotation();
    
    // Clear all input mapping contexts before switching
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->ClearAllMappings();
        
        // Re-add controller mapping context with higher priority
        if (ControllerMappingContext)
        {
            Subsystem->AddMappingContext(ControllerMappingContext, 1);
        }
    }
    
    // Unpossess current character
    UnPossess();
    
    // Return cyber state character to pool
    CharacterPool->ReturnCharacter(Cast<ACharacter>(CurrentPawn));
    
    // Get default character from pool
    if (ACharacter* DefaultChar = CharacterPool->GetCharacter(false))
    {
        DefaultChar->SetActorLocation(CharacterLocation);
        DefaultChar->SetActorRotation(CharacterRotation);
        RestoreCharacterState(DefaultChar);
        Possess(DefaultChar);
        
        bIsUsingCyberState = false;
        OnCharacterSwitched.Broadcast(DefaultChar);
        
        // Update all AI controllers to track the new player character
        UpdateAllAIControllers();
        
        // Show HUD notification
        if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(GetHUD()))
        {
            CyberHUD->ShowCharacterSwitchNotification(TEXT("▰▰ SWITCHED TO DEFAULT MODE ▰▰"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToDefaultCharacter: Failed to get default character from pool"));
    }
}

void ACyberSoulsPlayerController::SwitchToCyberStateCharacter()
{
    if (!CharacterPool)
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToCyberStateCharacter: CharacterPool is NULL"));
        return;
    }
    
    APawn* CurrentPawn = GetPawn();
    if (!CurrentPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToCyberStateCharacter: No current pawn"));
        return;
    }
    
    // Store current state
    StoreCharacterState(CurrentPawn);
    FVector CharacterLocation = CurrentPawn->GetActorLocation();
    FRotator CharacterRotation = CurrentPawn->GetActorRotation();
    
    // Clear all input mapping contexts before switching
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->ClearAllMappings();
        
        // Re-add controller mapping context with higher priority
        if (ControllerMappingContext)
        {
            Subsystem->AddMappingContext(ControllerMappingContext, 1);
        }
    }
    
    // Unpossess current character
    UnPossess();
    
    // Return default character to pool
    CharacterPool->ReturnCharacter(Cast<ACharacter>(CurrentPawn));
    
    // Get cyber state character from pool
    if (ACharacter* CyberStateChar = CharacterPool->GetCharacter(true))
    {
        CyberStateChar->SetActorLocation(CharacterLocation);
        CyberStateChar->SetActorRotation(CharacterRotation);
        RestoreCharacterState(CyberStateChar);
        Possess(CyberStateChar);
        
        bIsUsingCyberState = true;
        OnCharacterSwitched.Broadcast(CyberStateChar);
        
        // Update all AI controllers to track the new player character
        UpdateAllAIControllers();
        
        // Show HUD notification
        if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(GetHUD()))
        {
            CyberHUD->ShowCharacterSwitchNotification(TEXT("▰▰ SWITCHED TO CYBER STATE ▰▰"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToCyberStateCharacter: Failed to get cyber state character from pool"));
    }
}

void ACyberSoulsPlayerController::TransferCameraSettings(APawn* FromPawn, APawn* ToPawn)
{
    if (!FromPawn || !ToPawn) return;

    USpringArmComponent* FromSpringArm = FromPawn->FindComponentByClass<USpringArmComponent>();
    USpringArmComponent* ToSpringArm = ToPawn->FindComponentByClass<USpringArmComponent>();
    
    if (FromSpringArm && ToSpringArm)
    {
        ToSpringArm->SetWorldRotation(FromSpringArm->GetComponentRotation());
    }
}

void ACyberSoulsPlayerController::StoreCharacterState(APawn* CharacterPawn)
{
    if (!IsValid(CharacterPawn))
    {
        return;
    }
    
    FCharacterState* StateToStore = nullptr;
    
    if (CharacterPool)
    {
        if (CharacterPawn == CharacterPool->GetInactiveCharacter(false))
        {
            StateToStore = &DefaultCharacterState;
        }
        else if (CharacterPawn == CharacterPool->GetInactiveCharacter(true))
        {
            StateToStore = &CyberStateCharacterState;
        }
    }
    
    if (StateToStore)
    {
        StateToStore->Location = CharacterPawn->GetActorLocation();
        StateToStore->Rotation = CharacterPawn->GetActorRotation();
        
        // Store camera rotation
        if (USpringArmComponent* SpringArm = CharacterPawn->FindComponentByClass<USpringArmComponent>())
        {
            StateToStore->CameraRotation = SpringArm->GetComponentRotation();
        }
        
        StateToStore->bIsValid = true;
        
        UE_LOG(LogTemp, Warning, TEXT("Stored character state: Location %s, Rotation %s"), 
            *StateToStore->Location.ToString(), *StateToStore->Rotation.ToString());
    }
}

void ACyberSoulsPlayerController::RestoreCharacterState(APawn* CharacterPawn)
{
    if (!IsValid(CharacterPawn))
    {
        return;
    }
    
    FCharacterState* StateToRestore = nullptr;
    
    if (CharacterPool)
    {
        if (CharacterPawn == CharacterPool->GetInactiveCharacter(false))
        {
            StateToRestore = &DefaultCharacterState;
        }
        else if (CharacterPawn == CharacterPool->GetInactiveCharacter(true))
        {
            StateToRestore = &CyberStateCharacterState;
        }
    }
    
    if (StateToRestore && StateToRestore->bIsValid)
    {
        CharacterPawn->SetActorLocation(StateToRestore->Location);
        CharacterPawn->SetActorRotation(StateToRestore->Rotation);
        
        // Restore camera rotation
        if (USpringArmComponent* SpringArm = CharacterPawn->FindComponentByClass<USpringArmComponent>())
        {
            SpringArm->SetWorldRotation(StateToRestore->CameraRotation);
        }
        
        UE_LOG(LogTemp, Warning, TEXT("Restored character state: Location %s, Rotation %s"), 
            *StateToRestore->Location.ToString(), *StateToRestore->Rotation.ToString());
    }
    else
    {
        // If no stored state, use current possessed character's location or default
        APawn* CurrentPawn = GetPawn();
        if (IsValid(CurrentPawn) && CurrentPawn != CharacterPawn)
        {
            CharacterPawn->SetActorLocation(CurrentPawn->GetActorLocation());
            CharacterPawn->SetActorRotation(CurrentPawn->GetActorRotation());
            TransferCameraSettings(CurrentPawn, CharacterPawn);
        }
        
        UE_LOG(LogTemp, Warning, TEXT("No stored state found, using fallback position"));
    }
}

void ACyberSoulsPlayerController::HandleRestartInput()
{
    if (!IsValid(GetWorld()))
    {
        UE_LOG(LogTemp, Error, TEXT("HandleRestartInput: World is invalid"));
        return;
    }
    
    // Check if we should handle restart
    ACybersoulsHUD* CybersoulsHUD = GetHUD<ACybersoulsHUD>();
    if (!IsValid(CybersoulsHUD))
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleRestartInput: HUD is not valid"));
        return;
    }
    
    AcybersoulsGameMode* GameMode = GetWorld()->GetAuthGameMode<AcybersoulsGameMode>();
    if (!IsValid(GameMode))
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleRestartInput: GameMode is not valid"));
        return;
    }
    
    // Check if player is dead (death screen showing)
    if (CybersoulsHUD->IsShowingDeathScreen())
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleRestartInput: Restarting with XP reset (death)"));
        // Restart with XP reset
        GameMode->RestartLevel(true);
    }
    // Check if quest is complete (play again button showing)
    else if (CybersoulsHUD->IsShowingPlayAgainButton())
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleRestartInput: Restarting without XP reset (quest complete)"));
        // Restart without XP reset
        GameMode->RestartLevel(false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HandleRestartInput: Neither death screen nor play again button is showing"));
    }
}

void ACyberSoulsPlayerController::HandleShowXPInput()
{
    // Tab key now opens inventory (which shows XP)
    if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(GetHUD()))
    {
        CyberHUD->ToggleInventoryDisplay();
        UE_LOG(LogTemp, Warning, TEXT("HandleShowXPInput: Toggled inventory display (Tab key)"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HandleShowXPInput: HUD is not valid"));
    }
}

void ACyberSoulsPlayerController::HandleOpenInventoryInput()
{
    // Note: This function is kept for compatibility but E key functionality has been moved to Tab
    // The actual inventory is now opened via HandleShowXPInput (Tab key)
    UE_LOG(LogTemp, Warning, TEXT("HandleOpenInventoryInput: E key pressed (legacy - use Tab instead)"));
}

void ACyberSoulsPlayerController::UpdateAllAIControllers()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    
    // Find all AI controllers and update their player targets
    for (TActorIterator<ABaseEnemyAIController> ActorIterator(World); ActorIterator; ++ActorIterator)
    {
        ABaseEnemyAIController* AIController = *ActorIterator;
        if (IsValid(AIController))
        {
            AIController->UpdatePlayerTarget();
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Updated all AI controllers to track new player character"));
}