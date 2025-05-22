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

	/** Camera Toggle Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CameraToggleAction;

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


public:
	AcybersoulsCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	/** Called for slash input */
	void PerformSlash();

	/** Called for quickhack inputs */
	void UseQuickHack1();
	void UseQuickHack2();
	void UseQuickHack3();
	void UseQuickHack4();

	/** Called for camera toggle input */
	void ToggleCameraView();

	/** Crosshair-based aiming */
	void UpdateCrosshairTarget();
	void DetermineCrosshairBodyPart(const FHitResult& HitResult);
	
public:
	AActor* GetCrosshairTarget() const;
	EBodyPart GetCrosshairBodyPart() const;
			

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

private:
	// Camera view state
	bool bIsFirstPersonView = false;
	
	// Crosshair targeting
	AActor* CrosshairTarget = nullptr;
	EBodyPart CrosshairBodyPart = EBodyPart::UpperBody;
	
	// Crosshair settings
	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairRange = 2000.0f;
};

