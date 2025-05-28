// CybersoulsHUD.cpp
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/UI/InventoryWidget.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Character/PlayerCyberState.h"
#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h"
#include "cybersouls/Public/Player/DashAbilityComponent.h"
#include "cybersouls/Public/Abilities/SlashAbilityComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
#include "cybersouls/Public/Abilities/QuickHackManagerComponent.h"
#include "cybersouls/Public/Abilities/BlockAbilityComponent.h"
#include "cybersouls/Public/Abilities/DodgeAbilityComponent.h"
#include "cybersouls/Public/Combat/TargetLockComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"

ACybersoulsHUD::ACybersoulsHUD()
{
	// Load default font
	static ConstructorHelpers::FObjectFinder<UFont> HUDFontObject(TEXT("/Engine/EngineFonts/Roboto"));
	if (HUDFontObject.Succeeded())
	{
		HUDFont = HUDFontObject.Object;
	}
	
	// Initialize displays to hidden
	bShowXPDisplay = false;
	bShowPersistentXPDisplay = false;
	bShowInventoryDisplay = false;
	bShowDeathScreen = false;
	bShowPlayAgainButton = false;
	bShowSwitchNotification = false;
	SwitchNotificationTimer = 0.0f;
	InventoryWidget = nullptr;
	
	// Initialize inventory selection
	SelectedQuickHackSlot = 0;
	SelectedPassiveSlot = 0;
	
	// Initialize mouse interaction
	LastMousePosition = FVector2D::ZeroVector;
	bMouseButtonPressed = false;
	
	// Initialize dropdown system
	bShowQuickHackDropdown = false;
	bShowPassiveDropdown = false;
	ActiveDropdownSlot = -1;
	DropdownStartY = 0.0f;
	DropdownItemHeight = 25.0f;
	
	// Initialize available abilities
	InitializeAvailableAbilities();
}

void ACybersoulsHUD::BeginPlay()
{
	Super::BeginPlay();

	CyberSoulsController = Cast<ACyberSoulsPlayerController>(GetOwningPlayerController());
}

void ACybersoulsHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas || !IsValid(CyberSoulsController))
	{
		return;
	}
	
	// Canvas-based inventory display (always visible when toggled)
	if (bShowInventoryDisplay)
	{
		DrawCanvasInventory();
	}

	// Update character references based on current possession
	APawn* CurrentPawn = CyberSoulsController->GetPawn();
	if (CyberSoulsController->IsUsingCyberState())
	{
		PlayerCyberState = Cast<APlayerCyberState>(CurrentPawn);
		PlayerCharacter = nullptr;
	}
	else
	{
		PlayerCharacter = Cast<AcybersoulsCharacter>(CurrentPawn);
		PlayerCyberState = nullptr;
	}

	// Draw common elements
	DrawCrosshair();
	DrawXPDisplay();
	// Removed DrawPersistentXPDisplay - XP now shown in inventory
	DrawDeathScreen();
	DrawPlayAgainButton();
	DrawCharacterIndicator();
	DrawSwitchNotification();

	// Draw character-specific UI
	if (PlayerCharacter)
	{
		DrawIntegrityBar();
		DrawHackProgressBar();
		DrawQuickHackStatus();
		DrawTargetInfo();
		DrawTargetBodyPartIndicator();
	}
	else if (PlayerCyberState)
	{
		DrawStaminaBar();
		DrawDashCharges();
		// Cyber state uses different abilities, no quickhacks
	}
}

void ACybersoulsHUD::DrawIntegrityBar()
{
	UPlayerAttributeComponent* Attributes = PlayerCharacter->GetPlayerAttributes();
	if (!Attributes)
	{
		return;
	}

	float IntegrityPercent = Attributes->Integrity / Attributes->MaxIntegrity;
	
	// Draw integrity bar
	float BarWidth = 300.0f;
	float BarHeight = 20.0f;
	float BarX = 50.0f;
	float BarY = 50.0f;

	// Background
	DrawRect(FLinearColor::Black, BarX, BarY, BarWidth, BarHeight);

	// Integrity fill
	FLinearColor IntegrityColor = IntegrityPercent > 0.5f ? FLinearColor::Green : 
								 IntegrityPercent > 0.25f ? FLinearColor::Yellow : FLinearColor::Red;
	DrawRect(IntegrityColor, BarX, BarY, BarWidth * IntegrityPercent, BarHeight);

	// Text
	FString IntegrityText = FString::Printf(TEXT("Integrity: %.0f/%.0f"), Attributes->Integrity, Attributes->MaxIntegrity);
	DrawText(IntegrityText, FColor::White, BarX, BarY - 20, HUDFont);
}

void ACybersoulsHUD::DrawHackProgressBar()
{
	UPlayerAttributeComponent* Attributes = PlayerCharacter->GetPlayerAttributes();
	if (!Attributes)
	{
		return;
	}

	float HackPercent = Attributes->HackProgress / Attributes->MaxHackProgress;
	
	// Draw hack progress bar
	float BarWidth = 300.0f;
	float BarHeight = 20.0f;
	float BarX = 50.0f;
	float BarY = 90.0f;

	// Background
	DrawRect(FLinearColor::Black, BarX, BarY, BarWidth, BarHeight);

	// Hack progress fill
	FLinearColor HackColor = HackPercent < 0.5f ? FLinearColor::Blue : 
							HackPercent < 0.75f ? FLinearColor::Yellow : FLinearColor::Red;
	DrawRect(HackColor, BarX, BarY, BarWidth * HackPercent, BarHeight);

	// Text
	FString HackText = FString::Printf(TEXT("Hack Progress: %.0f%%"), HackPercent * 100.0f);
	DrawText(HackText, FColor::White, BarX, BarY - 20, HUDFont);
}

void ACybersoulsHUD::DrawQuickHackStatus()
{
	if (!PlayerCharacter || !PlayerCharacter->GetQuickHackManager())
	{
		return;
	}

	float StartY = 150.0f;
	float LineHeight = 25.0f;
	
	UQuickHackManagerComponent* QuickHackManager = PlayerCharacter->GetQuickHackManager();
	
	// Draw each QuickHack using the actual names from the manager
	for (int32 i = 0; i < 4; i++)
	{
		int32 SlotIndex = i + 1;
		FString QuickHackName = QuickHackManager->GetQuickHackNameInSlot(SlotIndex);
		
		// Always draw the slot, even if empty
		DrawQuickHackStatusLineFromManager(QuickHackManager, SlotIndex, QuickHackName, SlotIndex, StartY + i * LineHeight);
	}
}

void ACybersoulsHUD::DrawQuickHackStatusLine(UQuickHackComponent* QuickHack, const FString& Name, int32 KeyNumber, float YPosition)
{
	if (!QuickHack) return;

	FString StatusText = FString::Printf(TEXT("%d. %s"), KeyNumber, *Name);
	float Cooldown = QuickHack->CurrentCooldown;
	
	if (QuickHack->IsQuickHackActive())
	{
		// Show casting progress
		float CastPercent = QuickHack->CurrentCastTime / QuickHack->CastTime;
		StatusText += FString::Printf(TEXT(" [Casting: %.0f%%]"), CastPercent * 100.0f);
	}
	else if (Cooldown > 0.0f)
	{
		StatusText += FString::Printf(TEXT(" (%.1fs)"), Cooldown);
	}
	else
	{
		StatusText += TEXT(" (Ready)");
	}

	FColor TextColor = QuickHack->IsQuickHackActive() ? FColor::Yellow : (Cooldown > 0.0f ? FColor::Red : FColor::Green);
	DrawText(StatusText, TextColor, 50, YPosition, HUDFont);
}

