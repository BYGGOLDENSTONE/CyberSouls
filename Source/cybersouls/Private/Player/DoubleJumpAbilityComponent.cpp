#include "cybersouls/Public/Player/DoubleJumpAbilityComponent.h"
#include "cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UDoubleJumpAbilityComponent::UDoubleJumpAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentJumpsInAir = 0;
}

void UDoubleJumpAbilityComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        AttributeComponent = OwnerCharacter->FindComponentByClass<UPlayerCyberStateAttributeComponent>();
        OwnerCharacter->MovementModeChangedDelegate.AddDynamic(this, &UDoubleJumpAbilityComponent::OnMovementModeChanged);
    }
}

bool UDoubleJumpAbilityComponent::CanPerformAbility() const
{
    if (!IsValid(OwnerCharacter) || !IsValid(AttributeComponent))
    {
        return false;
    }

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (!Movement)
    {
        return false;
    }

    bool bIsInAir = Movement->IsFalling();
    
    if (!bIsInAir)
    {
        return true;
    }
    
    if (CurrentJumpsInAir >= MaxJumpsInAir)
    {
        return false;
    }

    return AttributeComponent->HasEnoughStamina(StaminaCost);
}

void UDoubleJumpAbilityComponent::PerformAbility()
{
    if (!CanPerformAbility())
    {
        return;
    }

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (!Movement)
    {
        return;
    }

    if (Movement->IsFalling())
    {
        AttributeComponent->UseStamina(StaminaCost);
        CurrentJumpsInAir++;
        
        FVector CurrentVelocity = Movement->Velocity;
        CurrentVelocity.Z = DoubleJumpVelocity;
        Movement->Velocity = CurrentVelocity;
        
        Movement->SetMovementMode(MOVE_Falling);
        
        UGameplayStatics::PlaySoundAtLocation(this, nullptr, OwnerCharacter->GetActorLocation());
    }
    else
    {
        OwnerCharacter->Jump();
    }
}

void UDoubleJumpAbilityComponent::ResetJumpCount()
{
    CurrentJumpsInAir = 0;
}

void UDoubleJumpAbilityComponent::OnMovementModeChanged(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
    if (Character == OwnerCharacter)
    {
        UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
        if (Movement && Movement->IsMovingOnGround())
        {
            ResetJumpCount();
        }
    }
}