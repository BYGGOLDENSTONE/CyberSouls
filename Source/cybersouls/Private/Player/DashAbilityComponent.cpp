#include "cybersouls/Public/Player/DashAbilityComponent.h"
#include "cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

UDashAbilityComponent::UDashAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bIsDashing = false;
    DashTimeRemaining = 0.0f;
    CooldownTimeRemaining = 0.0f;
}

void UDashAbilityComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        AttributeComponent = OwnerCharacter->FindComponentByClass<UPlayerCyberStateAttributeComponent>();
    }
}

void UDashAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (CooldownTimeRemaining > 0.0f)
    {
        CooldownTimeRemaining -= DeltaTime;
    }

    if (bIsDashing)
    {
        UpdateDash(DeltaTime);
    }
}

bool UDashAbilityComponent::CanPerformAbility() const
{
    if (!IsValid(OwnerCharacter) || !IsValid(AttributeComponent))
    {
        return false;
    }

    if (bIsDashing || CooldownTimeRemaining > 0.0f)
    {
        return false;
    }

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (!Movement)
    {
        return false;
    }

    if (!bCanDashInAir && Movement->IsFalling())
    {
        return false;
    }

    return AttributeComponent->HasEnoughStamina(StaminaCost);
}

void UDashAbilityComponent::PerformAbility()
{
    if (!CanPerformAbility())
    {
        return;
    }

    AttributeComponent->UseStamina(StaminaCost);
    StartDash();
}

void UDashAbilityComponent::StartDash()
{
    bIsDashing = true;
    DashTimeRemaining = DashDuration;
    
    FVector InputDirection = FVector::ZeroVector;
    
    // Get movement input direction from character's last movement input
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (Movement && !Movement->GetLastInputVector().IsNearlyZero())
    {
        InputDirection = Movement->GetLastInputVector().GetSafeNormal();
    }
    
    if (InputDirection.IsNearlyZero())
    {
        DashDirection = OwnerCharacter->GetActorForwardVector();
    }
    else
    {
        DashDirection = InputDirection;
    }
    
    if (Movement)
    {
        OriginalVelocity = Movement->Velocity;
        Movement->BrakingFrictionFactor = 0.0f;
        Movement->SetMovementMode(MOVE_Flying);
    }
    
    UGameplayStatics::PlaySoundAtLocation(this, nullptr, OwnerCharacter->GetActorLocation());
}

void UDashAbilityComponent::UpdateDash(float DeltaTime)
{
    DashTimeRemaining -= DeltaTime;
    
    if (DashTimeRemaining <= 0.0f)
    {
        EndDash();
        return;
    }
    
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (Movement)
    {
        float DashSpeed = DashDistance / DashDuration;
        Movement->Velocity = DashDirection * DashSpeed;
    }
}

void UDashAbilityComponent::EndDash()
{
    bIsDashing = false;
    CooldownTimeRemaining = DashCooldown;
    
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (Movement)
    {
        Movement->SetMovementMode(MOVE_Falling);
        Movement->BrakingFrictionFactor = 2.0f;
        
        Movement->Velocity = DashDirection * (DashDistance / DashDuration) * 0.3f;
        Movement->Velocity.Z = FMath::Max(Movement->Velocity.Z, -500.0f);
    }
}