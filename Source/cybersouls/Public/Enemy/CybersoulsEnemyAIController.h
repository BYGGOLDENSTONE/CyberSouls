// CybersoulsEnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/AI/CybersoulsAIController.h"
#include "CybersoulsEnemyAIController.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsEnemyAIController : public ACybersoulsAIController
{
	GENERATED_BODY()

public:
	ACybersoulsEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	class ACybersoulsEnemyBase* ControlledEnemy;

	UPROPERTY()
	AActor* CurrentTarget;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AttackRange = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float HackRange = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SightRange = 2000.0f;

	void UpdateBehavior();
	bool CanSeeTarget(AActor* Target) const;
	float GetDistanceToTarget(AActor* Target) const;
};