void ACybersoulsHUD::DrawQuickHackStatusLineFromManager(UQuickHackManagerComponent* Manager, int32 SlotIndex, const FString& Name, int32 KeyNumber, float YPosition)
{
	if (!Manager) return;

	FString StatusText = FString::Printf(TEXT("%d. %s"), KeyNumber, *Name);
	float Cooldown = Manager->GetCooldownRemaining(SlotIndex);
	
	if (Manager->IsQuickHackCasting(SlotIndex))
	{
		// Show casting progress
		float CastTime = Manager->GetCastTimeRemaining(SlotIndex);
		StatusText += FString::Printf(TEXT(" [Casting: %.1fs]"), CastTime);
	}
	else if (Cooldown > 0.0f)
	{
		StatusText += FString::Printf(TEXT(" (%.1fs)"), Cooldown);
	}
	else
	{
		StatusText += TEXT(" (Ready)");
	}

	FColor TextColor = Manager->IsQuickHackCasting(SlotIndex) ? FColor::Yellow : (Cooldown > 0.0f ? FColor::Red : FColor::Green);
	DrawText(StatusText, TextColor, 50, YPosition, HUDFont);
}

void ACybersoulsHUD::DrawTargetInfo()
{
	AActor* Target = PlayerCharacter->GetCrosshairTarget();
	ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(Target);
	if (!Enemy)
	{
		return;
	}

	// Draw target info
	float InfoX = Canvas->SizeX - 350.0f;
	float InfoY = 50.0f;

	// Enemy type
	FString EnemyTypeText = TEXT("Target: ");
	switch (Enemy->EnemyType)
	{
		case EEnemyType::Basic: EnemyTypeText += TEXT("Basic Enemy"); break;
		case EEnemyType::Block: EnemyTypeText += TEXT("Block Enemy"); break;
		case EEnemyType::Dodge: EnemyTypeText += TEXT("Dodge Enemy"); break;
		case EEnemyType::Netrunner: EnemyTypeText += TEXT("Netrunner"); break;
		case EEnemyType::BuffNetrunner: EnemyTypeText += TEXT("Buff Netrunner"); break;
		case EEnemyType::DebuffNetrunner: EnemyTypeText += TEXT("Debuff Netrunner"); break;
	}
	DrawText(EnemyTypeText, FColor::White, InfoX, InfoY, HUDFont);

	// Targeted body part
	FString BodyPartText = TEXT("Crosshair: ");
	switch (PlayerCharacter->GetCrosshairBodyPart())
	{
		case EBodyPart::UpperBody: BodyPartText += TEXT("Upper Body"); break;
		case EBodyPart::LeftLeg: 
		case EBodyPart::RightLeg: BodyPartText += TEXT("Legs"); break;
		default: BodyPartText += TEXT("Unknown"); break;
	}
	DrawText(BodyPartText, FColor::Yellow, InfoX, InfoY + 25, HUDFont);

	// Enemy charges (if applicable)
	if (Enemy->EnemyType == EEnemyType::Block)
	{
		UBlockAbilityComponent* BlockAbility = Enemy->FindComponentByClass<UBlockAbilityComponent>();
		if (BlockAbility)
		{
			FString BlockText = FString::Printf(TEXT("Block Charges: %d"), BlockAbility->CurrentBlockCharges);
			DrawText(BlockText, FColor::Blue, InfoX, InfoY + 50, HUDFont);
		}
	}
		else if (Enemy->EnemyType == EEnemyType::Dodge)
		{
			UDodgeAbilityComponent* DodgeAbility = Enemy->FindComponentByClass<UDodgeAbilityComponent>();
			if (DodgeAbility)
			{
				FString DodgeText = FString::Printf(TEXT("Dodge Charges: %d"), DodgeAbility->CurrentDodgeCharges);
				DrawText(DodgeText, FColor::Green, InfoX, InfoY + 50, HUDFont);
			}
		}
}

void ACybersoulsHUD::DrawTargetBodyPartIndicator()
{
	// This function is now obsolete as crosshair handles targeting
	// Crosshair in center screen shows what you're aiming at
}

// Removed DrawEnemyQuickHackCasting function - no longer showing enemy casting indicators

void ACybersoulsHUD::DrawXPDisplay()
{
	// Only show XP display after quest completion
	if (!bShowXPDisplay)                                      
	{
		return;
	}
	
	// Get progression from whichever character is active
	UPlayerProgressionComponent* Progression = nullptr;
	if (PlayerCharacter)
	{
		Progression = PlayerCharacter->GetPlayerProgression();
	}
	else if (PlayerCyberState)
	{
		Progression = PlayerCyberState->FindComponentByClass<UPlayerProgressionComponent>();
	}
	
	if (!Progression)
	{
		return;
	}

	// Position XP display in top middle of screen
	float BGWidth = 400.0f;
	float BGHeight = 140.0f;
	float XPX = (Canvas->SizeX - BGWidth) * 0.5f;
	float XPY = 50.0f;
	float LineHeight = 40.0f;

	// Draw larger XP background with border
	// Dark background with border
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.95f), XPX, XPY, BGWidth, BGHeight);
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), XPX, XPY, BGWidth, 4.0f); // Top border
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), XPX, XPY, 4.0f, BGHeight); // Left border
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), XPX + BGWidth - 4, XPY, 4.0f, BGHeight); // Right border
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), XPX, XPY + BGHeight - 4, BGWidth, 4.0f); // Bottom border

	// Large title with cyberpunk styling - centered
	float TitleX = XPX + (BGWidth - 240.0f) * 0.5f; // Approximate center for title
	DrawText(TEXT("▰▰ QUEST COMPLETE ▰▰"), FColor::Cyan, TitleX, XPY + 15, HUDFont, 1.4f);

	// Integrity XP with larger font and bright colors - centered
	FString IntegrityXPText = FString::Printf(TEXT("⚡ INTEGRITY XP: %.0f"), Progression->GetIntegrityXP());
	float IntegrityX = XPX + (BGWidth - 200.0f) * 0.5f;
	DrawText(IntegrityXPText, FColor::Green, IntegrityX, XPY + 15 + LineHeight, HUDFont, 1.2f);

	// Hacking XP with larger font and bright colors - centered
	FString HackingXPText = FString::Printf(TEXT("🔧 HACKING XP: %.0f"), Progression->GetHackingXP());
	float HackingX = XPX + (BGWidth - 200.0f) * 0.5f;
	DrawText(HackingXPText, FColor::Blue, HackingX, XPY + 15 + 2 * LineHeight, HUDFont, 1.2f);
}

void ACybersoulsHUD::DrawPersistentXPDisplay()
{
	// Only show when toggled by Tab key
	if (!bShowPersistentXPDisplay)
	{
		return;
	}
	
	// Get progression from whichever character is active
	UPlayerProgressionComponent* Progression = nullptr;
	if (PlayerCharacter)
	{
		Progression = PlayerCharacter->GetPlayerProgression();
	}
	else if (PlayerCyberState)
	{
		Progression = PlayerCyberState->FindComponentByClass<UPlayerProgressionComponent>();
	}
	
	if (!Progression)
	{
		return;
	}

	// Position XP display in top-left corner (different from quest complete display)
	float BGWidth = 300.0f;
	float BGHeight = 120.0f;
	float XPX = 20.0f;
	float XPY = 150.0f; // Below other HUD elements
	float LineHeight = 30.0f;

	// Draw background with border
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.85f), XPX, XPY, BGWidth, BGHeight);
	DrawRect(FLinearColor(0.0f, 1.0f, 0.8f, 1.0f), XPX, XPY, BGWidth, 3.0f); // Top border
	DrawRect(FLinearColor(0.0f, 1.0f, 0.8f, 1.0f), XPX, XPY, 3.0f, BGHeight); // Left border
	DrawRect(FLinearColor(0.0f, 1.0f, 0.8f, 1.0f), XPX + BGWidth - 3, XPY, 3.0f, BGHeight); // Right border
	DrawRect(FLinearColor(0.0f, 1.0f, 0.8f, 1.0f), XPX, XPY + BGHeight - 3, BGWidth, 3.0f); // Bottom border

	// Title
	DrawText(TEXT("═ XP PROGRESSION ═"), FColor::Cyan, XPX + 70.0f, XPY + 10, HUDFont, 1.1f);

	// Integrity XP
	FString IntegrityXPText = FString::Printf(TEXT("⚡ Integrity XP: %.0f"), Progression->GetIntegrityXP());
	DrawText(IntegrityXPText, FColor::Green, XPX + 20.0f, XPY + 10 + LineHeight, HUDFont, 1.0f);

	// Hacking XP
	FString HackingXPText = FString::Printf(TEXT("🔧 Hacking XP: %.0f"), Progression->GetHackingXP());
	DrawText(HackingXPText, FColor::Blue, XPX + 20.0f, XPY + 10 + 2 * LineHeight, HUDFont, 1.0f);

	// Instructions
	DrawText(TEXT("Press [Tab] to close"), FColor::White, XPX + 90.0f, XPY + 10 + 3 * LineHeight, HUDFont, 0.8f);
}

