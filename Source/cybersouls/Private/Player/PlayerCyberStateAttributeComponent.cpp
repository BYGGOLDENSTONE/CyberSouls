#include "cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h"

UPlayerCyberStateAttributeComponent::UPlayerCyberStateAttributeComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCyberStateAttributeComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentStamina = MaxStamina;
    TimeSinceLastStaminaUse = 0.0f;
}

void UPlayerCyberStateAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TimeSinceLastStaminaUse += DeltaTime;
    
    if (TimeSinceLastStaminaUse >= StaminaRegenDelay && CurrentStamina < MaxStamina)
    {
        RegenerateStamina(DeltaTime);
    }
}

void UPlayerCyberStateAttributeComponent::UseStamina(float Amount)
{
    if (Amount <= 0.0f) return;

    float OldStamina = CurrentStamina;
    CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);
    TimeSinceLastStaminaUse = 0.0f;

    if (OldStamina != CurrentStamina)
    {
        OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    }
}

bool UPlayerCyberStateAttributeComponent::HasEnoughStamina(float Amount) const
{
    return CurrentStamina >= Amount;
}

void UPlayerCyberStateAttributeComponent::RegenerateStamina(float DeltaTime)
{
    float OldStamina = CurrentStamina;
    CurrentStamina = FMath::Clamp(CurrentStamina + (StaminaRegenRate * DeltaTime), 0.0f, MaxStamina);

    if (OldStamina != CurrentStamina)
    {
        OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    }
}