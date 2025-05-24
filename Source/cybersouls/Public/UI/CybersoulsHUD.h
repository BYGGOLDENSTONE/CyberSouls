// CybersoulsHUD.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CybersoulsHUD.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACybersoulsHUD();

	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

private:
	class AcybersoulsCharacter* PlayerCharacter;
	class APlayerCyberState* PlayerCyberState;
	class ACyberSoulsPlayerController* CyberSoulsController;
	class UFont* HUDFont;
	bool bShowXPDisplay;
	bool bShowPersistentXPDisplay;
	bool bShowInventoryDisplay;
	bool bShowDeathScreen;
	bool bShowPlayAgainButton;
	
	// Character switch notification
	bool bShowSwitchNotification;
	float SwitchNotificationTimer;
	FString SwitchNotificationText;
	
	// Inventory system
	int32 SelectedQuickHackSlot;
	int32 SelectedPassiveSlot;
	
	static constexpr float SWITCH_NOTIFICATION_DURATION = 2.0f;

	void DrawIntegrityBar();
	void DrawHackProgressBar();
	void DrawQuickHackStatus();
	void DrawTargetInfo();
	void DrawTargetBodyPartIndicator();
	void DrawXPDisplay();
	void DrawPersistentXPDisplay();
	void DrawInventoryDisplay();
	void DrawCrosshair();
	void DrawStaminaBar();
	void DrawDashCharges();
	void DrawCharacterIndicator();
	void DrawSwitchNotification();
	void DrawDeathScreen();
	void DrawPlayAgainButton();
	
	// Helper method for drawing QuickHack status
	void DrawQuickHackStatusLine(class UQuickHackComponent* QuickHack, const FString& Name, int32 KeyNumber, float YPosition);
	void DrawQuickHackStatusLineFromManager(class UQuickHackManagerComponent* Manager, int32 SlotIndex, const FString& Name, int32 KeyNumber, float YPosition);
	
public:
	void ShowXPDisplay() { bShowXPDisplay = true; bShowPlayAgainButton = true; }
	void ShowDeathScreen() { bShowDeathScreen = true; }
	void ShowCharacterSwitchNotification(const FString& Text);
	void TogglePersistentXPDisplay() { bShowPersistentXPDisplay = !bShowPersistentXPDisplay; }
	void ToggleInventoryDisplay() { bShowInventoryDisplay = !bShowInventoryDisplay; }
	
	bool IsShowingDeathScreen() const { return bShowDeathScreen; }
	bool IsShowingPlayAgainButton() const { return bShowPlayAgainButton; }
};