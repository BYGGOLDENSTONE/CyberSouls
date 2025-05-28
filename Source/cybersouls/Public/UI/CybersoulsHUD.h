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
	class UInventoryWidget* InventoryWidget;
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
	
	// QuickHack management
	TArray<FString> AvailableQuickHacks;
	TArray<int32> EquippedQuickHackIndices; // Indices into AvailableQuickHacks array
	TArray<FString> AvailablePassives;
	TArray<int32> EquippedPassiveIndices;
	
	// Mouse interaction
	FVector2D LastMousePosition;
	bool bMouseButtonPressed;
	
	// Dropdown menu system
	bool bShowQuickHackDropdown;
	bool bShowPassiveDropdown;
	int32 ActiveDropdownSlot;
	float DropdownStartY;
	float DropdownItemHeight;
	
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
	void DrawCanvasInventory();
	
	// QuickHack management functions
	void InitializeAvailableAbilities();
	void HandleInventoryMouseClick(float MouseX, float MouseY);
	void OpenQuickHackDropdown(int32 SlotIndex, float SlotY);
	void OpenPassiveDropdown(int32 SlotIndex, float SlotY);
	void CloseAllDropdowns();
	void HandleDropdownSelection(float MouseX, float MouseY);
	void DrawQuickHackDropdown(float PanelX, float PanelWidth);
	void DrawPassiveDropdown(float PanelX, float PanelWidth);
	FString GetEquippedQuickHackName(int32 SlotIndex) const;
	FString GetEquippedPassiveName(int32 SlotIndex) const;
	
	// Helper method for drawing QuickHack status
	void DrawQuickHackStatusLine(class UQuickHackComponent* QuickHack, const FString& Name, int32 KeyNumber, float YPosition);
	void DrawQuickHackStatusLineFromManager(class UQuickHackManagerComponent* Manager, int32 SlotIndex, const FString& Name, int32 KeyNumber, float YPosition);
	
public:
	void ShowXPDisplay() { bShowXPDisplay = true; bShowPlayAgainButton = true; }
	void ShowDeathScreen() { bShowDeathScreen = true; }
	void ShowCharacterSwitchNotification(const FString& Text);
	void TogglePersistentXPDisplay() { bShowPersistentXPDisplay = !bShowPersistentXPDisplay; }
	void ToggleInventoryDisplay();

	// Widget class to spawn for inventory (set in Blueprint)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	// Force close inventory and restore game controls
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ForceCloseInventory();
	
	bool IsShowingDeathScreen() const { return bShowDeathScreen; }
	bool IsShowingPlayAgainButton() const { return bShowPlayAgainButton; }
};