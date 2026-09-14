// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TargetRushHUD.generated.h"

/** Canvas HUD for Target Rush: crosshair, hitmarker, target counter, countdown and the end-of-round banner. */
UCLASS()
class FIRSTPROJECT_API ATargetRushHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	/** Flashes the hitmarker around the crosshair. */
	void FlashHitMarker();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Target Rush", meta = (ClampMin = "0.0", Units = "s"))
	float HitMarkerDuration = 0.15f;

	/** The countdown turns red at or below this many seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Target Rush", meta = (ClampMin = "0.0", Units = "s"))
	float LowTimeWarningSeconds = 10.f;

private:
	void DrawCrosshair(float CenterX, float CenterY, float UIScale, bool bShowHitMarker);
	void DrawTextCentered(const FString& Text, UFont* Font, float TextScale, float CenterX, float TopY, const FLinearColor& Color);

	double LastHitRealTime = -1.0;
};
