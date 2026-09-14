// Copyright Epic Games, Inc. All Rights Reserved.

#include "TargetRush/TargetRushPlayerController.h"

#include "CollisionQueryParams.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "firstproject.h"
#include "Kismet/GameplayStatics.h"
#include "TargetRush/TargetRushGameMode.h"
#include "TargetRush/TargetRushHUD.h"
#include "TargetRush/TargetRushTarget.h"

namespace TargetRushPlayerController
{
	/** Above the template's IMC_Default and IMC_MouseLook, which are added at priority 0. */
	constexpr int32 MappingContextPriority = 1;
}

void ATargetRushPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalPlayerController())
	{
		return;
	}

	if (!TargetRushMappingContext)
	{
		UE_LOG(LogTargetRush, Warning, TEXT("%s has no TargetRushMappingContext; fire and restart are unbound"), *GetName());
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(TargetRushMappingContext, TargetRushPlayerController::MappingContextPriority);
	}
}

void ATargetRushPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogTargetRush, Error, TEXT("%s needs an EnhancedInputComponent; check DefaultInputComponentClass"), *GetName());
		return;
	}

	if (FireAction)
	{
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::OnFire);
	}
	if (RestartAction)
	{
		EnhancedInput->BindAction(RestartAction, ETriggerEvent::Started, this, &ThisClass::OnRestart);
	}
}

void ATargetRushPlayerController::OnFire()
{
	const ATargetRushGameMode* GameMode = GetWorld()->GetAuthGameMode<ATargetRushGameMode>();
	if (!GameMode || GameMode->GetRules().GetState() != ETargetRushState::Playing)
	{
		return;
	}

	const double Now = GetWorld()->GetTimeSeconds();
	if (Now < NextFireTime)
	{
		return;
	}
	NextFireTime = Now + FireCooldown;

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TargetRushFire), false, GetPawn());
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, ViewLocation + ViewRotation.Vector() * TraceRange, ECC_Visibility, QueryParams))
	{
		return;
	}

	if (ATargetRushTarget* Target = Cast<ATargetRushTarget>(Hit.GetActor()))
	{
		Target->HandleShot();

		if (ATargetRushHUD* TargetRushHUD = GetHUD<ATargetRushHUD>())
		{
			TargetRushHUD->FlashHitMarker();
		}
	}
}

void ATargetRushPlayerController::OnRestart()
{
	if (ATargetRushGameMode* GameMode = GetWorld()->GetAuthGameMode<ATargetRushGameMode>())
	{
		if (GameMode->GetRules().IsRoundOver())
		{
			GameMode->RestartRound();
		}
	}
}
