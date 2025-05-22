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
	UTargetLockComponent* TargetLock = PlayerCharacter->GetTargetLockComponent();
	if (!TargetLock || !TargetLock->IsLocked())
	{
		return;
	}

	AActor* Target = TargetLock->GetCurrentTarget();
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
	FString BodyPartText = TEXT("Targeting: ");
	switch (TargetLock->GetTargetedBodyPart())
	{
		case EBodyPart::UpperBody: BodyPartText += TEXT("Upper Body"); break;
		case EBodyPart::LeftLeg: BodyPartText += TEXT("Left Leg"); break;
		case EBodyPart::RightLeg: BodyPartText += TEXT("Right Leg"); break;
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
	UTargetLockComponent* TargetLock = PlayerCharacter->GetTargetLockComponent();
	if (!TargetLock || !TargetLock->IsLocked())
	{
		return;
	}
	
	AActor* Target = TargetLock->GetCurrentTarget();
	ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(Target);
	if (!Enemy)
	{
		return;
	}
	
	// Get the body part component
	UBodyPartComponent* BodyPartComp = nullptr;
	EBodyPart TargetedPart = TargetLock->GetTargetedBodyPart();
	
	switch (TargetedPart)
	{
		case EBodyPart::UpperBody:
			BodyPartComp = Enemy->UpperBodyPart;
			break;
		case EBodyPart::LeftLeg:
			BodyPartComp = Enemy->LeftLegPart;
			break;
		case EBodyPart::RightLeg:
			BodyPartComp = Enemy->RightLegPart;
			break;
	}
	
	if (!BodyPartComp)
	{
		return;
	}
	
	// Get body part location and convert to screen space
	FVector BodyPartLocation = BodyPartComp->GetComponentLocation();
	FVector2D ScreenLocation;
	
	if (GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(BodyPartLocation, ScreenLocation))
	{
		// Draw targeting reticle
		float ReticleSize = 20.0f;
		FColor ReticleColor = FColor::Yellow;
		
		// Draw crosshair
		DrawLine(ScreenLocation.X - ReticleSize, ScreenLocation.Y, 
				ScreenLocation.X + ReticleSize, ScreenLocation.Y, ReticleColor, 2.0f);
		DrawLine(ScreenLocation.X, ScreenLocation.Y - ReticleSize, 
				ScreenLocation.X, ScreenLocation.Y + ReticleSize, ReticleColor, 2.0f);
		
		// Draw corner brackets
		float BracketSize = ReticleSize * 1.5f;
		float BracketLength = ReticleSize * 0.5f;
		
		// Top-left
		DrawLine(ScreenLocation.X - BracketSize, ScreenLocation.Y - BracketSize,
				ScreenLocation.X - BracketSize + BracketLength, ScreenLocation.Y - BracketSize, ReticleColor, 3.0f);
		DrawLine(ScreenLocation.X - BracketSize, ScreenLocation.Y - BracketSize,
				ScreenLocation.X - BracketSize, ScreenLocation.Y - BracketSize + BracketLength, ReticleColor, 3.0f);
		
		// Top-right
		DrawLine(ScreenLocation.X + BracketSize, ScreenLocation.Y - BracketSize,
				ScreenLocation.X + BracketSize - BracketLength, ScreenLocation.Y - BracketSize, ReticleColor, 3.0f);
		DrawLine(ScreenLocation.X + BracketSize, ScreenLocation.Y - BracketSize,
				ScreenLocation.X + BracketSize, ScreenLocation.Y - BracketSize + BracketLength, ReticleColor, 3.0f);
		
		// Bottom-left
		DrawLine(ScreenLocation.X - BracketSize, ScreenLocation.Y + BracketSize,
				ScreenLocation.X - BracketSize + BracketLength, ScreenLocation.Y + BracketSize, ReticleColor, 3.0f);
		DrawLine(ScreenLocation.X - BracketSize, ScreenLocation.Y + BracketSize,
				ScreenLocation.X - BracketSize, ScreenLocation.Y + BracketSize - BracketLength, ReticleColor, 3.0f);
		
		// Bottom-right
		DrawLine(ScreenLocation.X + BracketSize, ScreenLocation.Y + BracketSize,
				ScreenLocation.X + BracketSize - BracketLength, ScreenLocation.Y + BracketSize, ReticleColor, 3.0f);
		DrawLine(ScreenLocation.X + BracketSize, ScreenLocation.Y + BracketSize,
				ScreenLocation.X + BracketSize, ScreenLocation.Y + BracketSize - BracketLength, ReticleColor, 3.0f);
		
		// Draw body part name
		FString BodyPartName;
		switch (TargetedPart)
		{
			case EBodyPart::UpperBody: BodyPartName = TEXT("UPPER BODY"); break;
			case EBodyPart::LeftLeg: BodyPartName = TEXT("LEFT LEG"); break;
			case EBodyPart::RightLeg: BodyPartName = TEXT("RIGHT LEG"); break;
		}
		
		DrawText(BodyPartName, ReticleColor, ScreenLocation.X - 40, ScreenLocation.Y + BracketSize + 5, HUDFont);
	}
}

