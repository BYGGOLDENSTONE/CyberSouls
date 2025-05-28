#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "InventoryWidget.generated.h"

class UQuickHackManagerComponent;

UCLASS()
class CYBERSOULS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// Override focus behavior to prevent warnings
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	// Initialize the widget with the player's QuickHack manager
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeInventory(UQuickHackManagerComponent* InQuickHackManager);

	// Close the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventory();

protected:
	// Widget components - created programmatically
	UPROPERTY()
	UUniformGridPanel* QuickHackGrid;

	UPROPERTY()
	UUniformGridPanel* PassiveGrid;

	UPROPERTY()
	UButton* CloseButton;

	UPROPERTY()
	UTextBlock* TitleText;

	// QuickHack slot buttons (1-4)
	UPROPERTY()
	UButton* QuickHackSlot1;

	UPROPERTY()
	UButton* QuickHackSlot2;

	UPROPERTY()
	UButton* QuickHackSlot3;

	UPROPERTY()
	UButton* QuickHackSlot4;

	// Passive ability slot buttons (1-4)
	UPROPERTY()
	UButton* PassiveSlot1;

	UPROPERTY()
	UButton* PassiveSlot2;

	UPROPERTY()
	UButton* PassiveSlot3;

	UPROPERTY()
	UButton* PassiveSlot4;

	// Reference to the player's QuickHack manager
	UPROPERTY()
	UQuickHackManagerComponent* QuickHackManager;
	
	// XP display widgets
	UPROPERTY()
	UTextBlock* IntegrityXPText;
	
	UPROPERTY()
	UTextBlock* HackingXPText;

private:
	// Button click handlers
	UFUNCTION()
	void OnQuickHackSlot1Clicked();

	UFUNCTION()
	void OnQuickHackSlot2Clicked();

	UFUNCTION()
	void OnQuickHackSlot3Clicked();

	UFUNCTION()
	void OnQuickHackSlot4Clicked();

	UFUNCTION()
	void OnPassiveSlot1Clicked();

	UFUNCTION()
	void OnPassiveSlot2Clicked();

	UFUNCTION()
	void OnPassiveSlot3Clicked();

	UFUNCTION()
	void OnPassiveSlot4Clicked();

	UFUNCTION()
	void OnCloseButtonClicked();

	// Update the display of all slots
	void UpdateSlotDisplays();

	// Update a specific QuickHack slot display
	void UpdateQuickHackSlot(int32 SlotIndex, UButton* SlotButton);

	// Update a specific passive slot display
	void UpdatePassiveSlot(int32 SlotIndex, UButton* SlotButton);

	// Selected slots for cycling through available abilities
	int32 SelectedQuickHackSlot = 0;
	int32 SelectedPassiveSlot = 0;

	// Available QuickHacks and Passives (will be populated from game data)
	TArray<FString> AvailableQuickHacks;
	TArray<FString> AvailablePassives;

	void InitializeAvailableAbilities();
	
	// Create all widgets programmatically
	void CreateWidgets();
};