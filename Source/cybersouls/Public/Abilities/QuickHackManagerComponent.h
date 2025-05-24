// QuickHackManagerComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "QuickHackManagerComponent.generated.h"

/**
 * Manages all QuickHack abilities for the player
 * This replaces having multiple QuickHackComponents on the player
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UQuickHackManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UQuickHackManagerComponent();

    // Maximum number of QuickHack slots
    static constexpr int32 MAX_QUICKHACK_SLOTS = 4;

    // Available QuickHacks that the player can use
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickHack")
    TArray<EQuickHackType> AvailableQuickHacks;

    // Currently equipped QuickHacks in slots 1-4
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickHack")
    TArray<EQuickHackType> EquippedQuickHacks;

    // Get the QuickHack type in a specific slot (1-4)
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    EQuickHackType GetQuickHackInSlot(int32 SlotIndex) const;

    // Set a QuickHack in a specific slot (1-4)
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    void SetQuickHackInSlot(int32 SlotIndex, EQuickHackType QuickHackType);

    // Swap QuickHacks between two slots
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    void SwapQuickHackSlots(int32 SlotA, int32 SlotB);

    // Activate QuickHack in a specific slot (1-4)
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    bool ActivateQuickHack(int32 SlotIndex);

    // Check if a QuickHack can be activated
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    bool CanActivateQuickHack(int32 SlotIndex) const;

    // Get cooldown time remaining for a slot
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    float GetCooldownRemaining(int32 SlotIndex) const;

    // Get cast time remaining for a slot
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    float GetCastTimeRemaining(int32 SlotIndex) const;

    // Check if a QuickHack is being cast
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    bool IsQuickHackCasting(int32 SlotIndex) const;

    // Cancel a QuickHack that's being cast
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    void CancelQuickHack(int32 SlotIndex);

    // Check if the player has a specific QuickHack available
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    bool HasQuickHackAvailable(EQuickHackType QuickHackType) const;

    // Add a new QuickHack to available list
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    void UnlockQuickHack(EQuickHackType QuickHackType);

    // Get all available QuickHacks
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    TArray<EQuickHackType> GetAllAvailableQuickHacks() const { return AvailableQuickHacks; }

    // Get all equipped QuickHacks
    UFUNCTION(BlueprintCallable, Category = "QuickHack")
    TArray<EQuickHackType> GetAllEquippedQuickHacks() const { return EquippedQuickHacks; }

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // Internal QuickHack instances for each slot
    UPROPERTY()
    TArray<UQuickHackComponent*> QuickHackInstances;

    // Initialize the component with default QuickHacks
    void InitializeDefaultQuickHacks();

    // Create a QuickHack component instance
    UQuickHackComponent* CreateQuickHackInstance(EQuickHackType Type);

    // Validate slot index
    bool IsValidSlot(int32 SlotIndex) const;
};