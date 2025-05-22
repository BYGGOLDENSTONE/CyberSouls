// DodgeAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "DodgeAbilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UDodgeAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UDodgeAbilityComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	int32 MaxDodgeCharges = 3;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	int32 CurrentDodgeCharges = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	TArray<EBodyPart> DodgeableBodyParts = { EBodyPart::LeftLeg, EBodyPart::RightLeg };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	float DodgeDistance = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge")
	float DodgeSpeed = 800.0f;

	UFUNCTION(BlueprintCallable, Category = "Dodge")
	bool TryDodge(EBodyPart AttackedBodyPart, AActor* Attacker = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "Dodge")
	bool CanDodge(EBodyPart BodyPart) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dodge")
	void ConsumeDodgeCharge();

protected:
	virtual void BeginPlay() override;
	
private:
	void PerformDodgeMovement(AActor* Attacker);
};