#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDamageReceiver.generated.h"

// Damage type enumeration
UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical UMETA(DisplayName = "Physical"),
    Hack UMETA(DisplayName = "Hack"),
    Environmental UMETA(DisplayName = "Environmental"),
    TrueDamage UMETA(DisplayName = "TrueDamage") // Ignores resistances
};

UINTERFACE(MinimalAPI, Blueprintable)
class UDamageReceiver : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that can receive damage
 */
class CYBERSOULS_API IDamageReceiver
{
    GENERATED_BODY()

public:
    /**
     * Apply damage to this actor
     * @param Damage The amount of damage to apply
     * @param DamageType The type of damage
     * @param DamageCauser The actor causing the damage
     * @param Instigator The controller responsible for the damage
     * @return The actual damage dealt after resistances/modifiers
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    float TakeDamage(float Damage, EDamageType DamageType, AActor* DamageCauser, AController* Instigator);
    
    /**
     * Check if this actor can take damage
     * @param DamageType The type of damage to check
     * @param DamageCauser The actor that would cause the damage
     * @return True if damage can be applied
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    bool CanTakeDamage(EDamageType DamageType, AActor* DamageCauser) const;
    
    /**
     * Get damage resistance for a specific damage type
     * @param DamageType The type of damage
     * @return Damage resistance multiplier (0 = immune, 1 = full damage)
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    float GetDamageResistance(EDamageType DamageType) const;
    
    /**
     * Called when this actor dies
     * @param Killer The actor that killed this one
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
    void OnDeath(AActor* Killer);
};