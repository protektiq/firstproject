// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TargetRush/TargetRushRules.h"
#include "TargetRushGameMode.generated.h"

class ATargetRushTarget;

/** Runs one Target Rush round: every target placed in the level must be hit before the clock runs out. */
UCLASS()
class FIRSTPROJECT_API ATargetRushGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATargetRushGameMode();

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Called by a target the moment it is shot. */
	void NotifyTargetHit(ATargetRushTarget* Target);

	/** Reloads the current level for a fresh round. Ignored until the round is over. */
	void RestartRound();

	const FTargetRushRules& GetRules() const { return Rules; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush", meta = (ClampMin = "1.0", Units = "s"))
	float RoundDurationSeconds = 60.f;

private:
	/** Freezes the player's pawn and logs the result. */
	void EndRound();

	FTargetRushRules Rules;
};