void ACybersoulsHUD::DrawInventoryDisplay()
{
	// Only show when toggled by E key
	if (!bShowInventoryDisplay)
	{
		return;
	}

	// Only show inventory for default character (QuickHacks not available in CyberState)
	if (!PlayerCharacter)
	{
		// Show message for CyberState
		float BGWidth = 400.0f;
		float BGHeight = 100.0f;
		float InvX = (Canvas->SizeX - BGWidth) * 0.5f;
		float InvY = (Canvas->SizeY - BGHeight) * 0.5f;

		DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f), InvX, InvY, BGWidth, BGHeight);
		DrawRect(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f), InvX, InvY, BGWidth, 4.0f);
		DrawRect(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f), InvX, InvY, 4.0f, BGHeight);
		DrawRect(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f), InvX + BGWidth - 4, InvY, 4.0f, BGHeight);
		DrawRect(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f), InvX, InvY + BGHeight - 4, BGWidth, 4.0f);

		DrawText(TEXT("INVENTORY NOT AVAILABLE"), FColor::Red, InvX + 80.0f, InvY + 25.0f, HUDFont, 1.2f);
		DrawText(TEXT("Switch to Default Character"), FColor::White, InvX + 80.0f, InvY + 50.0f, HUDFont, 1.0f);
		DrawText(TEXT("Press [E] to close"), FColor::Yellow, InvX + 140.0f, InvY + 75.0f, HUDFont, 0.8f);
		return;
	}

	// Main inventory display
	float BGWidth = 800.0f;
	float BGHeight = 600.0f;
	float InvX = (Canvas->SizeX - BGWidth) * 0.5f;
	float InvY = (Canvas->SizeY - BGHeight) * 0.5f;

	// Main background
	DrawRect(FLinearColor(0.05f, 0.05f, 0.1f, 0.95f), InvX, InvY, BGWidth, BGHeight);
	
	// Border
	float BorderThickness = 4.0f;
	FLinearColor BorderColor = FLinearColor(0.2f, 0.8f, 1.0f, 1.0f);
	DrawRect(BorderColor, InvX, InvY, BGWidth, BorderThickness); // Top
	DrawRect(BorderColor, InvX, InvY, BorderThickness, BGHeight); // Left
	DrawRect(BorderColor, InvX + BGWidth - BorderThickness, InvY, BorderThickness, BGHeight); // Right
	DrawRect(BorderColor, InvX, InvY + BGHeight - BorderThickness, BGWidth, BorderThickness); // Bottom

	// Title
	DrawText(TEXT("▰▰▰ CYBERWARE INVENTORY ▰▰▰"), FColor::Cyan, InvX + 250.0f, InvY + 20.0f, HUDFont, 1.5f);

	// QuickHack section
	float SectionY = InvY + 80.0f;
	DrawText(TEXT("═══ QUICKHACK SLOTS ═══"), FColor::Yellow, InvX + 50.0f, SectionY, HUDFont, 1.2f);

	// QuickHack slots (1-4)
	float SlotSize = 120.0f;
	float SlotSpacing = 20.0f;
	float SlotStartX = InvX + 50.0f;
	float SlotY = SectionY + 40.0f;

	TArray<FString> QuickHackNames = {TEXT("Interrupt Protocol"), TEXT("System Freeze"), TEXT("Firewall"), TEXT("Kill")};
	TArray<FColor> SlotColors = {FColor::Green, FColor::Blue, FColor::Purple, FColor::Red};

	for (int32 i = 0; i < 4; i++)
	{
		float SlotX = SlotStartX + i * (SlotSize + SlotSpacing);
		
		// Slot background
		FLinearColor SlotBG = (i == SelectedQuickHackSlot) ? 
			FLinearColor(0.3f, 0.6f, 1.0f, 0.8f) : 
			FLinearColor(0.1f, 0.1f, 0.2f, 0.8f);
		DrawRect(SlotBG, SlotX, SlotY, SlotSize, SlotSize);
		
		// Slot border
		FLinearColor SlotBorder = (i == SelectedQuickHackSlot) ? 
			FLinearColor(1.0f, 1.0f, 0.0f, 1.0f) : 
			FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
		DrawRect(SlotBorder, SlotX, SlotY, SlotSize, 3.0f); // Top
		DrawRect(SlotBorder, SlotX, SlotY, 3.0f, SlotSize); // Left
		DrawRect(SlotBorder, SlotX + SlotSize - 3, SlotY, 3.0f, SlotSize); // Right
		DrawRect(SlotBorder, SlotX, SlotY + SlotSize - 3, SlotSize, 3.0f); // Bottom

		// Key number
		FString KeyText = FString::Printf(TEXT("[%d]"), i + 1);
		DrawText(KeyText, FColor::White, SlotX + 10.0f, SlotY + 10.0f, HUDFont, 1.1f);

		// QuickHack name
		DrawText(QuickHackNames[i], SlotColors[i], SlotX + 10.0f, SlotY + 40.0f, HUDFont, 0.9f);

		// Status
		if (PlayerCharacter && PlayerCharacter->GetQuickHackManager())
		{
			UQuickHackManagerComponent* QuickHackManager = PlayerCharacter->GetQuickHackManager();
			float Cooldown = QuickHackManager->GetCooldownRemaining(i + 1); // Slots are 1-based
			
			FString StatusText = Cooldown > 0.0f ? 
				FString::Printf(TEXT("CD: %.1fs"), Cooldown) : 
				TEXT("Ready");
			FColor StatusColor = Cooldown > 0.0f ? FColor::Red : FColor::Green;
			DrawText(StatusText, StatusColor, SlotX + 10.0f, SlotY + 70.0f, HUDFont, 0.8f);
		}
	}

	// Passive abilities section
	float PassiveY = SectionY + 200.0f;
	DrawText(TEXT("═══ PASSIVE ABILITY SLOTS ═══"), FColor::Orange, InvX + 50.0f, PassiveY, HUDFont, 1.2f);

	// Passive slots (future implementation)
	TArray<FString> PassiveNames = {TEXT("Execution Chain"), TEXT("System Overcharge"), TEXT("Neural Buffer"), TEXT("Cyber Resilience")};
	TArray<FColor> PassiveColors = {FColor::Red, FColor::Blue, FColor::Green, FColor::Purple};

	float PassiveSlotY = PassiveY + 40.0f;
	for (int32 i = 0; i < 4; i++)
	{
		float SlotX = SlotStartX + i * (SlotSize + SlotSpacing);
		
		// Slot background
		FLinearColor SlotBG = (i == SelectedPassiveSlot) ? 
			FLinearColor(0.6f, 0.3f, 1.0f, 0.8f) : 
			FLinearColor(0.2f, 0.1f, 0.1f, 0.8f);
		DrawRect(SlotBG, SlotX, PassiveSlotY, SlotSize, SlotSize);
		
		// Slot border
		FLinearColor SlotBorder = (i == SelectedPassiveSlot) ? 
			FLinearColor(1.0f, 0.5f, 0.0f, 1.0f) : 
			FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
		DrawRect(SlotBorder, SlotX, PassiveSlotY, SlotSize, 3.0f); // Top
		DrawRect(SlotBorder, SlotX, PassiveSlotY, 3.0f, SlotSize); // Left
		DrawRect(SlotBorder, SlotX + SlotSize - 3, PassiveSlotY, 3.0f, SlotSize); // Right
		DrawRect(SlotBorder, SlotX, PassiveSlotY + SlotSize - 3, SlotSize, 3.0f); // Bottom

		// Passive name
		DrawText(PassiveNames[i], PassiveColors[i], SlotX + 10.0f, PassiveSlotY + 40.0f, HUDFont, 0.9f);

		// Status (Coming Soon)
		DrawText(TEXT("Coming Soon"), FColor(128, 128, 128), SlotX + 10.0f, PassiveSlotY + 70.0f, HUDFont, 0.8f);
	}

	// Instructions
	float InstructY = InvY + BGHeight - 80.0f;
	DrawText(TEXT("Navigation Instructions:"), FColor::Cyan, InvX + 50.0f, InstructY, HUDFont, 1.1f);
	DrawText(TEXT("• Use Number Keys (1-4) to select QuickHack slots"), FColor::White, InvX + 70.0f, InstructY + 25.0f, HUDFont, 0.9f);
	DrawText(TEXT("• Passive abilities will be available in future updates"), FColor::White, InvX + 70.0f, InstructY + 45.0f, HUDFont, 0.9f);
	DrawText(TEXT("• Press [E] to close inventory"), FColor::Yellow, InvX + 70.0f, InstructY + 65.0f, HUDFont, 0.9f);
}

