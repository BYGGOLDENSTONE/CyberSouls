#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICombatant.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UCombatant : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that participate in combat
 */
class CYBERSOULS_API ICombatant
{
    GENERATED_BODY()

public:
    /**
     * Get the combat team/faction this combatant belongs to
     * @return The team ID (0 = neutral, 1 = player, 2 = enemy, etc.)
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    int32 GetCombatTeam() const;
    
    /**
     * Check if this combatant is hostile to another
     * @param Other The other combatant to check
     * @return True if hostile
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    bool IsHostileTo(const TScriptInterface<ICombatant>& Other) const;
    
    /**
     * Get the current threat level of this combatant
     * @return Threat level (0 = no threat, higher = more dangerous)
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    float GetThreatLevel() const;
    
    /**
     * Check if this combatant is in combat
     * @return True if currently in combat
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    bool IsInCombat() const;
    
    /**
     * Get the current target of this combatant
     * @return The current target actor, or nullptr if none
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    AActor* GetCurrentTarget() const;
    
    /**
     * Set the current target for this combatant
     * @param NewTarget The new target to focus on
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void SetCurrentTarget(AActor* NewTarget);
};