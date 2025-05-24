#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Character/PlayerCyberState.h"
#include "cybersouls/Public/Input/CyberSoulsInputConfig.h"
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/Game/cybersoulsGameMode.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerStart.h"

ACyberSoulsPlayerController::ACyberSoulsPlayerController()
{
    bIsUsingCyberState = false;
    UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: Constructor called"));
}

void ACyberSoulsPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    UE_LOG(LogTemp, Warning, TEXT("CYBER SOULS PLAYER CONTROLLER: BeginPlay called"));
    
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
    
    SpawnCharacters();
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

void ACyberSoulsPlayerController::SpawnCharacters()
{
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: SpawnCharacters() called"));
    
    UWorld* World = GetWorld();
    if (!World) 
    {
        UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: World is NULL!"));
        return;
    }

    // Get player start location if available
    FVector InitialLocation = FVector::ZeroVector;
    FRotator InitialRotation = FRotator::ZeroRotator;
    
    if (APawn* ExistingPawn = GetPawn())
    {
        InitialLocation = ExistingPawn->GetActorLocation();
        InitialRotation = ExistingPawn->GetActorRotation();
    }
    else
    {
        // Try to find a player start
        AActor* PlayerStart = UGameplayStatics::GetActorOfClass(World, APlayerStart::StaticClass());
        if (PlayerStart)
        {
            InitialLocation = PlayerStart->GetActorLocation();
            InitialRotation = PlayerStart->GetActorRotation();
        }
    }

    UE_LOG(LogTemp, Log, TEXT("PLAYER CONTROLLER: Initial location: %s"), *InitialLocation.ToString());

    // Only spawn the default character initially
    if (DefaultCharacterClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Spawning Default Character of class: %s"), *DefaultCharacterClass->GetName());
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        DefaultCharacter = World->SpawnActor<AcybersoulsCharacter>(DefaultCharacterClass, InitialLocation, InitialRotation, SpawnParams);
        
        if (DefaultCharacter)
        {
            UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Default Character spawned successfully!"));
            if (!GetPawn())
            {
                UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Possessing Default Character"));
                Possess(DefaultCharacter);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: Failed to spawn Default Character!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: DefaultCharacterClass is NULL! Please set it in the Blueprint."));
    }
    
    // Initialize state structs
    DefaultCharacterState.bIsValid = false;
    CyberStateCharacterState.bIsValid = false;
    
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Spawning complete. Default: %s"), 
        IsValid(DefaultCharacter) ? TEXT("Valid") : TEXT("NULL"));
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
    if (!IsValid(CyberStateCharacter))
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToDefaultCharacter: CyberStateCharacter is not valid"));
        return;
    }
    
    // Store current CyberState character state
    StoreCharacterState(CyberStateCharacter);
    
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
    
    UnPossess();
    
    // Store location before destroying
    FVector CharacterLocation = CyberStateCharacter->GetActorLocation();
    FRotator CharacterRotation = CyberStateCharacter->GetActorRotation();
    
    // Destroy CyberState character
    CyberStateCharacter->Destroy();
    CyberStateCharacter = nullptr;
    
    // Spawn or restore Default character
    if (!IsValid(DefaultCharacter))
    {
        UWorld* World = GetWorld();
        if (World && DefaultCharacterClass)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            DefaultCharacter = World->SpawnActor<AcybersoulsCharacter>(DefaultCharacterClass, CharacterLocation, CharacterRotation, SpawnParams);
        }
    }
    
    if (IsValid(DefaultCharacter))
    {
        RestoreCharacterState(DefaultCharacter);
        Possess(DefaultCharacter);
    }
    
    bIsUsingCyberState = false;
    OnCharacterSwitched.Broadcast(DefaultCharacter);
    
    // Show HUD notification
    if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(GetHUD()))
    {
        CyberHUD->ShowCharacterSwitchNotification(TEXT("▰▰ SWITCHED TO DEFAULT MODE ▰▰"));
    }
}

void ACyberSoulsPlayerController::SwitchToCyberStateCharacter()
{
    if (!IsValid(DefaultCharacter))
    {
        UE_LOG(LogTemp, Error, TEXT("SwitchToCyberStateCharacter: DefaultCharacter is not valid"));
        return;
    }
    
    // Store current Default character state
    StoreCharacterState(DefaultCharacter);
    
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
    
    UnPossess();
    
    // Store location before destroying
    FVector CharacterLocation = DefaultCharacter->GetActorLocation();
    FRotator CharacterRotation = DefaultCharacter->GetActorRotation();
    
    // Destroy Default character
    DefaultCharacter->Destroy();
    DefaultCharacter = nullptr;
    
    // Spawn or restore CyberState character
    if (!IsValid(CyberStateCharacter))
    {
        UWorld* World = GetWorld();
        if (World && CyberStateCharacterClass)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            CyberStateCharacter = World->SpawnActor<APlayerCyberState>(CyberStateCharacterClass, CharacterLocation, CharacterRotation, SpawnParams);
        }
    }
    
    if (IsValid(CyberStateCharacter))
    {
        RestoreCharacterState(CyberStateCharacter);
        Possess(CyberStateCharacter);
    }
    
    bIsUsingCyberState = true;
    OnCharacterSwitched.Broadcast(CyberStateCharacter);
    
    // Show HUD notification
    if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(GetHUD()))
    {
        CyberHUD->ShowCharacterSwitchNotification(TEXT("▰▰ SWITCHED TO CYBER STATE ▰▰"));
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
    
    if (CharacterPawn == DefaultCharacter)
    {
        StateToStore = &DefaultCharacterState;
    }
    else if (CharacterPawn == CyberStateCharacter)
    {
        StateToStore = &CyberStateCharacterState;
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
    
    if (CharacterPawn == DefaultCharacter)
    {
        StateToRestore = &DefaultCharacterState;
    }
    else if (CharacterPawn == CyberStateCharacter)
    {
        StateToRestore = &CyberStateCharacterState;
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
    if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(GetHUD()))
    {
        CyberHUD->TogglePersistentXPDisplay();
        UE_LOG(LogTemp, Warning, TEXT("HandleShowXPInput: Toggled XP display"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HandleShowXPInput: HUD is not valid"));
    }
}

void ACyberSoulsPlayerController::HandleOpenInventoryInput()
{
    if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(GetHUD()))
    {
        CyberHUD->ToggleInventoryDisplay();
        UE_LOG(LogTemp, Warning, TEXT("HandleOpenInventoryInput: Toggled inventory display"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HandleOpenInventoryInput: HUD is not valid"));
    }
}