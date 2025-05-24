#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCyberState.generated.h"

class UPlayerCyberStateAttributeComponent;
class UDoubleJumpAbilityComponent;
class UDashAbilityComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class CYBERSOULS_API APlayerCyberState : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCyberState();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPlayerCyberStateAttributeComponent* AttributeComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UDoubleJumpAbilityComponent* DoubleJumpComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UDashAbilityComponent* DashComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float EnhancedMoveSpeed = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AirControlMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float GravityScale = 0.8f;

    // Enhanced Input - Same as cybersoulsCharacter
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* CyberStateMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* DashAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* CameraToggleAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* SwitchCharacterAction;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    // Enhanced Input Actions
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void TryJump();
    void TryDash();
    void OnSwitchCharacter();
    
    void ConfigureEnhancedMovement();
};