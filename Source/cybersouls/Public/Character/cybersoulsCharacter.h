// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "cybersoulsCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPlayerAttributeComponent;
class USlashAbilityComponent;
class UQuickHackComponent;
class UTargetLockComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AcybersoulsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Target Lock Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetLockAction;

	/** Target Change Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetChangeLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetChangeRightAction;

	/** Slash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlashAction;

	/** QuickHack Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QuickHack1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QuickHack2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QuickHack3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QuickHack4Action;

	/** Game system components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPlayerAttributeComponent* PlayerAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UPlayerProgressionComponent* PlayerProgression;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class USlashAbilityComponent* SlashAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UQuickHackComponent* InterruptProtocolAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UQuickHackComponent* SystemFreezeAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UQuickHackComponent* FirewallAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UQuickHackComponent* KillAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UTargetLockComponent* TargetLockComponent;

public:
	AcybersoulsCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for target lock input */
	void ToggleTargetLock();

	/** Called for target change input */
	void ChangeTargetLeft();
	void ChangeTargetRight();

	/** Called for slash input */
	void PerformSlash();

	/** Called for quickhack inputs */
	void UseQuickHack1();
	void UseQuickHack2();
	void UseQuickHack3();
	void UseQuickHack4();

	/** Target part change with mouse movement */
	void UpdateTargetPart(const FInputActionValue& Value);
	
	/** Camera lock functionality */
	void UpdateCameraLock(float DeltaTime);
	void OnTargetLocked(AActor* Target, EBodyPart BodyPart);
	void OnTargetUnlocked();
	FVector GetBodyPartLocation(AActor* Target, EBodyPart BodyPart) const;
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	
	// Tick
	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns PlayerAttributes subobject **/
	FORCEINLINE class UPlayerAttributeComponent* GetPlayerAttributes() const { return PlayerAttributes; }
	/** Returns PlayerProgression subobject **/
	FORCEINLINE class UPlayerProgressionComponent* GetPlayerProgression() const { return PlayerProgression; }
	/** Returns SlashAbility subobject **/
	FORCEINLINE class USlashAbilityComponent* GetSlashAbility() const { return SlashAbility; }
	/** Returns InterruptProtocolAbility subobject **/
	FORCEINLINE class UQuickHackComponent* GetInterruptProtocolAbility() const { return InterruptProtocolAbility; }
	/** Returns SystemFreezeAbility subobject **/
	FORCEINLINE class UQuickHackComponent* GetSystemFreezeAbility() const { return SystemFreezeAbility; }
	/** Returns FirewallAbility subobject **/
	FORCEINLINE class UQuickHackComponent* GetFirewallAbility() const { return FirewallAbility; }
	/** Returns KillAbility subobject **/
	FORCEINLINE class UQuickHackComponent* GetKillAbility() const { return KillAbility; }
	/** Returns TargetLockComponent subobject **/
	FORCEINLINE class UTargetLockComponent* GetTargetLockComponent() const { return TargetLockComponent; }

private:
	// Camera lock state
	bool bIsCameraLocked = false;
	AActor* LockedTarget = nullptr;
	EBodyPart LockedBodyPart = EBodyPart::UpperBody;
	
	// Camera lock settings
	UPROPERTY(EditAnywhere, Category = "Camera Lock")
	float CameraLockInterpSpeed = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "Camera Lock")
	float CameraLockOffsetZ = 50.0f;
	
	UPROPERTY(EditAnywhere, Category = "Camera Lock")
	float CameraLockDistance = 500.0f;
};