void ACybersoulsHUD::DrawCrosshair()
{
	if (!Canvas)
	{
		return;
	}

	// Get screen center
	float CenterX = Canvas->SizeX * 0.5f;
	float CenterY = Canvas->SizeY * 0.5f;
	
	// Check if aiming at enemy to change crosshair color
	bool bAimingAtEnemy = false;
	if (PlayerCharacter)
	{
		bAimingAtEnemy = PlayerCharacter->GetCrosshairTarget() != nullptr;
	}
	
	// Draw small dot crosshair
	float DotSize = 2.0f;
	FLinearColor CrosshairColor = bAimingAtEnemy ? FLinearColor::Red : FLinearColor::White;
	
	// Draw filled circle (dot) in the center
	DrawRect(CrosshairColor, CenterX - DotSize, CenterY - DotSize, DotSize * 2, DotSize * 2);
	
	// If aiming at enemy, draw additional targeting ring
	if (bAimingAtEnemy)
	{
		float RingSize = 8.0f;
		
		// Draw ring lines
		DrawLine(CenterX - RingSize, CenterY, CenterX - DotSize * 2, CenterY, FColor::Red, 1.0f);
		DrawLine(CenterX + DotSize * 2, CenterY, CenterX + RingSize, CenterY, FColor::Red, 1.0f);
		DrawLine(CenterX, CenterY - RingSize, CenterX, CenterY - DotSize * 2, FColor::Red, 1.0f);
		DrawLine(CenterX, CenterY + DotSize * 2, CenterX, CenterY + RingSize, FColor::Red, 1.0f);
	}
}

void ACybersoulsHUD::DrawStaminaBar()
{
	if (!PlayerCyberState)
	{
		return;
	}

	UPlayerCyberStateAttributeComponent* Attributes = PlayerCyberState->FindComponentByClass<UPlayerCyberStateAttributeComponent>();
	if (!Attributes)
	{
		return;
	}

	float StaminaPercent = Attributes->GetStaminaPercentage();
	
	// Draw stamina bar
	float BarWidth = 300.0f;
	float BarHeight = 20.0f;
	float BarX = 50.0f;
	float BarY = 50.0f;

	// Background
	DrawRect(FLinearColor::Black, BarX, BarY, BarWidth, BarHeight);

	// Stamina fill
	FLinearColor StaminaColor = StaminaPercent > 0.5f ? FLinearColor::Blue : 
								StaminaPercent > 0.25f ? FLinearColor::Yellow : FLinearColor::Red;
	DrawRect(StaminaColor, BarX, BarY, BarWidth * StaminaPercent, BarHeight);

	// Text
	FString StaminaText = FString::Printf(TEXT("Stamina: %.0f/%.0f"), Attributes->GetCurrentStamina(), Attributes->MaxStamina);
	DrawText(StaminaText, FColor::White, BarX, BarY - 20, HUDFont);
}

void ACybersoulsHUD::DrawDashCharges()
{
	if (!PlayerCyberState)
	{
		return;
	}

	UDashAbilityComponent* DashComponent = PlayerCyberState->FindComponentByClass<UDashAbilityComponent>();
	if (!DashComponent)
	{
		return;
	}

	// Position below stamina bar
	float StartX = 50.0f;
	float StartY = 90.0f;
	float ChargeBoxSize = 30.0f;
	float ChargeSpacing = 10.0f;

	// Draw dash charges label
	FString DashLabel = TEXT("Dash Charges:");
	DrawText(DashLabel, FColor::White, StartX, StartY, HUDFont);

	// Draw charge boxes
	int32 CurrentCharges = DashComponent->GetCurrentCharges();
	int32 MaxCharges = DashComponent->GetMaxCharges();
	
	for (int32 i = 0; i < MaxCharges; i++)
	{
		float BoxX = StartX + 120.0f + (i * (ChargeBoxSize + ChargeSpacing));
		float BoxY = StartY - 5.0f;

		// Draw box background
		DrawRect(FLinearColor::Black, BoxX, BoxY, ChargeBoxSize, ChargeBoxSize);

		// Draw charge fill
		if (i < CurrentCharges)
		{
			// Full charge - green
			DrawRect(FLinearColor::Green, BoxX + 2, BoxY + 2, ChargeBoxSize - 4, ChargeBoxSize - 4);
		}
		else if (i == CurrentCharges && CurrentCharges < MaxCharges)
		{
			// Regenerating charge - show progress
			float RegenProgress = DashComponent->GetChargeRegenProgress();
			
			// Draw partial fill from bottom up
			float FillHeight = (ChargeBoxSize - 4) * RegenProgress;
			float FillY = BoxY + 2 + (ChargeBoxSize - 4 - FillHeight);
			
			DrawRect(FLinearColor::Yellow, BoxX + 2, FillY, ChargeBoxSize - 4, FillHeight);
			
			// Draw regen timer text
			float TimeRemaining = DashComponent->ChargeRegenTime - (DashComponent->ChargeRegenTimer);
			FString TimerText = FString::Printf(TEXT("%.1fs"), TimeRemaining);
			DrawText(TimerText, FColor::Yellow, BoxX + ChargeBoxSize + 5, BoxY + 5, HUDFont, 0.8f);
		}
		// Empty charges remain black
	}

	// Draw cooldown if in cooldown
	if (DashComponent->CurrentCooldown > 0.0f)
	{
		FString CooldownText = FString::Printf(TEXT("Cooldown: %.1fs"), DashComponent->CurrentCooldown);
		DrawText(CooldownText, FColor::Red, StartX + 300.0f, StartY, HUDFont);
	}
}

