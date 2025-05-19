// BodyPartComponent.cpp
#include "cybersouls/Public/Combat/BodyPartComponent.h"

UBodyPartComponent::UBodyPartComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBodyPartComponent::BeginPlay()
{
	Super::BeginPlay();
}