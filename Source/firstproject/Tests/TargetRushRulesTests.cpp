// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "TargetRush/TargetRushRules.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTargetRushRulesTest, "Project.TargetRush.Rules",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTargetRushRulesTest::RunTest(const FString& Parameters)
{
	// Hitting every target before time runs out wins and records the clear time.
	{
		FTargetRushRules Rules;
		Rules.Start(3, 60.f);
		TestTrue(TEXT("Win: starts playing"), Rules.GetState() == ETargetRushState::Playing);

		Rules.Advance(10.f);
		TestTrue(TEXT("Win: first hit counts"), Rules.RegisterHit());
		Rules.RegisterHit();
		TestTrue(TEXT("Win: still playing before last target"), Rules.GetState() == ETargetRushState::Playing);
		Rules.RegisterHit();

		TestTrue(TEXT("Win: last target wins"), Rules.GetState() == ETargetRushState::Won);
		TestEqual(TEXT("Win: hits"), Rules.GetHits(), 3);
		TestEqual(TEXT("Win: elapsed time recorded"), Rules.GetElapsedSeconds(), 10.f, KINDA_SMALL_NUMBER);

		Rules.Advance(100.f);
		TestTrue(TEXT("Win: time passing afterwards does not lose"), Rules.GetState() == ETargetRushState::Won);
		TestFalse(TEXT("Win: extra hits are ignored"), Rules.RegisterHit());
		TestEqual(TEXT("Win: hits unchanged"), Rules.GetHits(), 3);
	}

	// Running out of time loses, and hits after that are ignored.
	{
		FTargetRushRules Rules;
		Rules.Start(3, 60.f);
		Rules.RegisterHit();
		Rules.Advance(30.f);
		TestTrue(TEXT("Lose: still playing mid-round"), Rules.GetState() == ETargetRushState::Playing);
		TestEqual(TEXT("Lose: time remaining mid-round"), Rules.GetTimeRemaining(), 30.f, KINDA_SMALL_NUMBER);

		Rules.Advance(31.f);
		TestTrue(TEXT("Lose: expiry loses"), Rules.GetState() == ETargetRushState::Lost);
		TestEqual(TEXT("Lose: time remaining clamps to zero"), Rules.GetTimeRemaining(), 0.f, KINDA_SMALL_NUMBER);
		TestEqual(TEXT("Lose: elapsed clamps to duration"), Rules.GetElapsedSeconds(), 60.f, KINDA_SMALL_NUMBER);
		TestFalse(TEXT("Lose: late hits are ignored"), Rules.RegisterHit());
		TestEqual(TEXT("Lose: hits unchanged"), Rules.GetHits(), 1);

		Rules.Start(3, 60.f);
		TestTrue(TEXT("Lose: restarting plays again"), Rules.GetState() == ETargetRushState::Playing);
		TestEqual(TEXT("Lose: restart clears hits"), Rules.GetHits(), 0);
	}

	// A level without targets never starts a round.
	{
		FTargetRushRules Rules;
		Rules.Start(0, 60.f);
		TestTrue(TEXT("No targets: waits"), Rules.GetState() == ETargetRushState::WaitingToStart);
		TestFalse(TEXT("No targets: hits ignored"), Rules.RegisterHit());
		Rules.Advance(100.f);
		TestTrue(TEXT("No targets: never loses"), Rules.GetState() == ETargetRushState::WaitingToStart);
	}

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