void ACybersoulsHUD::DrawCharacterIndicator()
{
	if (!Canvas || !IsValid(CyberSoulsController))
	{
		return;
	}

	// Position indicator in top-right corner
	float IndicatorWidth = 250.0f;
	float IndicatorHeight = 80.0f;
	float IndicatorX = Canvas->SizeX - IndicatorWidth - 20.0f;
	float IndicatorY = 20.0f;

	// Determine current character state
	bool bIsUsingCyberState = CyberSoulsController->IsUsingCyberState();
	
	// Background with cyberpunk styling
	FLinearColor BGColor = bIsUsingCyberState ? 
		FLinearColor(0.0f, 0.3f, 0.8f, 0.9f) :  // Blue for cyber state
		FLinearColor(0.8f, 0.3f, 0.0f, 0.9f);   // Orange for default
	
	DrawRect(BGColor, IndicatorX, IndicatorY, IndicatorWidth, IndicatorHeight);
	
	// Border
	FLinearColor BorderColor = bIsUsingCyberState ? 
		FLinearColor(0.2f, 0.8f, 1.0f, 1.0f) :  // Cyan for cyber state
		FLinearColor(1.0f, 0.6f, 0.0f, 1.0f);   // Bright orange for default
	
	float BorderThickness = 3.0f;
	DrawRect(BorderColor, IndicatorX, IndicatorY, IndicatorWidth, BorderThickness); // Top
	DrawRect(BorderColor, IndicatorX, IndicatorY, BorderThickness, IndicatorHeight); // Left
	DrawRect(BorderColor, IndicatorX + IndicatorWidth - BorderThickness, IndicatorY, BorderThickness, IndicatorHeight); // Right
	DrawRect(BorderColor, IndicatorX, IndicatorY + IndicatorHeight - BorderThickness, IndicatorWidth, BorderThickness); // Bottom

	// Character name and status
	FString CharacterName = bIsUsingCyberState ? TEXT("▰▰ CYBER STATE ▰▰") : TEXT("▰▰ DEFAULT MODE ▰▰");
	FString StatusText = bIsUsingCyberState ? TEXT("Enhanced Mobility") : TEXT("Combat Specialist");
	FString SwitchHint = TEXT("Press [F] to Switch");
	
	FColor TextColor = bIsUsingCyberState ? FColor::Cyan : FColor::Yellow;
	
	// Draw texts
	float TextY = IndicatorY + 8.0f;
	DrawText(CharacterName, TextColor, IndicatorX + 10.0f, TextY, HUDFont, 1.1f);
	DrawText(StatusText, FColor::White, IndicatorX + 10.0f, TextY + 25.0f, HUDFont, 0.8f);
	DrawText(SwitchHint, FColor::Green, IndicatorX + 10.0f, TextY + 45.0f, HUDFont, 0.7f);
}

void ACybersoulsHUD::DrawSwitchNotification()
{
	if (!bShowSwitchNotification || !Canvas)
	{
		return;
	}

	// Update timer
	SwitchNotificationTimer -= GetWorld()->GetDeltaSeconds();
	if (SwitchNotificationTimer <= 0.0f)
	{
		bShowSwitchNotification = false;
		return;
	}

	// Position notification in center of screen
	float NotificationWidth = 400.0f;
	float NotificationHeight = 100.0f;
	float NotificationX = (Canvas->SizeX - NotificationWidth) * 0.5f;
	float NotificationY = (Canvas->SizeY - NotificationHeight) * 0.3f; // Upper center

	// Fade effect based on remaining time
	float Alpha = FMath::Clamp(SwitchNotificationTimer / SWITCH_NOTIFICATION_DURATION, 0.0f, 1.0f);
	
	// Pulsing effect
	float PulseScale = 1.0f + 0.1f * FMath::Sin(GetWorld()->GetTimeSeconds() * 8.0f);
	
	// Background
	FLinearColor BGColor = FLinearColor(0.1f, 0.1f, 0.1f, 0.8f * Alpha);
	DrawRect(BGColor, NotificationX, NotificationY, NotificationWidth, NotificationHeight);
	
	// Animated border
	FLinearColor BorderColor = FLinearColor(0.0f, 1.0f, 1.0f, Alpha); // Cyan
	float BorderThickness = 4.0f * PulseScale;
	
	DrawRect(BorderColor, NotificationX, NotificationY, NotificationWidth, BorderThickness); // Top
	DrawRect(BorderColor, NotificationX, NotificationY, BorderThickness, NotificationHeight); // Left
	DrawRect(BorderColor, NotificationX + NotificationWidth - BorderThickness, NotificationY, BorderThickness, NotificationHeight); // Right
	DrawRect(BorderColor, NotificationX, NotificationY + NotificationHeight - BorderThickness, NotificationWidth, BorderThickness); // Bottom

	// Text
	FColor TextColor = FColor(255, 255, 255, (uint8)(255 * Alpha));
	float TextScale = 1.5f * PulseScale;
	
	// Center the text
	float TextX = NotificationX + (NotificationWidth - 250.0f) * 0.5f; // Approximate text width
	float TextY = NotificationY + (NotificationHeight - 30.0f) * 0.5f; // Approximate text height
	
	DrawText(SwitchNotificationText, TextColor, TextX, TextY, HUDFont, TextScale);
}

void ACybersoulsHUD::ShowCharacterSwitchNotification(const FString& Text)
{
	SwitchNotificationText = Text;
	SwitchNotificationTimer = SWITCH_NOTIFICATION_DURATION;
	bShowSwitchNotification = true;
}

void ACybersoulsHUD::DrawDeathScreen()
{
	if (!bShowDeathScreen) return;
	
	// Draw dark overlay
	DrawRect(FLinearColor(0, 0, 0, 0.8f), 0, 0, Canvas->SizeX, Canvas->SizeY);
	
	// Draw death message
	float CenterX = Canvas->SizeX * 0.5f;
	float CenterY = Canvas->SizeY * 0.4f;
	
	FString DeathMessage = TEXT("SYSTEM COMPROMISED");
	DrawText(DeathMessage, FColor::Red, CenterX - 150.0f, CenterY - 50.0f, HUDFont, 2.0f);
	
	FString HackMessage = TEXT("Hack Progress reached 100%");
	DrawText(HackMessage, FColor::White, CenterX - 120.0f, CenterY, HUDFont, 1.0f);
	
	// Draw restart instruction
	FString RestartMessage = TEXT("Press ENTER to Start Again");
	DrawText(RestartMessage, FColor::Yellow, CenterX - 120.0f, CenterY + 100.0f, HUDFont, 1.2f);
	
	FString WarningMessage = TEXT("(XP will be reset)");
	DrawText(WarningMessage, FColor(200, 200, 200), CenterX - 80.0f, CenterY + 130.0f, HUDFont, 0.8f);
}

void ACybersoulsHUD::DrawPlayAgainButton()
{
	if (!bShowPlayAgainButton || !bShowXPDisplay) return;
	
	float CenterX = Canvas->SizeX * 0.5f;
	float ButtonY = Canvas->SizeY * 0.65f;
	
	// Draw button background
	float ButtonWidth = 300.0f;
	float ButtonHeight = 60.0f;
	float ButtonX = CenterX - ButtonWidth * 0.5f;
	
	// Button background with border
	DrawRect(FLinearColor(0, 0.3f, 0, 0.7f), ButtonX, ButtonY, ButtonWidth, ButtonHeight);
	
	// Border
	float BorderThickness = 3.0f;
	FLinearColor BorderColor = FLinearColor(0, 1, 0, 1);
	DrawRect(BorderColor, ButtonX, ButtonY, ButtonWidth, BorderThickness); // Top
	DrawRect(BorderColor, ButtonX, ButtonY, BorderThickness, ButtonHeight); // Left
	DrawRect(BorderColor, ButtonX + ButtonWidth - BorderThickness, ButtonY, BorderThickness, ButtonHeight); // Right
	DrawRect(BorderColor, ButtonX, ButtonY + ButtonHeight - BorderThickness, ButtonWidth, BorderThickness); // Bottom
	
	// Button text
	FString ButtonText = TEXT("Play Again?");
	DrawText(ButtonText, FColor::White, CenterX - 50.0f, ButtonY + 20.0f, HUDFont, 1.5f);
	
	// Instruction
	FString InstructionText = TEXT("Press ENTER to continue with current XP");
	DrawText(InstructionText, FColor(200, 255, 200), CenterX - 150.0f, ButtonY + ButtonHeight + 20.0f, HUDFont, 1.0f);
}

