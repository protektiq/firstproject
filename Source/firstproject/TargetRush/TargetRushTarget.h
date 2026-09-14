// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetRushTarget.generated.h"

class UMaterialInterface;
class USoundBase;
class UStaticMeshComponent;

/** A shootable target. When hit it flashes, plays a sound, tells the game mode, then pops out of existence. */
UCLASS()
class FIRSTPROJECT_API ATargetRushTarget : public AActor
{
	GENERATED_BODY()

public:
	ATargetRushTarget();

	virtual void Tick(float DeltaSeconds) override;

	/** Reacts to being shot. Only the first shot counts. */
	void HandleShot();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target Rush")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Feedback")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Feedback", meta = (ClampMin = "0.1"))
	float HitSoundPitch = 1.5f;

	/** Applied to every material slot the moment the target is hit. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Feedback")
	TObjectPtr<UMaterialInterface> FlashMaterial;

	/** How long the swell-and-shrink pop lasts before the target is destroyed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Target Rush|Feedback", meta = (ClampMin = "0.01", Units = "s"))
	float PopDuration = 0.2f;

private:
	bool bHit = false;
	float PopElapsed = 0.f;
	FVector BaseScale = FVector::OneVector;
};
