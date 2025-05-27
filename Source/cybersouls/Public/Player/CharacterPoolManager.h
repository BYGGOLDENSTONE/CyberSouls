#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterPoolManager.generated.h"

class ACharacter;
class APlayerController;

/**
 * Manages a pool of characters for efficient switching
 * Instead of destroying and recreating characters, this hides/shows them
 */
UCLASS()
class CYBERSOULS_API ACharacterPoolManager : public AActor
{
    GENERATED_BODY()

public:
    ACharacterPoolManager();

    /**
     * Initialize the character pool with the given classes
     * @param DefaultClass The default character class
     * @param CyberStateClass The cyber state character class
     */
    UFUNCTION(BlueprintCallable, Category = "Character Pool")
    void InitializePool(TSubclassOf<ACharacter> DefaultClass, TSubclassOf<ACharacter> CyberStateClass);

    /**
     * Get a character from the pool
     * @param bCyberState True to get cyber state character, false for default
     * @return The requested character, or nullptr if not available
     */
    UFUNCTION(BlueprintCallable, Category = "Character Pool")
    ACharacter* GetCharacter(bool bCyberState);

    /**
     * Return a character to the pool (hide it)
     * @param Character The character to return
     */
    UFUNCTION(BlueprintCallable, Category = "Character Pool")
    void ReturnCharacter(ACharacter* Character);

    /**
     * Get the inactive character
     * @param bCyberState True to get cyber state character, false for default
     * @return The inactive character
     */
    UFUNCTION(BlueprintCallable, Category = "Character Pool")
    ACharacter* GetInactiveCharacter(bool bCyberState) const;

    /**
     * Swap which character is active
     */
    UFUNCTION(BlueprintCallable, Category = "Character Pool")
    void SwapActiveCharacter();

    /**
     * Clean up the pool
     */
    void CleanupPool();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY()
    ACharacter* DefaultCharacter;

    UPROPERTY()
    ACharacter* CyberStateCharacter;

    UPROPERTY()
    TSubclassOf<ACharacter> DefaultCharacterClass;

    UPROPERTY()
    TSubclassOf<ACharacter> CyberStateCharacterClass;

    bool bDefaultCharacterActive;

    /**
     * Hide a character without destroying it
     * @param Character The character to hide
     */
    void HideCharacter(ACharacter* Character);

    /**
     * Show a previously hidden character
     * @param Character The character to show
     */
    void ShowCharacter(ACharacter* Character);

    /**
     * Spawn a character for the pool
     * @param CharacterClass The class to spawn
     * @return The spawned character
     */
    ACharacter* SpawnPooledCharacter(TSubclassOf<ACharacter> CharacterClass);
};