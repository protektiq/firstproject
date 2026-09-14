// Copyright Epic Games, Inc. All Rights Reserved.

#include "TargetRush/TargetRushHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "TargetRush/TargetRushGameMode.h"

namespace TargetRushHUD
{
	/** Layout is authored for 1080p and scaled to the actual viewport height. */
	constexpr float ReferenceHeight = 1080.f;
	const FLinearColor WarningColor(1.f, 0.2f, 0.15f);
	const FLinearColor WinColor(0.3f, 1.f, 0.4f);
	const FLinearColor BannerBackground(0.f, 0.f, 0.f, 0.6f);
}

void ATargetRushHUD::FlashHitMarker()
{
	LastHitRealTime = GetWorld()->GetRealTimeSeconds();
}

void ATargetRushHUD::DrawHUD()
{
	Super::DrawHUD();

	const ATargetRushGameMode* GameMode = GetWorld()->GetAuthGameMode<ATargetRushGameMode>();
	if (!GameMode || !Canvas)
	{
		return;
	}

	const FTargetRushRules& Rules = GameMode->GetRules();
	const float CenterX = Canvas->ClipX * 0.5f;
	const float CenterY = Canvas->ClipY * 0.5f;
	const float UIScale = Canvas->ClipY / TargetRushHUD::ReferenceHeight;
	UFont* LargeFont = GEngine->GetLargeFont();
	UFont* MediumFont = GEngine->GetMediumFont();

	if (Rules.GetState() == ETargetRushState::WaitingToStart)
	{
		DrawTextCentered(TEXT("No targets placed"), LargeFont, 2.f * UIScale, CenterX, CenterY, TargetRushHUD::WarningColor);
		return;
	}

	const bool bRoundOver = Rules.IsRoundOver();

	if (!bRoundOver)
	{
		const bool bShowHitMarker = LastHitRealTime >= 0.0 && GetWorld()->GetRealTimeSeconds() - LastHitRealTime < HitMarkerDuration;
		DrawCrosshair(CenterX, CenterY, UIScale, bShowHitMarker);
	}

	DrawText(FString::Printf(TEXT("TARGETS %d/%d"), Rules.GetHits(), Rules.GetTotalTargets()),
		FLinearColor::White, 40.f * UIScale, 30.f * UIScale, LargeFont, 1.5f * UIScale);

	const int32 SecondsLeft = FMath::CeilToInt(Rules.GetTimeRemaining());
	const bool bLowTime = !bRoundOver && Rules.GetTimeRemaining() <= LowTimeWarningSeconds;
	DrawTextCentered(FString::Printf(TEXT("%d:%02d"), SecondsLeft / 60, SecondsLeft % 60),
		LargeFont, 2.f * UIScale, CenterX, 20.f * UIScale, bLowTime ? TargetRushHUD::WarningColor : FLinearColor::White);

	if (bRoundOver)
	{
		const bool bWon = Rules.GetState() == ETargetRushState::Won;
		const FString Banner = bWon
			? FString::Printf(TEXT("YOU WIN - %.1fs"), Rules.GetElapsedSeconds())
			: FString::Printf(TEXT("TIME'S UP - %d/%d"), Rules.GetHits(), Rules.GetTotalTargets());

		DrawRect(TargetRushHUD::BannerBackground, 0.f, CenterY - 110.f * UIScale, Canvas->ClipX, 220.f * UIScale);
		DrawTextCentered(Banner, LargeFont, 3.f * UIScale, CenterX, CenterY - 85.f * UIScale,
			bWon ? TargetRushHUD::WinColor : TargetRushHUD::WarningColor);
		DrawTextCentered(TEXT("Press R to restart"), MediumFont, 1.5f * UIScale, CenterX, CenterY + 35.f * UIScale, FLinearColor::White);
	}
}

void ATargetRushHUD::DrawCrosshair(float CenterX, float CenterY, float UIScale, bool bShowHitMarker)
{
	const float Gap = 4.f * UIScale;
	const float Length = 10.f * UIScale;
	const float Thickness = FMath::Max(2.f * UIScale, 1.f);

	DrawLine(CenterX - Gap - Length, CenterY, CenterX - Gap, CenterY, FLinearColor::White, Thickness);
	DrawLine(CenterX + Gap, CenterY, CenterX + Gap + Length, CenterY, FLinearColor::White, Thickness);
	DrawLine(CenterX, CenterY - Gap - Length, CenterX, CenterY - Gap, FLinearColor::White, Thickness);
	DrawLine(CenterX, CenterY + Gap, CenterX, CenterY + Gap + Length, FLinearColor::White, Thickness);

	if (bShowHitMarker)
	{
		const float Inner = 8.f * UIScale;
		const float Outer = 18.f * UIScale;
		for (const FVector2D& Direction : { FVector2D(1, 1), FVector2D(1, -1), FVector2D(-1, 1), FVector2D(-1, -1) })
		{
			DrawLine(CenterX + Direction.X * Inner, CenterY + Direction.Y * Inner,
				CenterX + Direction.X * Outer, CenterY + Direction.Y * Outer, TargetRushHUD::WarningColor, Thickness);
		}
	}
}

void ATargetRushHUD::DrawTextCentered(const FString& Text, UFont* Font, float TextScale, float CenterX, float TopY, const FLinearColor& Color)
{
	float Width = 0.f;
	float Height = 0.f;
	GetTextSize(Text, Width, Height, Font, TextScale);
	DrawText(Text, Color, CenterX - Width * 0.5f, TopY, Font, TextScale);
}
