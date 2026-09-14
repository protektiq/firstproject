// Copyright Epic Games, Inc. All Rights Reserved.

#include "TargetRush/TargetRushGameMode.h"

#include "EngineUtils.h"
#include "firstproject.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TargetRush/TargetRushHUD.h"
#include "TargetRush/TargetRushTarget.h"

ATargetRushGameMode::ATargetRushGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	HUDClass = ATargetRushHUD::StaticClass();
}

void ATargetRushGameMode::StartPlay()
{
	Super::StartPlay();

	int32 TargetCount = 0;
	for (TActorIterator<ATargetRushTarget> It(GetWorld()); It; ++It)
	{
		++TargetCount;
	}

	Rules.Start(TargetCount, RoundDurationSeconds);

	if (Rules.GetState() == ETargetRushState::Playing)
	{
		UE_LOG(LogTargetRush, Log, TEXT("Round started: %d targets, %.0fs"), TargetCount, RoundDurationSeconds);
	}
	else
	{
		UE_LOG(LogTargetRush, Warning, TEXT("Round not started: no ATargetRushTarget actors found in %s"), *GetWorld()->GetMapName());
	}
}

void ATargetRushGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Rules.GetState() != ETargetRushState::Playing)
	{
		return;
	}

	Rules.Advance(DeltaSeconds);
	if (Rules.IsRoundOver())
	{
		EndRound();
	}
}

void ATargetRushGameMode::NotifyTargetHit(ATargetRushTarget* Target)
{
	if (!Rules.RegisterHit())
	{
		return;
	}

	UE_LOG(LogTargetRush, Log, TEXT("Target hit: %s (%d/%d)"), *GetNameSafe(Target), Rules.GetHits(), Rules.GetTotalTargets());

	if (Rules.IsRoundOver())
	{
		EndRound();
	}
}

void ATargetRushGameMode::RestartRound()
{
	if (!Rules.IsRoundOver())
	{
		return;
	}

	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void ATargetRushGameMode::EndRound()
{
	// Disabling the pawn's input stops move, look and jump; the controller keeps its own bindings so restart still works.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PlayerController = It->Get();
		if (PlayerController && PlayerController->GetPawn())
		{
			PlayerController->GetPawn()->DisableInput(PlayerController);
		}
	}

	if (Rules.GetState() == ETargetRushState::Won)
	{
		UE_LOG(LogTargetRush, Log, TEXT("Round won: %d/%d in %.1fs"), Rules.GetHits(), Rules.GetTotalTargets(), Rules.GetElapsedSeconds());
	}
	else
	{
		UE_LOG(LogTargetRush, Log, TEXT("Round lost: %d/%d"), Rules.GetHits(), Rules.GetTotalTargets());
	}
}