void ACybersoulsHUD::ToggleInventoryDisplay()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("ToggleInventoryDisplay: No PlayerController found"));
		return;
	}

	// Check if inventory is currently open (use bShowInventoryDisplay as the source of truth)
	UE_LOG(LogTemp, Warning, TEXT("ToggleInventoryDisplay: bShowInventoryDisplay=%s, InventoryWidget=%s"), 
		bShowInventoryDisplay ? TEXT("true") : TEXT("false"),
		(InventoryWidget && IsValid(InventoryWidget)) ? TEXT("valid") : TEXT("null"));
	
	if (bShowInventoryDisplay || (InventoryWidget && IsValid(InventoryWidget)))
	{
		// Force close inventory
		ForceCloseInventory();
		UE_LOG(LogTemp, Warning, TEXT("Inventory closed"));
		return;
	}

	// Check if we're trying to open but no widget class set
	if (!InventoryWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidgetClass not set in BP_CybersoulsHUD! Please set it in Blueprint."));
		UE_LOG(LogTemp, Error, TEXT("To fix: Open BP_CybersoulsHUD in Blueprint editor and set InventoryWidgetClass to your inventory widget"));
		
		// Create a simple test widget to verify the system works
		UE_LOG(LogTemp, Error, TEXT("Creating TEST widget to verify system..."));
		
		// Use UserWidget base class for testing
		InventoryWidget = CreateWidget<UInventoryWidget>(PC, UInventoryWidget::StaticClass());
		if (!InventoryWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create widget even with StaticClass!"));
			// Restore game controls since we can't create widget
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			return;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Created fallback inventory widget programmatically"));
	}

	// Try to open inventory
	UE_LOG(LogTemp, Warning, TEXT("Attempting to create inventory widget..."));
	
	InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
	if (!InventoryWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create inventory widget from class"));
		
		// Restore game controls since widget creation failed
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		return;
	}

	// Initialize with player's QuickHack manager
	if (PlayerCharacter && PlayerCharacter->GetQuickHackManager())
	{
		InventoryWidget->InitializeInventory(PlayerCharacter->GetQuickHackManager());
		UE_LOG(LogTemp, Warning, TEXT("Inventory initialized with QuickHack manager"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerCharacter or QuickHackManager found"));
	}
	
	// Add to viewport with proper size and centering
	InventoryWidget->AddToViewport(999); // Very high Z-order to ensure it's on top
	InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	InventoryWidget->SetDesiredSizeInViewport(FVector2D(800.0f, 600.0f));
	InventoryWidget->SetPositionInViewport(FVector2D(0.0f, 0.0f), false);
	InventoryWidget->SetAnchorsInViewport(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
	InventoryWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	bShowInventoryDisplay = true;
	
	// Force widget to be interactive
	InventoryWidget->SetUserFocus(PC);
	
	UE_LOG(LogTemp, Warning, TEXT("Inventory widget visibility: %s"), 
		*UEnum::GetValueAsString(InventoryWidget->GetVisibility()));
	
	// Enable mouse cursor and input
	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
	
	// Use GameAndUI mode for better compatibility
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	PC->SetInputMode(InputMode);
	
	UE_LOG(LogTemp, Warning, TEXT("Inventory widget added to viewport successfully with mouse controls"));
}

void ACybersoulsHUD::DrawCanvasInventory()
{
	if (!Canvas) return;
	
	// Get screen dimensions
	float ScreenWidth = Canvas->SizeX;
	float ScreenHeight = Canvas->SizeY;
	
	// Calculate inventory panel dimensions
	float PanelWidth = 800.0f;
	float PanelHeight = 600.0f;
	float PanelX = (ScreenWidth - PanelWidth) * 0.5f;
	float PanelY = (ScreenHeight - PanelHeight) * 0.5f;
	
	// Draw background
	DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f), PanelX, PanelY, PanelWidth, PanelHeight);
	
	// Draw border
	float BorderThickness = 4.0f;
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), PanelX, PanelY, PanelWidth, BorderThickness); // Top
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), PanelX, PanelY, BorderThickness, PanelHeight); // Left
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), PanelX + PanelWidth - BorderThickness, PanelY, BorderThickness, PanelHeight); // Right
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 1.0f), PanelX, PanelY + PanelHeight - BorderThickness, PanelWidth, BorderThickness); // Bottom
	
	float CurrentY = PanelY + 20.0f;
	float LineHeight = 30.0f;
	
	// Title
	DrawText(TEXT("▰▰ CYBERSOULS INVENTORY ▰▰"), FColor::Cyan, PanelX + 50.0f, CurrentY, HUDFont, 1.5f);
	CurrentY += LineHeight * 1.5f;
	
	// XP Display
	APlayerController* PC = GetOwningPlayerController();
	if (PC && PC->GetPawn())
	{
		UPlayerProgressionComponent* Progression = PC->GetPawn()->FindComponentByClass<UPlayerProgressionComponent>();
		if (Progression)
		{
			FString IntegrityXPText = FString::Printf(TEXT("⚡ INTEGRITY XP: %.0f"), Progression->GetIntegrityXP());
			FString HackingXPText = FString::Printf(TEXT("🔧 HACKING XP: %.0f"), Progression->GetHackingXP());
			
			DrawText(IntegrityXPText, FColor::Green, PanelX + 50.0f, CurrentY, HUDFont, 1.2f);
			CurrentY += LineHeight;
			DrawText(HackingXPText, FColor::Blue, PanelX + 50.0f, CurrentY, HUDFont, 1.2f);
			CurrentY += LineHeight * 1.5f;
		}
	}
	
	// QuickHacks Section
	DrawText(TEXT("▰ QUICKHACKS (Click to change)"), FColor::Yellow, PanelX + 50.0f, CurrentY, HUDFont, 1.3f);
	CurrentY += LineHeight;
	
	// Draw equipped QuickHacks with slot backgrounds
	for (int32 i = 0; i < 4; i++)
	{
		float SlotY = CurrentY + (i * LineHeight * 0.8f);
		float SlotHeight = LineHeight * 0.8f;
		
		// Draw slot background (darker for clickable area)
		DrawRect(FLinearColor(0.2f, 0.2f, 0.3f, 0.7f), PanelX + 70.0f, SlotY, PanelWidth - 120.0f, SlotHeight);
		
		// Draw slot border
		float SlotBorderThickness = 1.0f;
		DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 0.8f), PanelX + 70.0f, SlotY, PanelWidth - 120.0f, SlotBorderThickness);
		DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 0.8f), PanelX + 70.0f, SlotY + SlotHeight - SlotBorderThickness, PanelWidth - 120.0f, SlotBorderThickness);
		DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 0.8f), PanelX + 70.0f, SlotY, SlotBorderThickness, SlotHeight);
		DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 0.8f), PanelX + PanelWidth - 50.0f - SlotBorderThickness, SlotY, SlotBorderThickness, SlotHeight);
		
		FString QuickHackText = FString::Printf(TEXT("%d. %s"), i + 1, *GetEquippedQuickHackName(i));
		DrawText(QuickHackText, FColor::White, PanelX + 80.0f, SlotY + 5.0f, HUDFont);
	}
	CurrentY += 4 * LineHeight * 0.8f;
	
	CurrentY += LineHeight * 0.5f;
	
	// Passive Abilities Section
	DrawText(TEXT("▰ PASSIVE ABILITIES (Click to change)"), FColor::Magenta, PanelX + 50.0f, CurrentY, HUDFont, 1.3f);
	CurrentY += LineHeight;
	
	// Draw equipped Passives with slot backgrounds
	for (int32 i = 0; i < 4; i++)
	{
		float SlotY = CurrentY + (i * LineHeight * 0.8f);
		float SlotHeight = LineHeight * 0.8f;
		
		// Draw slot background (darker for clickable area)
		DrawRect(FLinearColor(0.3f, 0.2f, 0.3f, 0.7f), PanelX + 70.0f, SlotY, PanelWidth - 120.0f, SlotHeight);
		
		// Draw slot border
		float PassiveBorderThickness = 1.0f;
		DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 0.8f), PanelX + 70.0f, SlotY, PanelWidth - 120.0f, PassiveBorderThickness);
		DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 0.8f), PanelX + 70.0f, SlotY + SlotHeight - PassiveBorderThickness, PanelWidth - 120.0f, PassiveBorderThickness);
		DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 0.8f), PanelX + 70.0f, SlotY, PassiveBorderThickness, SlotHeight);
		DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 0.8f), PanelX + PanelWidth - 50.0f - PassiveBorderThickness, SlotY, PassiveBorderThickness, SlotHeight);
		
		FString PassiveText = FString::Printf(TEXT("%d. %s"), i + 1, *GetEquippedPassiveName(i));
		DrawText(PassiveText, FColor::White, PanelX + 80.0f, SlotY + 5.0f, HUDFont);
	}
	
	// Instructions
	DrawText(TEXT("Press Tab to close | Click slots for dropdown menu"), FColor::Orange, PanelX + 50.0f, PanelY + PanelHeight - 40.0f, HUDFont);
	
	// Enable mouse cursor when inventory is shown
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && !PlayerController->bShowMouseCursor)
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
		
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);
	}
	
	// Handle mouse clicks
	if (PlayerController)
	{
		float MouseX, MouseY;
		PlayerController->GetMousePosition(MouseX, MouseY);
		
		// Check for mouse click
		bool bCurrentMousePressed = PlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
		if (bCurrentMousePressed && !bMouseButtonPressed)
		{
			// Mouse button just pressed
			HandleInventoryMouseClick(MouseX, MouseY);
		}
		bMouseButtonPressed = bCurrentMousePressed;
	}
	
	// Draw dropdowns on top of everything else
	DrawQuickHackDropdown(PanelX, PanelWidth);
	DrawPassiveDropdown(PanelX, PanelWidth);
}

