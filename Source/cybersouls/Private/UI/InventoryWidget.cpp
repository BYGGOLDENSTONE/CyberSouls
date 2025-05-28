#include "cybersouls/Public/UI/InventoryWidget.h"
#include "cybersouls/Public/Abilities/QuickHackManagerComponent.h"
#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Make widget focusable to handle input properly
	SetIsFocusable(true);

	// Only create widgets if we don't have a root widget yet
	if (!WidgetTree || !WidgetTree->RootWidget)
	{
		// Create all widgets programmatically
		CreateWidgets();
	}

	// Bind button click events
	if (QuickHackSlot1)
	{
		QuickHackSlot1->OnClicked.AddDynamic(this, &UInventoryWidget::OnQuickHackSlot1Clicked);
	}
	if (QuickHackSlot2)
	{
		QuickHackSlot2->OnClicked.AddDynamic(this, &UInventoryWidget::OnQuickHackSlot2Clicked);
	}
	if (QuickHackSlot3)
	{
		QuickHackSlot3->OnClicked.AddDynamic(this, &UInventoryWidget::OnQuickHackSlot3Clicked);
	}
	if (QuickHackSlot4)
	{
		QuickHackSlot4->OnClicked.AddDynamic(this, &UInventoryWidget::OnQuickHackSlot4Clicked);
	}

	if (PassiveSlot1)
	{
		PassiveSlot1->OnClicked.AddDynamic(this, &UInventoryWidget::OnPassiveSlot1Clicked);
	}
	if (PassiveSlot2)
	{
		PassiveSlot2->OnClicked.AddDynamic(this, &UInventoryWidget::OnPassiveSlot2Clicked);
	}
	if (PassiveSlot3)
	{
		PassiveSlot3->OnClicked.AddDynamic(this, &UInventoryWidget::OnPassiveSlot3Clicked);
	}
	if (PassiveSlot4)
	{
		PassiveSlot4->OnClicked.AddDynamic(this, &UInventoryWidget::OnPassiveSlot4Clicked);
	}

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnCloseButtonClicked);
	}

	// Initialize available abilities
	InitializeAvailableAbilities();

	// Set title
	if (TitleText)
	{
		TitleText->SetText(FText::FromString(TEXT("CYBERSOULS INVENTORY")));
	}

	UE_LOG(LogTemp, Warning, TEXT("InventoryWidget: NativeConstruct completed"));
}

void UInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// Update slot displays periodically
	UpdateSlotDisplays();
}

void UInventoryWidget::InitializeInventory(UQuickHackManagerComponent* InQuickHackManager)
{
	QuickHackManager = InQuickHackManager;
	UpdateSlotDisplays();
}

void UInventoryWidget::CloseInventory()
{
	// Restore game input mode
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	// Remove from parent
	RemoveFromParent();
}

void UInventoryWidget::InitializeAvailableAbilities()
{
	// Initialize available QuickHacks based on your CLAUDE.md specifications
	AvailableQuickHacks.Empty();
	AvailableQuickHacks.Add(TEXT("Cascade Virus"));
	AvailableQuickHacks.Add(TEXT("Ghost Protocol"));
	AvailableQuickHacks.Add(TEXT("Charge Drain"));
	AvailableQuickHacks.Add(TEXT("Gravity Flip"));
	AvailableQuickHacks.Add(TEXT("System Shock"));
	AvailableQuickHacks.Add(TEXT("Neural Disrupt"));

	// Initialize available passive abilities
	AvailablePassives.Empty();
	AvailablePassives.Add(TEXT("Execution Chains"));
	AvailablePassives.Add(TEXT("System Overcharge"));
	AvailablePassives.Add(TEXT("Neural Buffer"));
	AvailablePassives.Add(TEXT("Cyber Resilience"));
}

