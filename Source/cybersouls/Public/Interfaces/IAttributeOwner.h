#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAttributeOwner.generated.h"

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
    None        UMETA(DisplayName = "None"),
    Integrity   UMETA(DisplayName = "Integrity"),
    HackProgress UMETA(DisplayName = "Hack Progress"),
    Stamina     UMETA(DisplayName = "Stamina"),
    Health      UMETA(DisplayName = "Health"),
    BlockCharge UMETA(DisplayName = "Block Charge"),
    DodgeCharge UMETA(DisplayName = "Dodge Charge")
};

// Forward declarations
class UAttributeComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UAttributeOwner : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that have attributes
 */
class CYBERSOULS_API IAttributeOwner
{
    GENERATED_BODY()

public:
    /**
     * Get the attribute component
     * @return The attribute component, or nullptr if none
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes")
    UAttributeComponent* GetAttributeComponent() const;
    
    /**
     * Called when an attribute value changes
     * @param AttributeType The type of attribute that changed
     * @param NewValue The new value of the attribute
     * @param OldValue The previous value of the attribute
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes")
    void OnAttributeChanged(EAttributeType AttributeType, float NewValue, float OldValue);
    
    /**
     * Check if the owner is alive
     * @return True if the owner is alive
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes")
    bool IsAlive() const;
    
    /**
     * Get the health percentage (0-1)
     * @return The health percentage
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Attributes")
    float GetHealthPercent() const;
};