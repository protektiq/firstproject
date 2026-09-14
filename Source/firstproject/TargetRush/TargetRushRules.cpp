// Copyright Epic Games, Inc. All Rights Reserved.

#include "TargetRush/TargetRushRules.h"

void FTargetRushRules::Start(int32 InTotalTargets, float InDurationSeconds)
{
	TotalTargets = FMath::Max(InTotalTargets, 0);
	DurationSeconds = FMath::Max(InDurationSeconds, 0.f);
	Hits = 0;
	ElapsedSeconds = 0.f;
	State = (TotalTargets > 0 && DurationSeconds > 0.f) ? ETargetRushState::Playing : ETargetRushState::WaitingToStart;
}

void FTargetRushRules::Advance(float DeltaSeconds)
{
	if (State != ETargetRushState::Playing)
	{
		return;
	}

	ElapsedSeconds = FMath::Min(ElapsedSeconds + FMath::Max(DeltaSeconds, 0.f), DurationSeconds);
	if (ElapsedSeconds >= DurationSeconds)
	{
		State = ETargetRushState::Lost;
	}
}

bool FTargetRushRules::RegisterHit()
{
	if (State != ETargetRushState::Playing)
	{
		return false;
	}

	++Hits;
	if (Hits >= TotalTargets)
	{
		State = ETargetRushState::Won;
	}
	return true;
}