void UInventoryWidget::UpdateSlotDisplays()
{
	// Update QuickHack slots
	UpdateQuickHackSlot(0, QuickHackSlot1);
	UpdateQuickHackSlot(1, QuickHackSlot2);
	UpdateQuickHackSlot(2, QuickHackSlot3);
	UpdateQuickHackSlot(3, QuickHackSlot4);

	// Update passive slots
	UpdatePassiveSlot(0, PassiveSlot1);
	UpdatePassiveSlot(1, PassiveSlot2);
	UpdatePassiveSlot(2, PassiveSlot3);
	UpdatePassiveSlot(3, PassiveSlot4);
	
	// Update XP display
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->GetPawn())
	{
		UPlayerProgressionComponent* Progression = PC->GetPawn()->FindComponentByClass<UPlayerProgressionComponent>();
		if (Progression)
		{
			if (IntegrityXPText)
			{
				IntegrityXPText->SetText(FText::FromString(FString::Printf(TEXT("Integrity XP: %.0f"), Progression->GetIntegrityXP())));
			}
			if (HackingXPText)
			{
				HackingXPText->SetText(FText::FromString(FString::Printf(TEXT("Hacking XP: %.0f"), Progression->GetHackingXP())));
			}
		}
	}
}

void UInventoryWidget::UpdateQuickHackSlot(int32 SlotIndex, UButton* SlotButton)
{
	if (!SlotButton) return;

	// Get the currently assigned QuickHack for this slot
	FString SlotContent = TEXT("Empty");
	
	if (QuickHackManager)
	{
		// You'll need to implement GetQuickHackNameForSlot in QuickHackManagerComponent
		// For now, use placeholder logic
		if (SlotIndex < AvailableQuickHacks.Num())
		{
			SlotContent = AvailableQuickHacks[SlotIndex];
		}
	}

	// Update button text (you'll need to add a text child widget to buttons in UMG)
	// For now, just update tooltip
	SlotButton->SetToolTipText(FText::FromString(FString::Printf(TEXT("Slot %d: %s"), SlotIndex + 1, *SlotContent)));
}

void UInventoryWidget::UpdatePassiveSlot(int32 SlotIndex, UButton* SlotButton)
{
	if (!SlotButton) return;

	// Get the currently assigned passive for this slot
	FString SlotContent = TEXT("Empty");
	
	if (SlotIndex < AvailablePassives.Num())
	{
		SlotContent = AvailablePassives[SlotIndex];
	}

	// Update button tooltip
	SlotButton->SetToolTipText(FText::FromString(FString::Printf(TEXT("Passive %d: %s"), SlotIndex + 1, *SlotContent)));
}

// QuickHack slot click handlers - cycle through available QuickHacks
void UInventoryWidget::OnQuickHackSlot1Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("QuickHack Slot 1 clicked"));
	// Implement slot assignment logic
}

void UInventoryWidget::OnQuickHackSlot2Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("QuickHack Slot 2 clicked"));
	// Implement slot assignment logic
}

void UInventoryWidget::OnQuickHackSlot3Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("QuickHack Slot 3 clicked"));
	// Implement slot assignment logic
}

void UInventoryWidget::OnQuickHackSlot4Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("QuickHack Slot 4 clicked"));
	// Implement slot assignment logic
}

// Passive slot click handlers
void UInventoryWidget::OnPassiveSlot1Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Passive Slot 1 clicked"));
	// Implement passive assignment logic
}

void UInventoryWidget::OnPassiveSlot2Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Passive Slot 2 clicked"));
	// Implement passive assignment logic
}

void UInventoryWidget::OnPassiveSlot3Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Passive Slot 3 clicked"));
	// Implement passive assignment logic
}

void UInventoryWidget::OnPassiveSlot4Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Passive Slot 4 clicked"));
	// Implement passive assignment logic
}

