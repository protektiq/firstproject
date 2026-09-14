// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TargetRushPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class USoundBase;

/**
 * Adds Target Rush's fire and restart input on top of the template's movement controls.
 * Firing is a hitscan from the player's camera; movement stays on the pawn.
 */
UCLASS()
class FIRSTPROJECT_API ATargetRushPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/** Added alongside the template's contexts, at a higher priority. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Input")
	TObjectPtr<UInputMappingContext> TargetRushMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Input")
	TObjectPtr<UInputAction> RestartAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Weapon")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Weapon", meta = (ClampMin = "100.0", Units = "cm"))
	float TraceRange = 10000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Weapon", meta = (ClampMin = "0.0", Units = "s"))
	float FireCooldown = 0.15f;

private:
	void OnFire();
	void OnRestart();

	double NextFireTime = 0.0;
};
