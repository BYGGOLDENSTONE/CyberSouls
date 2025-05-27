#include "cybersouls/Public/Player/DashAbilityComponent.h"
#include "cybersouls/Public/Player/PlayerCyberStateAttributeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

UDashAbilityComponent::UDashAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bIsDashing = false;
    DashTimeRemaining = 0.0f;
    CurrentCharges = 1;
    ChargeRegenTimer = 0.0f;
    
    // Set base ability properties
    AbilityName = "Dash";
    Cooldown = 0.5f;
}

void UDashAbilityComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        AttributeComponent = OwnerCharacter->FindComponentByClass<UPlayerCyberStateAttributeComponent>();
    }
    
    // Initialize charges
    CurrentCharges = MaxCharges;
    ChargeRegenTimer = 0.0f;
}

void UDashAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsDashing)
    {
        UpdateDash(DeltaTime);
    }
    
    // Handle cooldown and charge regeneration
    if (OwnerCharacter)
    {
        // Cooldown is now handled by base class
        
        UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
        if (Movement && Movement->IsMovingOnGround())
        {
            // Handle charge regeneration only when on ground
            if (CurrentCharges < MaxCharges)
            {
                ChargeRegenTimer += DeltaTime;
                
                if (ChargeRegenTimer >= ChargeRegenTime)
                {
                    CurrentCharges++;
                    ChargeRegenTimer = 0.0f;
                }
            }
        }
        // When in air: charge regen pauses (timer maintains current value)
    }
}

bool UDashAbilityComponent::CanActivateAbility()
{
    if (!IsValid(OwnerCharacter) || !IsValid(AttributeComponent))
    {
        return false;
    }

    // Check if we have charges available
    if (CurrentCharges <= 0)
    {
        return false;
    }

    if (bIsDashing || CurrentCooldown > 0.0f)
    {
        return false;
    }

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (!Movement)
    {
        return false;
    }

    if (!bCanDashInAir && Movement->IsFalling())
    {
        return false;
    }

    return AttributeComponent->HasEnoughStamina(StaminaCost);
}

void UDashAbilityComponent::ActivateAbility()
{
    if (!CanActivateAbility())
    {
        return;
    }

    AttributeComponent->UseStamina(StaminaCost);
    CurrentCharges--;
    ChargeRegenTimer = 0.0f; // Reset regen timer when using a charge
    
    StartDash();
}

void UDashAbilityComponent::StartDash()
{
    bIsDashing = true;
    DashTimeRemaining = DashDuration;
    
    // Get crosshair direction
    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC)
    {
        DashDirection = OwnerCharacter->GetActorForwardVector();
    }
    else
    {
        // Get camera location and direction
        FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
        FVector CameraForward = PC->PlayerCameraManager->GetCameraRotation().Vector();
        
        // Perform line trace from camera through crosshair
        FVector TraceEnd = CameraLocation + (CameraForward * 10000.0f);
        FHitResult HitResult;
        FCollisionQueryParams TraceParams;
        TraceParams.AddIgnoredActor(OwnerCharacter);
        
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            CameraLocation,
            TraceEnd,
            ECC_Visibility,
            TraceParams
        );
        
        // Calculate dash direction toward hit point or camera forward
        FVector TargetPoint = bHit ? HitResult.Location : TraceEnd;
        FVector DashVector = (TargetPoint - OwnerCharacter->GetActorLocation()).GetSafeNormal();
        
        UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
        if (Movement)
        {
            if (Movement->IsMovingOnGround())
            {
                // On ground: 2D dash (remove vertical component)
                DashDirection = FVector(DashVector.X, DashVector.Y, 0.0f).GetSafeNormal();
            }
            else
            {
                // In air: 3D dash (full directional)
                DashDirection = DashVector;
            }
        }
    }
    
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (Movement)
    {
        OriginalVelocity = Movement->Velocity;
        Movement->BrakingFrictionFactor = 0.0f;
        Movement->SetMovementMode(MOVE_Flying);
    }
    
    UGameplayStatics::PlaySoundAtLocation(this, nullptr, OwnerCharacter->GetActorLocation());
}

void UDashAbilityComponent::UpdateDash(float DeltaTime)
{
    DashTimeRemaining -= DeltaTime;
    
    if (DashTimeRemaining <= 0.0f)
    {
        EndDash();
        return;
    }
    
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (Movement)
    {
        float DashSpeed = DashDistance / DashDuration;
        Movement->Velocity = DashDirection * DashSpeed;
    }
}

void UDashAbilityComponent::EndDash()
{
    bIsDashing = false;
    CurrentCooldown = Cooldown;
    
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (Movement)
    {
        Movement->SetMovementMode(MOVE_Falling);
        Movement->BrakingFrictionFactor = 2.0f;
        
        // Maintain some momentum after dash
        float EndSpeed = (DashDistance / DashDuration) * 0.3f;
        Movement->Velocity = DashDirection * EndSpeed;
        
        // For air dashes, preserve some vertical momentum
        if (!Movement->IsMovingOnGround())
        {
            Movement->Velocity.Z = FMath::Max(Movement->Velocity.Z, -500.0f);
        }
    }
}

void UDashAbilityComponent::ResetCharges()
{
    CurrentCharges = MaxCharges;
    ChargeRegenTimer = 0.0f;
}

void UDashAbilityComponent::DeactivateAbility()
{
    if (bIsDashing)
    {
        EndDash();
    }
}