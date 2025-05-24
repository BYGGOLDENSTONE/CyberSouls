// CybersoulsHUD.cpp
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Character/PlayerCyberState.h"
#include "cybersouls/Public/Player/CyberSoulsPlayerController.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
#include "cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h"
#include "cybersouls/Public/Abilities/SlashAbilityComponent.h"
#include "cybersouls/Public/Abilities/QuickHackComponent.h"
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
	bShowDeathScreen = false;
	bShowPlayAgainButton = false;
	bShowSwitchNotification = false;
	SwitchNotificationTimer = 0.0f;
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
	float StartY = 150.0f;
	float LineHeight = 25.0f;

	// Draw each QuickHack using the helper method
	if (PlayerCharacter->GetInterruptProtocolAbility())
	{
		DrawQuickHackStatusLine(PlayerCharacter->GetInterruptProtocolAbility(), TEXT("Interrupt Protocol"), 1, StartY);
	}

	if (PlayerCharacter->GetSystemFreezeAbility())
	{
		DrawQuickHackStatusLine(PlayerCharacter->GetSystemFreezeAbility(), TEXT("System Freeze"), 2, StartY + LineHeight);
	}

	if (PlayerCharacter->GetFirewallAbility())
	{
		DrawQuickHackStatusLine(PlayerCharacter->GetFirewallAbility(), TEXT("Firewall"), 3, StartY + 2 * LineHeight);
	}
	
	if (PlayerCharacter->GetKillAbility())
	{
		DrawQuickHackStatusLine(PlayerCharacter->GetKillAbility(), TEXT("Kill"), 4, StartY + 3 * LineHeight);
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
