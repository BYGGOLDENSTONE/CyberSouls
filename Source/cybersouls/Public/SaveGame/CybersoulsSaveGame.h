#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CybersoulsSaveGame.generated.h"

UCLASS()
class CYBERSOULS_API UCybersoulsSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UCybersoulsSaveGame();

    // Player progression data
    UPROPERTY(SaveGame)
    float IntegrityXP;

    UPROPERTY(SaveGame)
    float HackingXP;

    // Save slot name
    static const FString SaveSlotName;
    static const int32 UserIndex;
};