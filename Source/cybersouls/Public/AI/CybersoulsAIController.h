// CybersoulsAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CybersoulsAIController.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACybersoulsAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AcceptanceRadius = 100.0f;

	UPROPERTY()
	class ACharacter* PlayerCharacter;

	void MoveToPlayer();
};