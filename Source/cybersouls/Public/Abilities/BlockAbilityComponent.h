// BlockAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "BlockAbilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UBlockAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UBlockAbilityComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	int32 MaxBlockCharges = 3;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Block")
	int32 CurrentBlockCharges = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	TArray<EBodyPart> BlockableBodyParts = { EBodyPart::UpperBody };

	UFUNCTION(BlueprintCallable, Category = "Block")
	bool TryBlock(EBodyPart AttackedBodyPart);
	
	UFUNCTION(BlueprintCallable, Category = "Block")
	bool CanBlock(EBodyPart BodyPart) const;
	
	UFUNCTION(BlueprintCallable, Category = "Block")
	void ConsumeBlockCharge();

protected:
	virtual void BeginPlay() override;
};