void UInventoryWidget::OnCloseButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Close button clicked"));
	
	// Get the HUD and call ForceCloseInventory to ensure proper cleanup
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		if (ACybersoulsHUD* CyberHUD = Cast<ACybersoulsHUD>(PC->GetHUD()))
		{
			CyberHUD->ForceCloseInventory();
		}
		else
		{
			// Fallback to local close if HUD not available
			CloseInventory();
		}
	}
}

void UInventoryWidget::CreateWidgets()
{
	// Create a canvas panel as root
	UCanvasPanel* RootCanvas = NewObject<UCanvasPanel>(this);
	RootCanvas->SetVisibility(ESlateVisibility::Visible);
	
	// Create main container with background
	UBorder* BackgroundBorder = NewObject<UBorder>(this);
	BackgroundBorder->SetVisibility(ESlateVisibility::Visible);
	
	UVerticalBox* MainContainer = NewObject<UVerticalBox>(this);
	MainContainer->SetVisibility(ESlateVisibility::Visible);
	
	// Create title
	TitleText = NewObject<UTextBlock>(this);
	TitleText->SetText(FText::FromString(TEXT("CYBERSOULS INVENTORY")));
	
	// Style the title with color only
	TitleText->SetColorAndOpacity(FLinearColor::White);
	
	// Create XP display section
	UHorizontalBox* XPContainer = NewObject<UHorizontalBox>(this);
	
	IntegrityXPText = NewObject<UTextBlock>(this);
	IntegrityXPText->SetText(FText::FromString(TEXT("Integrity XP: 0")));
	IntegrityXPText->SetColorAndOpacity(FLinearColor::Green);
	
	HackingXPText = NewObject<UTextBlock>(this);
	HackingXPText->SetText(FText::FromString(TEXT("Hacking XP: 0")));
	HackingXPText->SetColorAndOpacity(FLinearColor::Blue);
	
	// Add spacing between XP displays
	UTextBlock* SpacerText = NewObject<UTextBlock>(this);
	SpacerText->SetText(FText::FromString(TEXT("    ")));
	
	XPContainer->AddChild(IntegrityXPText);
	XPContainer->AddChild(SpacerText);
	XPContainer->AddChild(HackingXPText);
	
	// Create QuickHack section
	UTextBlock* QuickHackLabel = NewObject<UTextBlock>(this);
	QuickHackLabel->SetText(FText::FromString(TEXT("QuickHacks")));
	
	QuickHackGrid = NewObject<UUniformGridPanel>(this);
	
	// Create QuickHack slots with text
	QuickHackSlot1 = NewObject<UButton>(this);
	UTextBlock* QH1Text = NewObject<UTextBlock>(this);
	QH1Text->SetText(FText::FromString(TEXT("Slot 1")));
	QuickHackSlot1->AddChild(QH1Text);
	
	QuickHackSlot2 = NewObject<UButton>(this);
	UTextBlock* QH2Text = NewObject<UTextBlock>(this);
	QH2Text->SetText(FText::FromString(TEXT("Slot 2")));
	QuickHackSlot2->AddChild(QH2Text);
	
	QuickHackSlot3 = NewObject<UButton>(this);
	UTextBlock* QH3Text = NewObject<UTextBlock>(this);
	QH3Text->SetText(FText::FromString(TEXT("Slot 3")));
	QuickHackSlot3->AddChild(QH3Text);
	
	QuickHackSlot4 = NewObject<UButton>(this);
	UTextBlock* QH4Text = NewObject<UTextBlock>(this);
	QH4Text->SetText(FText::FromString(TEXT("Slot 4")));
	QuickHackSlot4->AddChild(QH4Text);
	
	// Add QuickHack slots to grid
	QuickHackGrid->AddChildToUniformGrid(QuickHackSlot1, 0, 0);
	QuickHackGrid->AddChildToUniformGrid(QuickHackSlot2, 0, 1);
	QuickHackGrid->AddChildToUniformGrid(QuickHackSlot3, 1, 0);
	QuickHackGrid->AddChildToUniformGrid(QuickHackSlot4, 1, 1);
	
	// Create Passive section
	UTextBlock* PassiveLabel = NewObject<UTextBlock>(this);
	PassiveLabel->SetText(FText::FromString(TEXT("Passive Abilities")));
	
	PassiveGrid = NewObject<UUniformGridPanel>(this);
	
	// Create Passive slots with text
	PassiveSlot1 = NewObject<UButton>(this);
	UTextBlock* P1Text = NewObject<UTextBlock>(this);
	P1Text->SetText(FText::FromString(TEXT("Passive 1")));
	PassiveSlot1->AddChild(P1Text);
	
	PassiveSlot2 = NewObject<UButton>(this);
	UTextBlock* P2Text = NewObject<UTextBlock>(this);
	P2Text->SetText(FText::FromString(TEXT("Passive 2")));
	PassiveSlot2->AddChild(P2Text);
	
	PassiveSlot3 = NewObject<UButton>(this);
	UTextBlock* P3Text = NewObject<UTextBlock>(this);
	P3Text->SetText(FText::FromString(TEXT("Passive 3")));
	PassiveSlot3->AddChild(P3Text);
	
	PassiveSlot4 = NewObject<UButton>(this);
	UTextBlock* P4Text = NewObject<UTextBlock>(this);
	P4Text->SetText(FText::FromString(TEXT("Passive 4")));
	PassiveSlot4->AddChild(P4Text);
	
	// Add Passive slots to grid
	PassiveGrid->AddChildToUniformGrid(PassiveSlot1, 0, 0);
	PassiveGrid->AddChildToUniformGrid(PassiveSlot2, 0, 1);
	PassiveGrid->AddChildToUniformGrid(PassiveSlot3, 1, 0);
	PassiveGrid->AddChildToUniformGrid(PassiveSlot4, 1, 1);
	
	// Create close button
	CloseButton = NewObject<UButton>(this);
	UTextBlock* CloseButtonText = NewObject<UTextBlock>(this);
	CloseButtonText->SetText(FText::FromString(TEXT("Close")));
	CloseButton->AddChild(CloseButtonText);
	
	// Build layout
	MainContainer->AddChild(TitleText);
	MainContainer->AddChild(XPContainer);  // Add XP display after title
	MainContainer->AddChild(QuickHackLabel);
	MainContainer->AddChild(QuickHackGrid);
	MainContainer->AddChild(PassiveLabel);
	MainContainer->AddChild(PassiveGrid);
	MainContainer->AddChild(CloseButton);
	
	// Set background color and put main container inside border
	BackgroundBorder->SetBrushColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f)); // Dark background
	
	// Set padding for the border
	FMargin BorderPadding(50.0f);
	BackgroundBorder->SetPadding(BorderPadding);
	
	BackgroundBorder->AddChild(MainContainer);
	
	// Add the border to the root canvas
	RootCanvas->AddChild(BackgroundBorder);
	
	// Configure the border slot to center and size properly
	if (UCanvasPanelSlot* BorderSlot = Cast<UCanvasPanelSlot>(BackgroundBorder->Slot))
	{
		BorderSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		BorderSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		BorderSlot->SetSize(FVector2D(800.0f, 600.0f));
		BorderSlot->SetPosition(FVector2D(0.0f, 0.0f));
	}
	
	// Set the canvas as root widget
	if (WidgetTree)
	{
		WidgetTree->RootWidget = RootCanvas;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("CreateWidgets: Root widget created and set. WidgetTree: %s, RootCanvas children: %d"), 
		WidgetTree ? TEXT("Valid") : TEXT("NULL"),
		RootCanvas->GetChildrenCount());
	
}

FReply UInventoryWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	// Redirect focus to first button
	if (QuickHackSlot1)
	{
		QuickHackSlot1->SetKeyboardFocus();
		return FReply::Handled();
	}
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}