void ACybersoulsHUD::ForceCloseInventory()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		return;
	}

	// Force close any inventory widget
	if (InventoryWidget && IsValid(InventoryWidget))
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
	}

	// Reset the display flag
	bShowInventoryDisplay = false;
	
	// Close any open dropdowns
	CloseAllDropdowns();

	// Restore controls based on player controller settings
	if (ACyberSoulsPlayerController* CyberPC = Cast<ACyberSoulsPlayerController>(PC))
	{
		if (CyberPC->bEnableMouseInGameplay)
		{
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;
			
			if (CyberPC->bUseGameAndUIInputMode)
			{
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
				InputMode.SetHideCursorDuringCapture(false);
				PC->SetInputMode(InputMode);
			}
			else
			{
				FInputModeGameOnly InputMode;
				PC->SetInputMode(InputMode);
			}
		}
		else
		{
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
	}
	else
	{
		// Fallback to default game-only mode
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ForceCloseInventory: Game controls restored"));
}

void ACybersoulsHUD::InitializeAvailableAbilities()
{
	// Initialize available QuickHacks based on CLAUDE.md specifications
	AvailableQuickHacks.Empty();
	AvailableQuickHacks.Add(TEXT("Cascade Virus"));
	AvailableQuickHacks.Add(TEXT("Ghost Protocol"));
	AvailableQuickHacks.Add(TEXT("Charge Drain"));
	AvailableQuickHacks.Add(TEXT("Gravity Flip"));
	AvailableQuickHacks.Add(TEXT("System Shock"));
	AvailableQuickHacks.Add(TEXT("Neural Disrupt"));
	AvailableQuickHacks.Add(TEXT("Data Breach"));
	AvailableQuickHacks.Add(TEXT("Memory Wipe"));
	
	// Initialize available passive abilities
	AvailablePassives.Empty();
	AvailablePassives.Add(TEXT("Execution Chains"));
	AvailablePassives.Add(TEXT("System Overcharge"));
	AvailablePassives.Add(TEXT("Neural Buffer"));
	AvailablePassives.Add(TEXT("Cyber Resilience"));
	AvailablePassives.Add(TEXT("Data Recovery"));
	AvailablePassives.Add(TEXT("Signal Boost"));
	
	// Initialize equipped abilities (start with first 4 QuickHacks and Passives)
	EquippedQuickHackIndices.SetNum(4);
	for (int32 i = 0; i < 4; i++)
	{
		EquippedQuickHackIndices[i] = i; // Start with first 4 QuickHacks
	}
	
	EquippedPassiveIndices.SetNum(4);
	for (int32 i = 0; i < 4; i++)
	{
		EquippedPassiveIndices[i] = i; // Start with first 4 Passives
	}
}

void ACybersoulsHUD::HandleInventoryMouseClick(float MouseX, float MouseY)
{
	if (!bShowInventoryDisplay || !Canvas) return;
	
	// Check if clicking on dropdown items first
	if (bShowQuickHackDropdown || bShowPassiveDropdown)
	{
		HandleDropdownSelection(MouseX, MouseY);
		return;
	}
	
	// Calculate inventory panel dimensions (same as in DrawCanvasInventory)
	float ScreenWidth = Canvas->SizeX;
	float ScreenHeight = Canvas->SizeY;
	float PanelWidth = 800.0f;
	float PanelHeight = 600.0f;
	float PanelX = (ScreenWidth - PanelWidth) * 0.5f;
	float PanelY = (ScreenHeight - PanelHeight) * 0.5f;
	
	// Calculate QuickHack section position (MUST match DrawCanvasInventory exactly)
	float CurrentY = PanelY + 20.0f; // Start position
	float LineHeight = 30.0f;
	
	// Skip title
	CurrentY += LineHeight * 1.5f;
	
	// Skip XP section - check if we have progression component
	APlayerController* PC = GetOwningPlayerController();
	if (PC && PC->GetPawn())
	{
		UPlayerProgressionComponent* Progression = PC->GetPawn()->FindComponentByClass<UPlayerProgressionComponent>();
		if (Progression)
		{
			CurrentY += LineHeight; // Integrity XP
			CurrentY += LineHeight; // Hacking XP  
			CurrentY += LineHeight * 1.5f; // Extra spacing
		}
	}
	
	// Skip QuickHacks title
	CurrentY += LineHeight;
	
	// Check if click is in QuickHack slots area
	float SlotHeight = LineHeight * 0.8f;
	for (int32 i = 0; i < 4; i++)
	{
		float SlotY = CurrentY + (i * SlotHeight);
		if (MouseY >= SlotY && MouseY <= SlotY + SlotHeight &&
			MouseX >= PanelX + 70.0f && MouseX <= PanelX + PanelWidth - 50.0f)
		{
			OpenQuickHackDropdown(i, SlotY);
			UE_LOG(LogTemp, Warning, TEXT("Opened QuickHack dropdown for slot %d at Y=%f"), i + 1, SlotY);
			return;
		}
	}
	
	// Calculate Passive section position
	CurrentY += (4 * SlotHeight); // QuickHacks slots
	CurrentY += LineHeight * 0.5f; // Spacing after QuickHacks
	CurrentY += LineHeight; // Passives title
	
	// Check if click is in Passive slots area
	for (int32 i = 0; i < 4; i++)
	{
		float SlotY = CurrentY + (i * SlotHeight);
		if (MouseY >= SlotY && MouseY <= SlotY + SlotHeight &&
			MouseX >= PanelX + 70.0f && MouseX <= PanelX + PanelWidth - 50.0f)
		{
			OpenPassiveDropdown(i, SlotY);
			UE_LOG(LogTemp, Warning, TEXT("Opened Passive dropdown for slot %d at Y=%f"), i + 1, SlotY);
			return;
		}
	}
}

void ACybersoulsHUD::OpenQuickHackDropdown(int32 SlotIndex, float SlotY)
{
	CloseAllDropdowns(); // Close any existing dropdowns
	
	bShowQuickHackDropdown = true;
	ActiveDropdownSlot = SlotIndex;
	DropdownStartY = SlotY + 25.0f; // Start dropdown below the slot
	
	UE_LOG(LogTemp, Warning, TEXT("Opened QuickHack dropdown for slot %d"), SlotIndex + 1);
}

void ACybersoulsHUD::OpenPassiveDropdown(int32 SlotIndex, float SlotY)
{
	CloseAllDropdowns(); // Close any existing dropdowns
	
	bShowPassiveDropdown = true;
	ActiveDropdownSlot = SlotIndex;
	DropdownStartY = SlotY + 25.0f; // Start dropdown below the slot
	
	UE_LOG(LogTemp, Warning, TEXT("Opened Passive dropdown for slot %d"), SlotIndex + 1);
}

void ACybersoulsHUD::CloseAllDropdowns()
{
	bShowQuickHackDropdown = false;
	bShowPassiveDropdown = false;
	ActiveDropdownSlot = -1;
}

void ACybersoulsHUD::HandleDropdownSelection(float MouseX, float MouseY)
{
	if (!Canvas) return;
	
	float ScreenWidth = Canvas->SizeX;
	float PanelWidth = 800.0f;
	float PanelX = (ScreenWidth - PanelWidth) * 0.5f;
	
	if (bShowQuickHackDropdown)
	{
		// Check if click is within QuickHack dropdown area
		float DropdownWidth = PanelWidth - 120.0f;
		float DropdownHeight = AvailableQuickHacks.Num() * DropdownItemHeight;
		
		if (MouseX >= PanelX + 70.0f && MouseX <= PanelX + 70.0f + DropdownWidth &&
			MouseY >= DropdownStartY && MouseY <= DropdownStartY + DropdownHeight)
		{
			// Calculate which item was clicked
			int32 ClickedIndex = (MouseY - DropdownStartY) / DropdownItemHeight;
			if (ClickedIndex >= 0 && ClickedIndex < AvailableQuickHacks.Num())
			{
				// Set the selected QuickHack
				if (ActiveDropdownSlot >= 0 && ActiveDropdownSlot < EquippedQuickHackIndices.Num())
				{
					EquippedQuickHackIndices[ActiveDropdownSlot] = ClickedIndex;
					UE_LOG(LogTemp, Warning, TEXT("QuickHack Slot %d changed to: %s"), 
						ActiveDropdownSlot + 1, *AvailableQuickHacks[ClickedIndex]);
				}
			}
		}
		CloseAllDropdowns();
	}
	else if (bShowPassiveDropdown)
	{
		// Check if click is within Passive dropdown area
		float DropdownWidth = PanelWidth - 120.0f;
		float DropdownHeight = AvailablePassives.Num() * DropdownItemHeight;
		
		if (MouseX >= PanelX + 70.0f && MouseX <= PanelX + 70.0f + DropdownWidth &&
			MouseY >= DropdownStartY && MouseY <= DropdownStartY + DropdownHeight)
		{
			// Calculate which item was clicked
			int32 ClickedIndex = (MouseY - DropdownStartY) / DropdownItemHeight;
			if (ClickedIndex >= 0 && ClickedIndex < AvailablePassives.Num())
			{
				// Set the selected Passive
				if (ActiveDropdownSlot >= 0 && ActiveDropdownSlot < EquippedPassiveIndices.Num())
				{
					EquippedPassiveIndices[ActiveDropdownSlot] = ClickedIndex;
					UE_LOG(LogTemp, Warning, TEXT("Passive Slot %d changed to: %s"), 
						ActiveDropdownSlot + 1, *AvailablePassives[ClickedIndex]);
				}
			}
		}
		// If we reach here, the click was outside the dropdown - close it
		CloseAllDropdowns();
	}
}

FString ACybersoulsHUD::GetEquippedQuickHackName(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= EquippedQuickHackIndices.Num()) return TEXT("Empty");
	
	int32 QuickHackIndex = EquippedQuickHackIndices[SlotIndex];
	if (QuickHackIndex < 0 || QuickHackIndex >= AvailableQuickHacks.Num()) return TEXT("Empty");
	
	return AvailableQuickHacks[QuickHackIndex];
}

