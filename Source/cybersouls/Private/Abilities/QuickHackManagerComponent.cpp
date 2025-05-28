// QuickHackManagerComponent.cpp
#include "cybersouls/Public/Abilities/QuickHackManagerComponent.h"
#include "cybersouls/Public/Abilities/PassiveAbilityComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

UQuickHackManagerComponent::UQuickHackManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize arrays
    EquippedQuickHacks.SetNum(MAX_QUICKHACK_SLOTS);
    QuickHackInstances.SetNum(MAX_QUICKHACK_SLOTS);
    
    // Set default available QuickHacks
    AvailableQuickHacks = {
        EQuickHackType::InterruptProtocol,
        EQuickHackType::SystemFreeze,
        EQuickHackType::Firewall,
        EQuickHackType::Kill
    };
}

void UQuickHackManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeDefaultQuickHacks();
}

void UQuickHackManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Tick all active QuickHack instances
    for (int32 i = 0; i < QuickHackInstances.Num(); i++)
    {
        if (QuickHackInstances[i])
        {
            QuickHackInstances[i]->TickComponent(DeltaTime, TickType, ThisTickFunction);
        }
    }
}

void UQuickHackManagerComponent::InitializeDefaultQuickHacks()
{
    // Set default equipped QuickHacks if not already set
    if (EquippedQuickHacks.Num() >= MAX_QUICKHACK_SLOTS)
    {
        if (EquippedQuickHacks[0] == EQuickHackType::None)
            EquippedQuickHacks[0] = EQuickHackType::InterruptProtocol;
        if (EquippedQuickHacks[1] == EQuickHackType::None)
            EquippedQuickHacks[1] = EQuickHackType::SystemFreeze;
        if (EquippedQuickHacks[2] == EQuickHackType::None)
            EquippedQuickHacks[2] = EQuickHackType::Firewall;
        if (EquippedQuickHacks[3] == EQuickHackType::None)
            EquippedQuickHacks[3] = EQuickHackType::Kill;
    }
    
    // Create QuickHack instances for each slot
    for (int32 i = 0; i < MAX_QUICKHACK_SLOTS; i++)
    {
        if (EquippedQuickHacks[i] != EQuickHackType::None)
        {
            QuickHackInstances[i] = CreateQuickHackInstance(EquippedQuickHacks[i]);
        }
    }
}

UQuickHackComponent* UQuickHackManagerComponent::CreateQuickHackInstance(EQuickHackType Type)
{
    if (Type == EQuickHackType::None)
    {
        return nullptr;
    }
    
    UQuickHackComponent* NewQuickHack = NewObject<UQuickHackComponent>(this);
    if (NewQuickHack)
    {
        NewQuickHack->SetQuickHackType(Type);
        NewQuickHack->RegisterComponent();
    }
    
    return NewQuickHack;
}

EQuickHackType UQuickHackManagerComponent::GetQuickHackInSlot(int32 SlotIndex) const
{
    if (!IsValidSlot(SlotIndex))
    {
        return EQuickHackType::None;
    }
    
    return EquippedQuickHacks[SlotIndex - 1];
}

