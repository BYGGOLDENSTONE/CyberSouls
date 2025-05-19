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
	class UFont* HUDFont;

	void DrawIntegrityBar();
	void DrawHackProgressBar();
	void DrawQuickHackStatus();
	void DrawTargetInfo();
	void DrawTargetBodyPartIndicator();
	void DrawEnemyQuickHackCasting();
};