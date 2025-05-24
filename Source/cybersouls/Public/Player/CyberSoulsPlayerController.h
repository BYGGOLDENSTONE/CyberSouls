#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "CyberSoulsPlayerController.generated.h"

class AcybersoulsCharacter;
class APlayerCyberState;
class UInputMappingContext;
class UCyberSoulsInputConfig;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSwitched, APawn*, NewCharacter);

/**
 * Main player controller for Cybersouls
 * 
 * Manages character switching between the default combat character
 * and the CyberState mobility character. Handles input contexts
 * and maintains both character instances throughout the game.
 */
UCLASS()
class CYBERSOULS_API ACyberSoulsPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ACyberSoulsPlayerController();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Switching")
    TSubclassOf<AcybersoulsCharacter> DefaultCharacterClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Switching")
    TSubclassOf<APlayerCyberState> CyberStateCharacterClass;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCharacterSwitched OnCharacterSwitched;

    UFUNCTION(BlueprintCallable, Category = "Character Switching")
    void SwitchCharacter();

    UFUNCTION(BlueprintCallable, Category = "Character Switching")
    bool IsUsingCyberState() const { return bIsUsingCyberState; }

    // Enhanced Input
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* ControllerMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UCyberSoulsInputConfig* InputConfig;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    UPROPERTY()
    AcybersoulsCharacter* DefaultCharacter;

    UPROPERTY()
    APlayerCyberState* CyberStateCharacter;

    bool bIsUsingCyberState;

    void SpawnCharacters();
    void SwitchToDefaultCharacter();
    void SwitchToCyberStateCharacter();
    void TransferCameraSettings(APawn* FromPawn, APawn* ToPawn);
    void StoreCharacterState(APawn* CharacterPawn);
    void RestoreCharacterState(APawn* CharacterPawn);
    
    // Character state preservation
    struct FCharacterState
    {
        FVector Location;
        FRotator Rotation;
        FRotator CameraRotation;
        // Add more state data as needed
        bool bIsValid = false;
    };
    
    FCharacterState DefaultCharacterState;
    FCharacterState CyberStateCharacterState;
    
    // Input handling
    void HandleRestartInput();
    void HandleShowXPInput();
    void HandleOpenInventoryInput();
};