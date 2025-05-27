#include "cybersouls/Public/Player/CharacterPoolManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ACharacterPoolManager::ACharacterPoolManager()
{
    PrimaryActorTick.bCanEverTick = false;
    DefaultCharacter = nullptr;
    CyberStateCharacter = nullptr;
    bDefaultCharacterActive = true;
}

void ACharacterPoolManager::BeginPlay()
{
    Super::BeginPlay();
}

void ACharacterPoolManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupPool();
    Super::EndPlay(EndPlayReason);
}

void ACharacterPoolManager::InitializePool(TSubclassOf<ACharacter> DefaultClass, TSubclassOf<ACharacter> CyberStateClass)
{
    DefaultCharacterClass = DefaultClass;
    CyberStateCharacterClass = CyberStateClass;

    // Spawn both characters immediately
    if (DefaultCharacterClass)
    {
        DefaultCharacter = SpawnPooledCharacter(DefaultCharacterClass);
        if (DefaultCharacter)
        {
            ShowCharacter(DefaultCharacter);
            bDefaultCharacterActive = true;
        }
    }

    if (CyberStateCharacterClass)
    {
        CyberStateCharacter = SpawnPooledCharacter(CyberStateCharacterClass);
        if (CyberStateCharacter)
        {
            HideCharacter(CyberStateCharacter);
        }
    }
}

ACharacter* ACharacterPoolManager::GetCharacter(bool bCyberState)
{
    ACharacter* RequestedCharacter = bCyberState ? CyberStateCharacter : DefaultCharacter;
    
    if (RequestedCharacter)
    {
        ShowCharacter(RequestedCharacter);
        bDefaultCharacterActive = !bCyberState;
    }
    
    return RequestedCharacter;
}

void ACharacterPoolManager::ReturnCharacter(ACharacter* Character)
{
    if (Character)
    {
        HideCharacter(Character);
    }
}

ACharacter* ACharacterPoolManager::GetInactiveCharacter(bool bCyberState) const
{
    return bCyberState ? CyberStateCharacter : DefaultCharacter;
}

void ACharacterPoolManager::SwapActiveCharacter()
{
    if (bDefaultCharacterActive)
    {
        if (DefaultCharacter) HideCharacter(DefaultCharacter);
        if (CyberStateCharacter) ShowCharacter(CyberStateCharacter);
        bDefaultCharacterActive = false;
    }
    else
    {
        if (CyberStateCharacter) HideCharacter(CyberStateCharacter);
        if (DefaultCharacter) ShowCharacter(DefaultCharacter);
        bDefaultCharacterActive = true;
    }
}

void ACharacterPoolManager::CleanupPool()
{
    if (DefaultCharacter)
    {
        DefaultCharacter->Destroy();
        DefaultCharacter = nullptr;
    }

    if (CyberStateCharacter)
    {
        CyberStateCharacter->Destroy();
        CyberStateCharacter = nullptr;
    }
}

void ACharacterPoolManager::HideCharacter(ACharacter* Character)
{
    if (!Character) return;

    // Disable collision
    Character->SetActorEnableCollision(false);
    
    // Hide visual representation
    Character->SetActorHiddenInGame(true);
    
    // Disable tick
    Character->SetActorTickEnabled(false);
    
    // Stop movement
    if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
    {
        Movement->StopMovementImmediately();
        Movement->SetComponentTickEnabled(false);
    }
    
    // Move to a safe location off-screen
    Character->SetActorLocation(FVector(0, 0, -10000));
}

void ACharacterPoolManager::ShowCharacter(ACharacter* Character)
{
    if (!Character) return;

    // Re-enable collision
    Character->SetActorEnableCollision(true);
    
    // Show visual representation
    Character->SetActorHiddenInGame(false);
    
    // Enable tick
    Character->SetActorTickEnabled(true);
    
    // Re-enable movement
    if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
    {
        Movement->SetComponentTickEnabled(true);
    }
}

ACharacter* ACharacterPoolManager::SpawnPooledCharacter(TSubclassOf<ACharacter> CharacterClass)
{
    if (!CharacterClass || !GetWorld()) return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    // Spawn at a safe location
    FVector SpawnLocation(0, 0, -10000);
    FRotator SpawnRotation = FRotator::ZeroRotator;
    
    ACharacter* SpawnedCharacter = GetWorld()->SpawnActor<ACharacter>(CharacterClass, SpawnLocation, SpawnRotation, SpawnParams);
    
    return SpawnedCharacter;
}