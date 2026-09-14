// Copyright Epic Games, Inc. All Rights Reserved.

#include "TargetRush/TargetRushTarget.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TargetRush/TargetRushGameMode.h"

namespace TargetRushTarget
{
	/** Fraction of the pop spent swelling before shrinking away. */
	constexpr float SwellFraction = 1.f / 3.f;
	constexpr float SwellScale = 1.3f;
}

ATargetRushTarget::ATargetRushTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	Mesh->SetMobility(EComponentMobility::Movable);
	RootComponent = Mesh;
}

void ATargetRushTarget::HandleShot()
{
	if (bHit)
	{
		return;
	}
	bHit = true;

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (FlashMaterial)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < Mesh->GetNumMaterials(); ++MaterialIndex)
		{
			Mesh->SetMaterial(MaterialIndex, FlashMaterial);
		}
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation(), 1.f, HitSoundPitch);
	}

	if (ATargetRushGameMode* GameMode = GetWorld()->GetAuthGameMode<ATargetRushGameMode>())
	{
		GameMode->NotifyTargetHit(this);
	}

	BaseScale = GetActorScale3D();
	SetActorTickEnabled(true);
}

void ATargetRushTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PopElapsed += DeltaSeconds;
	const float Alpha = FMath::Clamp(PopElapsed / PopDuration, 0.f, 1.f);

	const float Scale = Alpha < TargetRushTarget::SwellFraction
		? FMath::Lerp(1.f, TargetRushTarget::SwellScale, Alpha / TargetRushTarget::SwellFraction)
		: FMath::Lerp(TargetRushTarget::SwellScale, 0.f, (Alpha - TargetRushTarget::SwellFraction) / (1.f - TargetRushTarget::SwellFraction));

	SetActorScale3D(BaseScale * FMath::Max(Scale, KINDA_SMALL_NUMBER));

	if (Alpha >= 1.f)
	{
		Destroy();
	}
}
