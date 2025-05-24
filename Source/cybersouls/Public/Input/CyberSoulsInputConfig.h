#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "CyberSoulsInputConfig.generated.h"

/**
 * Input configuration for CyberSouls game
 * Contains shared input actions between characters
 */
UCLASS()
class CYBERSOULS_API UCyberSoulsInputConfig : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // Common actions shared between all characters
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Common Input")
    UInputAction* SwitchCharacterAction;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Common Input")
    UInputAction* RestartAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Input")
    UInputAction* ShowXPAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI Input")
    UInputAction* OpenInventoryAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Input")
    UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Input")
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Input")
    UInputAction* CameraToggleAction;

    // Default character specific actions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Input")
    UInputAction* SlashAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickHack Input")
    UInputAction* QuickHack1Action;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickHack Input")
    UInputAction* QuickHack2Action;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickHack Input")
    UInputAction* QuickHack3Action;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuickHack Input")
    UInputAction* QuickHack4Action;

    // Cyber State character specific actions
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cyber State Input")
    UInputAction* DashAction;
};