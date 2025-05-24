# CODING_STANDARDS.md

## Metadata
- **Category**: CORE
- **Type**: Coding Standards
- **Version**: 1.0
- **Last Updated**: 2025-01-24
- **Dependencies**: Unreal Engine C++ Standards

## Purpose
Establishes consistent coding conventions and best practices for the Cybersouls project.

## Naming Conventions

### Classes
- **Prefix**: Use Unreal Engine standard prefixes
  - `A` for Actors (e.g., `ABasicEnemy`)
  - `U` for UObjects (e.g., `USlashAbilityComponent`)
  - `F` for structs (e.g., `FQuickHackData`)
  - `E` for enums (e.g., `EEnemyType`)
  - `I` for interfaces (e.g., `ITargetable`)

### Variables
- **Member Variables**: PascalCase (e.g., `MaxHackProgress`)
- **Local Variables**: camelCase (e.g., `currentTarget`)
- **Boolean Prefix**: `b` (e.g., `bIsBlocking`)
- **Pointers**: Descriptive names (e.g., `PlayerController`, not `PC`)

### Functions
- **Public Functions**: PascalCase (e.g., `PerformSlash()`)
- **Private Functions**: PascalCase with context (e.g., `UpdateCrosshairVisuals()`)
- **Getters**: `Get` prefix (e.g., `GetCurrentIntegrity()`)
- **Setters**: `Set` prefix (e.g., `SetStamina()`)
- **Callbacks**: `On` prefix (e.g., `OnEnemyDeath()`)

### Constants
- **Global Constants**: UPPER_SNAKE_CASE (e.g., `MAX_HACK_PROGRESS`)
- **Class Constants**: PascalCase with `k` prefix (e.g., `kDefaultStamina`)

## File Organization

### Header Files (.h)
```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// Other includes in alphabetical order
#include "SlashAbilityComponent.generated.h"

// Forward declarations
class AcybersoulsCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API USlashAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Constructors
    USlashAbilityComponent();

    // Public functions
    void PerformSlash();

protected:
    // Protected overrides
    virtual void BeginPlay() override;

    // Protected functions
    void ProcessSlashHit();

private:
    // Private variables
    UPROPERTY()
    float SlashRange = 300.0f;

    // Private functions
    bool ValidateTarget(AActor* Target);
};
```

### Source Files (.cpp)
```cpp
#include "Abilities/SlashAbilityComponent.h"
#include "Character/cybersoulsCharacter.h"
// Other includes in dependency order
```

## Code Patterns

### Null Checking
```cpp
// Always use IsValid() for UObjects
if (IsValid(TargetActor))
{
    // Process target
}

// Use nullptr checks for raw pointers
if (RawPointer != nullptr)
{
    // Use pointer
}
```

### Component Access
```cpp
// Always cache component references
if (USlashAbilityComponent* SlashComp = GetOwner()->FindComponentByClass<USlashAbilityComponent>())
{
    SlashComp->PerformSlash();
}
```

### Error Handling
```cpp
// Use UE_LOG for debugging
UE_LOG(LogCybersouls, Warning, TEXT("Failed to find target: %s"), *GetName());

// Use ensure() for development checks
ensure(IsValid(PlayerController));

// Use check() for critical failures
check(GetWorld() != nullptr);
```

## Blueprint Integration

### UPROPERTY Specifiers
```cpp
// Exposed to Blueprint with category
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
float Damage = 100.0f;

// Read-only in Blueprint
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
int32 CurrentLevel = 1;

// Event delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKilled, AActor*, KilledEnemy);
```

### UFUNCTION Specifiers
```cpp
// Blueprint callable
UFUNCTION(BlueprintCallable, Category = "Abilities")
void ActivateAbility();

// Blueprint implementable event
UFUNCTION(BlueprintImplementableEvent, Category = "Events")
void OnAbilityActivated();

// Blueprint native event
UFUNCTION(BlueprintNativeEvent, Category = "Combat")
void TakeDamage(float Amount);
```

## Memory Management

### Smart Pointers
```cpp
// Use TWeakObjectPtr for cross-references
TWeakObjectPtr<AcybersoulsCharacter> OwningCharacter;

// Use UPROPERTY() for automatic garbage collection
UPROPERTY()
UAbilityComponent* AbilityComponent;
```

### Object Creation
```cpp
// Use NewObject for UObjects
UAbilityComponent* NewComp = NewObject<UAbilityComponent>(this);

// Use SpawnActor for Actors
AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnParams);
```

## Comments and Documentation

### Function Documentation
```cpp
/**
 * Performs a slash attack against the current crosshair target
 * @param bForceAttack - If true, bypasses cooldown checks
 * @return True if slash was successful
 */
bool PerformSlash(bool bForceAttack = false);
```

### Inline Comments
```cpp
// Calculate damage falloff based on distance
float DamageFalloff = FMath::Clamp(1.0f - (Distance / MaxRange), 0.0f, 1.0f);

// TODO: Implement particle effects for slash
// HACK: Temporary fix for targeting issue - remove after proper fix
// NOTE: This value must match the one in BP_SlashEffect
```

## Version Control

### Commit Messages
- Format: `[Category] Brief description`
- Examples:
  - `[Combat] Add slash ability cooldown`
  - `[AI] Fix enemy search behavior`
  - `[UI] Update HUD stamina display`

### Branch Naming
- Feature: `feature/ability-name`
- Bugfix: `fix/issue-description`
- Refactor: `refactor/system-name`