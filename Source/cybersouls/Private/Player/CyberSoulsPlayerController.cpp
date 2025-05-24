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

    FVector InitialLocation = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
    FRotator InitialRotation = GetPawn() ? GetPawn()->GetActorRotation() : FRotator::ZeroRotator;

    UE_LOG(LogTemp, Log, TEXT("PLAYER CONTROLLER: Initial location: %s"), *InitialLocation.ToString());

    if (DefaultCharacterClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Spawning Default Character of class: %s"), *DefaultCharacterClass->GetName());
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
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

    if (CyberStateCharacterClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Spawning CyberState Character of class: %s"), *CyberStateCharacterClass->GetName());
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
        CyberStateCharacter = World->SpawnActor<APlayerCyberState>(CyberStateCharacterClass, InitialLocation + FVector(0, 0, -1000), InitialRotation, SpawnParams);
        if (CyberStateCharacter)
        {
            UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: CyberState Character spawned successfully!"));
            CyberStateCharacter->SetActorHiddenInGame(true);
            CyberStateCharacter->SetActorEnableCollision(false);
            CyberStateCharacter->SetActorTickEnabled(false);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: Failed to spawn CyberState Character!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: CyberStateCharacterClass is NULL! Please set it in the Blueprint."));
    }
    
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: Spawning complete. Default: %s, CyberState: %s"), 
        IsValid(DefaultCharacter) ? TEXT("Valid") : TEXT("NULL"),
        IsValid(CyberStateCharacter) ? TEXT("Valid") : TEXT("NULL"));
}

void ACyberSoulsPlayerController::SwitchCharacter()
{
    UE_LOG(LogTemp, Warning, TEXT("PLAYER CONTROLLER: SwitchCharacter() called!"));
    
    if (!IsValid(DefaultCharacter) || !IsValid(CyberStateCharacter))
    {
        UE_LOG(LogTemp, Error, TEXT("PLAYER CONTROLLER: Characters not valid! Default: %s, CyberState: %s"), 
            IsValid(DefaultCharacter) ? TEXT("Valid") : TEXT("NULL"),
            IsValid(CyberStateCharacter) ? TEXT("Valid") : TEXT("NULL"));
        return;
    }
    
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
    if (!IsValid(DefaultCharacter) || !IsValid(CyberStateCharacter)) return;

    FVector CurrentLocation = CyberStateCharacter->GetActorLocation();
    FRotator CurrentRotation = CyberStateCharacter->GetActorRotation();
    
    TransferCameraSettings(CyberStateCharacter, DefaultCharacter);
    
    UnPossess();
    
    CyberStateCharacter->SetActorLocation(CurrentLocation + FVector(0, 0, -1000));
    CyberStateCharacter->SetActorHiddenInGame(true);
    CyberStateCharacter->SetActorEnableCollision(false);
    CyberStateCharacter->SetActorTickEnabled(false);
    
    DefaultCharacter->SetActorLocation(CurrentLocation);
    DefaultCharacter->SetActorRotation(CurrentRotation);
    DefaultCharacter->SetActorHiddenInGame(false);
    DefaultCharacter->SetActorEnableCollision(true);
    DefaultCharacter->SetActorTickEnabled(true);
    
    Possess(DefaultCharacter);
    
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
    if (!IsValid(DefaultCharacter) || !IsValid(CyberStateCharacter)) return;

    FVector CurrentLocation = DefaultCharacter->GetActorLocation();
    FRotator CurrentRotation = DefaultCharacter->GetActorRotation();
    
    TransferCameraSettings(DefaultCharacter, CyberStateCharacter);
    
    UnPossess();
    
    DefaultCharacter->SetActorLocation(CurrentLocation + FVector(0, 0, -1000));
    DefaultCharacter->SetActorHiddenInGame(true);
    DefaultCharacter->SetActorEnableCollision(false);
    DefaultCharacter->SetActorTickEnabled(false);
    
    CyberStateCharacter->SetActorLocation(CurrentLocation);
    CyberStateCharacter->SetActorRotation(CurrentRotation);
    CyberStateCharacter->SetActorHiddenInGame(false);
    CyberStateCharacter->SetActorEnableCollision(true);
    CyberStateCharacter->SetActorTickEnabled(true);
    
    Possess(CyberStateCharacter);
    
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