void UQuickHackManagerComponent::SetQuickHackInSlot(int32 SlotIndex, EQuickHackType QuickHackType)
{
    if (!IsValidSlot(SlotIndex))
    {
        return;
    }
    
    // Check if the player has this QuickHack available
    if (QuickHackType != EQuickHackType::None && !HasQuickHackAvailable(QuickHackType))
    {
        UE_LOG(LogTemp, Warning, TEXT("QuickHackManager: Player doesn't have %s available"), 
            *UEnum::GetValueAsString(QuickHackType));
        return;
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    
    // Remove old QuickHack instance
    if (QuickHackInstances[ArrayIndex])
    {
        QuickHackInstances[ArrayIndex]->DestroyComponent();
        QuickHackInstances[ArrayIndex] = nullptr;
    }
    
    // Set new QuickHack
    EquippedQuickHacks[ArrayIndex] = QuickHackType;
    
    // Create new instance if not None
    if (QuickHackType != EQuickHackType::None)
    {
        QuickHackInstances[ArrayIndex] = CreateQuickHackInstance(QuickHackType);
    }
}

void UQuickHackManagerComponent::SwapQuickHackSlots(int32 SlotA, int32 SlotB)
{
    if (!IsValidSlot(SlotA) || !IsValidSlot(SlotB))
    {
        return;
    }
    
    int32 IndexA = SlotA - 1;
    int32 IndexB = SlotB - 1;
    
    // Swap QuickHack types
    EQuickHackType TempType = EquippedQuickHacks[IndexA];
    EquippedQuickHacks[IndexA] = EquippedQuickHacks[IndexB];
    EquippedQuickHacks[IndexB] = TempType;
    
    // Swap instances
    UQuickHackComponent* TempInstance = QuickHackInstances[IndexA];
    QuickHackInstances[IndexA] = QuickHackInstances[IndexB];
    QuickHackInstances[IndexB] = TempInstance;
}

bool UQuickHackManagerComponent::ActivateQuickHack(int32 SlotIndex)
{
    if (!IsValidSlot(SlotIndex))
    {
        return false;
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    UQuickHackComponent* QuickHack = QuickHackInstances[ArrayIndex];
    
    if (!QuickHack)
    {
        return false;
    }
    
    // Check if SystemOvercharge passive is active (blocks all QuickHacks)
    if (GetOwner())
    {
        UPassiveAbilityComponent* PassiveComp = GetOwner()->FindComponentByClass<UPassiveAbilityComponent>();
        if (PassiveComp && PassiveComp->GetPassiveType() == EPassiveAbilityType::SystemOvercharge)
        {
            UE_LOG(LogTemp, Warning, TEXT("QuickHacks are disabled by System Overcharge passive"));
            return false;
        }
    }
    
    if (QuickHack->CanActivateAbility())
    {
        QuickHack->ActivateAbility();
        return true;
    }
    
    return false;
}

bool UQuickHackManagerComponent::CanActivateQuickHack(int32 SlotIndex) const
{
    if (!IsValidSlot(SlotIndex))
    {
        return false;
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    UQuickHackComponent* QuickHack = QuickHackInstances[ArrayIndex];
    
    if (!QuickHack)
    {
        return false;
    }
    
    // Check if SystemOvercharge passive is active
    if (GetOwner())
    {
        UPassiveAbilityComponent* PassiveComp = GetOwner()->FindComponentByClass<UPassiveAbilityComponent>();
        if (PassiveComp && PassiveComp->GetPassiveType() == EPassiveAbilityType::SystemOvercharge)
        {
            return false;
        }
    }
    
    return QuickHack->CanActivateAbility();
}

float UQuickHackManagerComponent::GetCooldownRemaining(int32 SlotIndex) const
{
    if (!IsValidSlot(SlotIndex))
    {
        return 0.0f;
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    UQuickHackComponent* QuickHack = QuickHackInstances[ArrayIndex];
    
    return QuickHack ? QuickHack->GetCooldownRemaining() : 0.0f;
}

float UQuickHackManagerComponent::GetCastTimeRemaining(int32 SlotIndex) const
{
    if (!IsValidSlot(SlotIndex))
    {
        return 0.0f;
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    UQuickHackComponent* QuickHack = QuickHackInstances[ArrayIndex];
    
    return QuickHack ? QuickHack->GetCastTimeRemaining() : 0.0f;
}

bool UQuickHackManagerComponent::IsQuickHackCasting(int32 SlotIndex) const
{
    if (!IsValidSlot(SlotIndex))
    {
        return false;
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    UQuickHackComponent* QuickHack = QuickHackInstances[ArrayIndex];
    
    return QuickHack ? QuickHack->IsCasting() : false;
}

void UQuickHackManagerComponent::CancelQuickHack(int32 SlotIndex)
{
    if (!IsValidSlot(SlotIndex))
    {
        return;
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    UQuickHackComponent* QuickHack = QuickHackInstances[ArrayIndex];
    
    if (QuickHack && QuickHack->IsCasting())
    {
        QuickHack->CancelQuickHack();
    }
}

bool UQuickHackManagerComponent::HasQuickHackAvailable(EQuickHackType QuickHackType) const
{
    return AvailableQuickHacks.Contains(QuickHackType);
}

void UQuickHackManagerComponent::UnlockQuickHack(EQuickHackType QuickHackType)
{
    if (!HasQuickHackAvailable(QuickHackType))
    {
        AvailableQuickHacks.Add(QuickHackType);
        UE_LOG(LogTemp, Log, TEXT("QuickHackManager: Unlocked %s"), 
            *UEnum::GetValueAsString(QuickHackType));
    }
}

bool UQuickHackManagerComponent::IsValidSlot(int32 SlotIndex) const
{
    return SlotIndex >= 1 && SlotIndex <= MAX_QUICKHACK_SLOTS;
}

FString UQuickHackManagerComponent::GetQuickHackName(EQuickHackType QuickHackType) const
{
    switch (QuickHackType)
    {
        case EQuickHackType::InterruptProtocol:
            return TEXT("Interrupt Protocol");
        case EQuickHackType::SystemFreeze:
            return TEXT("System Freeze");
        case EQuickHackType::Firewall:
            return TEXT("Firewall");
        case EQuickHackType::Kill:
            return TEXT("Kill");
        case EQuickHackType::CascadeVirus:
            return TEXT("Cascade Virus");
        case EQuickHackType::GhostProtocol:
            return TEXT("Ghost Protocol");
        case EQuickHackType::ChargeDrain:
            return TEXT("Charge Drain");
        case EQuickHackType::GravityFlip:
            return TEXT("Gravity Flip");
        case EQuickHackType::None:
        default:
            return TEXT("Empty");
    }
}

FString UQuickHackManagerComponent::GetQuickHackNameInSlot(int32 SlotIndex) const
{
    if (!IsValidSlot(SlotIndex))
    {
        return TEXT("Invalid Slot");
    }
    
    int32 ArrayIndex = SlotIndex - 1;
    if (ArrayIndex < EquippedQuickHacks.Num())
    {
        return GetQuickHackName(EquippedQuickHacks[ArrayIndex]);
    }
    
    return TEXT("Empty");
}