// PlayerProgressionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerProgressionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntegrityXPChanged, float, NewIntegrityXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHackingXPChanged, float, NewHackingXP);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UPlayerProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerProgressionComponent();

	// XP Values from preserved stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float IntegrityXP = 0.0f; // XP from preserved integrity

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float HackingXP = 0.0f; // XP from preserved hack resistance (inverse of hackprogress)

	// Events
	UPROPERTY(BlueprintAssignable)
	FOnIntegrityXPChanged OnIntegrityXPChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHackingXPChanged OnHackingXPChanged;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void ConvertStatsToXP(float RemainingIntegrity, float RemainingHackProgress, float MaxHackProgress);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddIntegrityXP(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddHackingXP(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	float GetIntegrityXP() const { return IntegrityXP; }

	UFUNCTION(BlueprintCallable, Category = "Progression")
	float GetHackingXP() const { return HackingXP; }

	// Save/Load progression
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void SaveProgression();

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void LoadProgression();

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void ResetProgression();

protected:
	virtual void BeginPlay() override;

private:
	// XP conversion ratios
	const float IntegrityToXPRatio = 1.0f; // 1:1 conversion
	const float HackResistanceToXPRatio = 0.5f; // Lower ratio since hack resistance is more valuable
};