FString ACybersoulsHUD::GetEquippedPassiveName(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= EquippedPassiveIndices.Num()) return TEXT("Empty");
	
	int32 PassiveIndex = EquippedPassiveIndices[SlotIndex];
	if (PassiveIndex < 0 || PassiveIndex >= AvailablePassives.Num()) return TEXT("Empty");
	
	return AvailablePassives[PassiveIndex];
}

void ACybersoulsHUD::DrawQuickHackDropdown(float PanelX, float PanelWidth)
{
	if (!bShowQuickHackDropdown || !Canvas) return;
	
	float DropdownWidth = PanelWidth - 120.0f;
	float DropdownHeight = AvailableQuickHacks.Num() * DropdownItemHeight;
	float DropdownX = PanelX + 70.0f;
	
	// Draw dropdown background
	DrawRect(FLinearColor(0.15f, 0.15f, 0.2f, 0.95f), DropdownX, DropdownStartY, DropdownWidth, DropdownHeight);
	
	// Draw dropdown border
	float BorderThickness = 2.0f;
	DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 1.0f), DropdownX, DropdownStartY, DropdownWidth, BorderThickness);
	DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 1.0f), DropdownX, DropdownStartY + DropdownHeight - BorderThickness, DropdownWidth, BorderThickness);
	DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 1.0f), DropdownX, DropdownStartY, BorderThickness, DropdownHeight);
	DrawRect(FLinearColor(0.4f, 0.6f, 0.8f, 1.0f), DropdownX + DropdownWidth - BorderThickness, DropdownStartY, BorderThickness, DropdownHeight);
	
	// Draw each QuickHack option
	for (int32 i = 0; i < AvailableQuickHacks.Num(); i++)
	{
		float ItemY = DropdownStartY + (i * DropdownItemHeight);
		
		// Highlight currently equipped QuickHack
		bool bIsEquipped = (ActiveDropdownSlot >= 0 && ActiveDropdownSlot < EquippedQuickHackIndices.Num() && 
							EquippedQuickHackIndices[ActiveDropdownSlot] == i);
		
		if (bIsEquipped)
		{
			DrawRect(FLinearColor(0.3f, 0.5f, 0.7f, 0.6f), DropdownX + 2.0f, ItemY, DropdownWidth - 4.0f, DropdownItemHeight);
		}
		
		// Draw QuickHack name
		FColor TextColor = bIsEquipped ? FColor::Yellow : FColor::White;
		DrawText(AvailableQuickHacks[i], TextColor, DropdownX + 10.0f, ItemY + 3.0f, HUDFont, 0.9f);
	}
}

void ACybersoulsHUD::DrawPassiveDropdown(float PanelX, float PanelWidth)
{
	if (!bShowPassiveDropdown || !Canvas) return;
	
	float DropdownWidth = PanelWidth - 120.0f;
	float DropdownHeight = AvailablePassives.Num() * DropdownItemHeight;
	float DropdownX = PanelX + 70.0f;
	
	// Draw dropdown background
	DrawRect(FLinearColor(0.2f, 0.15f, 0.2f, 0.95f), DropdownX, DropdownStartY, DropdownWidth, DropdownHeight);
	
	// Draw dropdown border
	float BorderThickness = 2.0f;
	DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 1.0f), DropdownX, DropdownStartY, DropdownWidth, BorderThickness);
	DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 1.0f), DropdownX, DropdownStartY + DropdownHeight - BorderThickness, DropdownWidth, BorderThickness);
	DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 1.0f), DropdownX, DropdownStartY, BorderThickness, DropdownHeight);
	DrawRect(FLinearColor(0.8f, 0.4f, 0.8f, 1.0f), DropdownX + DropdownWidth - BorderThickness, DropdownStartY, BorderThickness, DropdownHeight);
	
	// Draw each Passive option
	for (int32 i = 0; i < AvailablePassives.Num(); i++)
	{
		float ItemY = DropdownStartY + (i * DropdownItemHeight);
		
		// Highlight currently equipped Passive
		bool bIsEquipped = (ActiveDropdownSlot >= 0 && ActiveDropdownSlot < EquippedPassiveIndices.Num() && 
							EquippedPassiveIndices[ActiveDropdownSlot] == i);
		
		if (bIsEquipped)
		{
			DrawRect(FLinearColor(0.7f, 0.3f, 0.7f, 0.6f), DropdownX + 2.0f, ItemY, DropdownWidth - 4.0f, DropdownItemHeight);
		}
		
		// Draw Passive name
		FColor TextColor = bIsEquipped ? FColor::Yellow : FColor::White;
		DrawText(AvailablePassives[i], TextColor, DropdownX + 10.0f, ItemY + 3.0f, HUDFont, 0.9f);
	}
}