void ACybersoulsHUD::DrawEnemyQuickHackCasting()
{
	// Get all enemies in range
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACybersoulsEnemyBase::StaticClass(), FoundEnemies);
	
	float MaxDisplayDistance = 2000.0f; // Only show casting for enemies within this range
	
	for (AActor* EnemyActor : FoundEnemies)
	{
		ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(EnemyActor);
		if (!Enemy)
		{
			continue;
		}
		
		// Check distance to player
		float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), Enemy->GetActorLocation());
		if (Distance > MaxDisplayDistance)
		{
			continue;
		}
		
		// Check for active QuickHacks
		TArray<UQuickHackComponent*> QuickHacks;
		Enemy->GetComponents<UQuickHackComponent>(QuickHacks);
		
		for (UQuickHackComponent* QuickHack : QuickHacks)
		{
			if (QuickHack && QuickHack->IsQuickHackActive())
			{
				// Convert enemy position to screen
				FVector EnemyPosition = Enemy->GetActorLocation() + FVector(0, 0, 150); // Above enemy's head
				FVector2D ScreenPosition;
				
				if (GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(EnemyPosition, ScreenPosition))
				{
					// Draw casting bar
					float CastPercent = QuickHack->CurrentCastTime / QuickHack->CastTime;
					float BarWidth = 100.0f;
					float BarHeight = 10.0f;
					
					// Background
					DrawRect(FLinearColor::Black, ScreenPosition.X - BarWidth/2, ScreenPosition.Y, BarWidth, BarHeight);
					
					// Cast progress
					FLinearColor CastColor = FLinearColor::Red;
					DrawRect(CastColor, ScreenPosition.X - BarWidth/2, ScreenPosition.Y, BarWidth * CastPercent, BarHeight);
					
					// QuickHack name
					FString QuickHackName;
					switch (QuickHack->QuickHackType)
					{
					case EQuickHackType::InterruptProtocol:
						QuickHackName = TEXT("Interrupt Protocol");
						break;
					case EQuickHackType::SystemFreeze:
						QuickHackName = TEXT("System Freeze");
						break;
					case EQuickHackType::Firewall:
						QuickHackName = TEXT("Firewall");
						break;
					case EQuickHackType::Kill:
						QuickHackName = TEXT("Kill");
						break;
					default:
						QuickHackName = TEXT("Unknown");
						break;
					}
					
					// Enemy type + casting text
					FString EnemyTypeText;
					switch (Enemy->EnemyType)
					{
					case EEnemyType::Netrunner:
						EnemyTypeText = TEXT("Netrunner");
						break;
					case EEnemyType::BuffNetrunner:
						EnemyTypeText = TEXT("Buff Netrunner");
						break;
					case EEnemyType::DebuffNetrunner:
						EnemyTypeText = TEXT("Debuff Netrunner");
						break;
					default:
						EnemyTypeText = TEXT("Enemy");
						break;
					}
					
					FString CastingText = FString::Printf(TEXT("%s casting %s"), *EnemyTypeText, *QuickHackName);
					DrawText(CastingText, FColor::Red, ScreenPosition.X - 75, ScreenPosition.Y - 20, HUDFont);
					
					// Cast time remaining
					float TimeRemaining = QuickHack->CastTime - QuickHack->CurrentCastTime;
					FString TimeText = FString::Printf(TEXT("%.1fs"), TimeRemaining);
					DrawText(TimeText, FColor::White, ScreenPosition.X - 15, ScreenPosition.Y + 12, HUDFont);
				}
			}
		}
	}
}
void ACybersoulsHUD::DrawXPDisplay()
{
	UPlayerProgressionComponent* Progression = PlayerCharacter->GetPlayerProgression();
	if (!Progression)
	{
		return;
	}

	// Position XP display in top right corner for better visibility
	float XPX = Canvas->SizeX - 350.0f;
	float XPY = 120.0f;
	float LineHeight = 35.0f;

	// Draw larger XP background with border
	float BGWidth = 320.0f;
	float BGHeight = 120.0f;
	
	// Dark background with border
	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f), XPX - 15, XPY - 15, BGWidth, BGHeight);
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 0.8f), XPX - 15, XPY - 15, BGWidth, 3.0f); // Top border
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 0.8f), XPX - 15, XPY - 15, 3.0f, BGHeight); // Left border
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 0.8f), XPX + BGWidth - 18, XPY - 15, 3.0f, BGHeight); // Right border
	DrawRect(FLinearColor(0.2f, 0.8f, 1.0f, 0.8f), XPX - 15, XPY + BGHeight - 18, BGWidth, 3.0f); // Bottom border

	// Large title with cyberpunk styling
	DrawText(TEXT("▰▰ EXPERIENCE POINTS ▰▰"), FColor::Cyan, XPX, XPY, HUDFont, 1.2f);

	// Integrity XP with larger font and bright colors
	FString IntegrityXPText = FString::Printf(TEXT("⚡ INTEGRITY XP: %.0f"), Progression->GetIntegrityXP());
	DrawText(IntegrityXPText, FColor::Green, XPX, XPY + LineHeight, HUDFont, 1.1f);

	// Hacking XP with larger font and bright colors
	FString HackingXPText = FString::Printf(TEXT("🔧 HACKING XP: %.0f"), Progression->GetHackingXP());
	DrawText(HackingXPText, FColor::Blue, XPX, XPY + 2 * LineHeight, HUDFont, 1.1f);
}
