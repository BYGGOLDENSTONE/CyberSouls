// TargetLockComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "TargetLockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTargetLocked, AActor*, Target, EBodyPart, BodyPart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetUnlocked);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UTargetLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetLockComponent();

	UPROPERTY(BlueprintAssignable)
	FOnTargetLocked OnTargetLocked;

	UPROPERTY(BlueprintAssignable)
	FOnTargetUnlocked OnTargetUnlocked;

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	void ToggleTargetLock();

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	void LockTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	void UnlockTarget();

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	void ChangeTargetLeft();

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	void ChangeTargetRight();

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	void ChangeBodyPart(EBodyPart NewBodyPart);

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	EBodyPart GetTargetedBodyPart() const { return TargetedBodyPart; }

	UFUNCTION(BlueprintCallable, Category = "Target Lock")
	bool IsLocked() const { return CurrentTarget != nullptr; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Lock")
	float MaxLockDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Lock")
	float MaxLockAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Lock")
	TSubclassOf<AActor> EnemyClass;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	UPROPERTY()
	EBodyPart TargetedBodyPart = EBodyPart::UpperBody;

	AActor* FindBestTarget() const;
	TArray<AActor*> GetValidTargets() const;
	bool IsValidTarget(AActor* Target) const;
	void UpdateLockValidity();
	UBodyPartComponent* GetBodyPartComponent(AActor* Actor, EBodyPart BodyPart) const;
};