// BodyPartComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BodyPartComponent.generated.h"

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
	None UMETA(DisplayName = "None"),
	UpperBody UMETA(DisplayName = "Upper Body"),
	LeftLeg UMETA(DisplayName = "Left Leg"),
	RightLeg UMETA(DisplayName = "Right Leg")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UBodyPartComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UBodyPartComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body Part")
	EBodyPart BodyPartType = EBodyPart::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body Part")
	FVector TargetOffset = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, Category = "Body Part")
	bool CanBeTargeted() const { return BodyPartType != EBodyPart::None; }

protected:
	virtual void BeginPlay() override;
};