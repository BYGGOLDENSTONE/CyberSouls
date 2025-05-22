// CybersoulsHUD.cpp
#include "cybersouls/Public/UI/CybersoulsHUD.h"
#include "cybersouls/Public/Character/cybersoulsCharacter.h"
#include "cybersouls/Public/Attributes/PlayerAttributeComponent.h"
#include "cybersouls/Public/Attributes/PlayerProgressionComponent.h"
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
	
	// Initialize XP display to hidden
	bShowXPDisplay = false;
}

void ACybersoulsHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AcybersoulsCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void ACybersoulsHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!PlayerCharacter || !Canvas)
	{
		return;
	}

	DrawIntegrityBar();
	DrawHackProgressBar();
	DrawQuickHackStatus();
	DrawTargetInfo();
	DrawTargetBodyPartIndicator();
	DrawXPDisplay();
	DrawCrosshair();
	// Removed DrawEnemyQuickHackCasting - no longer needed
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
	// Get QuickHack components directly
	float StartY = 150.0f;
	float LineHeight = 25.0f;

	// InterruptProtocol
	if (PlayerCharacter->GetInterruptProtocolAbility())
	{
		UQuickHackComponent* InterruptProtocol = PlayerCharacter->GetInterruptProtocolAbility();
		FString StatusText = TEXT("1. Interrupt Protocol");
		float Cooldown = InterruptProtocol->CurrentCooldown;
		
		if (InterruptProtocol->IsQuickHackActive())
		{
			// Show casting progress
			float CastPercent = InterruptProtocol->CurrentCastTime / InterruptProtocol->CastTime;
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

		FColor TextColor = InterruptProtocol->IsQuickHackActive() ? FColor::Yellow : (Cooldown > 0.0f ? FColor::Red : FColor::Green);
		DrawText(StatusText, TextColor, 50, StartY, HUDFont);
	}

	// SystemFreeze
	if (PlayerCharacter->GetSystemFreezeAbility())
	{
		UQuickHackComponent* SystemFreeze = PlayerCharacter->GetSystemFreezeAbility();
		FString StatusText = TEXT("2. System Freeze");
		float Cooldown = SystemFreeze->CurrentCooldown;
		
		if (SystemFreeze->IsQuickHackActive())
		{
			// Show casting progress
			float CastPercent = SystemFreeze->CurrentCastTime / SystemFreeze->CastTime;
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

		FColor TextColor = SystemFreeze->IsQuickHackActive() ? FColor::Yellow : (Cooldown > 0.0f ? FColor::Red : FColor::Green);
		DrawText(StatusText, TextColor, 50, StartY + LineHeight, HUDFont);
	}

	// Firewall
	if (PlayerCharacter->GetFirewallAbility())
	{
		UQuickHackComponent* Firewall = PlayerCharacter->GetFirewallAbility();
		FString StatusText = TEXT("3. Firewall");
		float Cooldown = Firewall->CurrentCooldown;
		
		if (Firewall->IsQuickHackActive())
		{
			// Show casting progress
			float CastPercent = Firewall->CurrentCastTime / Firewall->CastTime;
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

		FColor TextColor = Firewall->IsQuickHackActive() ? FColor::Yellow : (Cooldown > 0.0f ? FColor::Red : FColor::Green);
		DrawText(StatusText, TextColor, 50, StartY + 2 * LineHeight, HUDFont);
	}
	
	// Kill
	if (PlayerCharacter->GetKillAbility())
	{
		UQuickHackComponent* Kill = PlayerCharacter->GetKillAbility();
		FString StatusText = TEXT("4. Kill");
		float Cooldown = Kill->CurrentCooldown;
		
		if (Kill->IsQuickHackActive())
		{
			// Show casting progress
			float CastPercent = Kill->CurrentCastTime / Kill->CastTime;
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

		FColor TextColor = Kill->IsQuickHackActive() ? FColor::Yellow : (Cooldown > 0.0f ? FColor::Red : FColor::Green);
		DrawText(StatusText, TextColor, 50, StartY + 3 * LineHeight, HUDFont);
	}
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
	
	UPlayerProgressionComponent* Progression = PlayerCharacter->GetPlayerProgression();
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
	bool bAimingAtEnemy = PlayerCharacter->GetCrosshairTarget() != nullptr;
	
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
