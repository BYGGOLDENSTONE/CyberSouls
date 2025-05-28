#include "cybersouls/Public/SaveGame/CybersoulsSaveGame.h"

const FString UCybersoulsSaveGame::SaveSlotName = TEXT("CybersoulsSaveSlot");
const int32 UCybersoulsSaveGame::UserIndex = 0;

UCybersoulsSaveGame::UCybersoulsSaveGame()
{
    IntegrityXP = 0.0f;
    HackingXP = 0.0f;
}