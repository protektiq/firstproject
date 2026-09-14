// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class ETargetRushState : uint8
{
	WaitingToStart,
	Playing,
	Won,
	Lost
};

/**
 * Round rules for Target Rush: hit every target before the clock runs out.
 * Has no world or UObject dependencies so it can be unit tested directly.
 */
struct FIRSTPROJECT_API FTargetRushRules
{
	/** Starts a fresh round. Stays WaitingToStart if there are no targets or no time. */
	void Start(int32 InTotalTargets, float InDurationSeconds);

	/** Advances the round clock. Ends the round as Lost when time runs out. */
	void Advance(float DeltaSeconds);

	/** Counts a hit while playing. Ends the round as Won on the last target. Returns false if the hit was ignored. */
	bool RegisterHit();

	ETargetRushState GetState() const { return State; }
	bool IsRoundOver() const { return State == ETargetRushState::Won || State == ETargetRushState::Lost; }
	int32 GetHits() const { return Hits; }
	int32 GetTotalTargets() const { return TotalTargets; }
	float GetElapsedSeconds() const { return ElapsedSeconds; }
	float GetTimeRemaining() const { return FMath::Max(DurationSeconds - ElapsedSeconds, 0.f); }

private:
	ETargetRushState State = ETargetRushState::WaitingToStart;
	int32 TotalTargets = 0;
	int32 Hits = 0;
	float DurationSeconds = 0.f;
	float ElapsedSeconds = 0.f;
};
