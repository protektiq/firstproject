# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

`firstproject` is an Unreal Engine **5.8** project created from the Blank C++ template (config still has `TP_Blank` → `firstproject` redirects), with Epic's **First Person template content pack added on top**. It ships one gameplay loop, **Target Rush**: hit every target in `Lvl_FirstPerson` before a 60-second clock runs out. The loop's rules live in C++ under `Source/firstproject/TargetRush/`; Blueprints only hold asset references and tuning values. Template movement (move, look, jump) is still pure Blueprint.

- Engine: launcher install at `C:\Program Files\Epic Games\UE_5.8` (the `EngineAssociation` GUID in `firstproject.uproject` maps to this path via `HKCU\SOFTWARE\Epic Games\Unreal Engine\Builds`).
- Git repository with **Git LFS** for binary assets (`.gitattributes` tracks `*.uasset`, `*.umap`, images, audio, fonts). In a fresh clone run `git lfs install --local` before pulling.
- `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, `Build/` (packaged output), `.vs/`, and the `.sln`/`.slnx` files are generated and gitignored — don't edit them by hand.

## Build and run

Close the editor before building the editor target from the command line, otherwise the DLL is locked. Live Coding (Ctrl+Alt+F11) is fine for edits to existing functions, but adding a new `UCLASS`/`UPROPERTY` needs a full editor-target build with the editor closed. The game target doesn't touch the editor DLL, so building it while the editor is open is a quick compile check.

```powershell
# Build the editor target (what the editor loads: Binaries/Win64/UnrealEditor-firstproject.dll)
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" firstprojectEditor Win64 Development -Project="D:\firstproject\firstproject.uproject" -WaitMutex

# Build the standalone game target
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" firstproject Win64 Development -Project="D:\firstproject\firstproject.uproject" -WaitMutex

# Regenerate Visual Studio project files (after adding/removing source files or modules)
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" -ProjectFiles -Project="D:\firstproject\firstproject.uproject" -Game

# Launch the editor
& "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" "D:\firstproject\firstproject.uproject"
```

## Tests

`Project.TargetRush.Rules` (`Source/firstproject/Tests/TargetRushRulesTests.cpp`) unit-tests `FTargetRushRules`: winning, losing on time, ignored late hits, and a level with no targets. Run automation tests with the editor closed:

```powershell
# Run tests whose name starts with a filter (e.g. "Project." or a specific test path)
& "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "D:\firstproject\firstproject.uproject" -ExecCmds="Automation RunTests Project." -TestExit="Automation Test Queue Empty" -unattended -nop4 -nosplash -NullRHI -log
```

Results land in `Saved/Logs/firstproject.log` (look for `Test Completed. Result={Success}`).

## Packaging

```powershell
# Development package (keeps logging and the console; use it to smoke-test a cook)
& "C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="D:\firstproject\firstproject.uproject" -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive -archivedirectory="D:\firstproject\Build\Development" -utf8output

# Shipping package (the release build): same command with
#   -clientconfig=Shipping -archivedirectory="D:\firstproject\Build\Shipping"
```

The executable ends up in `Build\<Config>\Windows\firstproject.exe`. Only `Lvl_FirstPerson` is cooked (`+MapsToCook` in `Config/DefaultGame.ini`), so add any new map there. A cold cook compiles SM6, ray tracing and Substrate shaders and takes a long time; later cooks reuse the DDC.

## Code layout

- `firstproject.uproject` — module list and enabled plugins.
- `Source/firstproject.Target.cs` / `Source/firstprojectEditor.Target.cs` — Game and Editor targets (`BuildSettingsVersion.V7`, `EngineIncludeOrderVersion.Unreal5_8`).
- `Source/firstproject/firstproject.Build.cs` — module dependencies: `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`. Add new engine modules here (e.g. `Slate`/`SlateCore`, `UMG`) when code needs them. It adds the module directory to the include paths, so code in subfolders is included as `"TargetRush/TargetRushGameMode.h"`.
- `Source/firstproject/firstproject.h` / `.cpp` — module boilerplate and the `LogTargetRush` log category.
- `Source/firstproject/TargetRush/` — the Target Rush gameplay classes (see below). `Source/firstproject/Tests/` — automation tests.
- `Config/DefaultEngine.ini` `[GameMapsSettings]` — game and editor startup map is `/Game/FirstPerson/Lvl_FirstPerson`; global default game mode is `BP_FirstPersonGameMode`.
- `Config/DefaultGame.ini` — packaging settings (maps to cook).
- `Config/Default*.ini` — project settings. Rendering is configured for high-end desktop: DX12 / SM6, Lumen GI and reflections, virtual shadow maps, hardware ray tracing, Substrate materials, static lighting disabled.

## Target Rush (the gameplay loop)

| Class | Role |
| --- | --- |
| `FTargetRushRules` | Plain C++ round state machine (`WaitingToStart` → `Playing` → `Won`/`Lost`) with no world dependencies; this is what the unit test covers. |
| `ATargetRushGameMode` | Counts `ATargetRushTarget` actors in `StartPlay`, advances the clock (`RoundDurationSeconds`) in `Tick`, disables the pawn's input when the round ends, and reloads the level in `RestartRound()`. Parent class of `BP_FirstPersonGameMode`. |
| `ATargetRushPlayerController` | Adds `IMC_TargetRush` at priority 1 and binds fire (a hitscan from the camera on `ECC_Visibility`) and restart. Parent class of `BP_FirstPersonPlayerController`, whose own BeginPlay still adds `IMC_Default` / `IMC_MouseLook`. |
| `ATargetRushTarget` | Shootable target: swaps to a flash material, plays a pitched hit sound, notifies the game mode, scales up then pops and destroys itself. |
| `ATargetRushHUD` | Canvas HUD (no widget assets): crosshair, hitmarker, target counter, countdown, win/lose banner. Set as the game mode's `HUDClass`. |

- Content lives in `/Game/TargetRush/`: `BP_TargetRushTarget` (mesh `SM_TargetBaseMesh`, `MI_DefaultColorway`, flash `MI_GlowNT`, sound `FirstPersonTemplateWeaponFire02`), and `Input/IA_Fire` (LMB, gamepad RT), `Input/IA_Restart` (R, gamepad Start), `Input/IMC_TargetRush`.
- The 8 placed targets are in the `TargetRush` outliner folder of `Lvl_FirstPerson`. Each has **Is Spatially Loaded** turned off so World Partition can't stream one out of the round's count; keep it off for any target you add.
- Tune without code: `RoundDurationSeconds` on `BP_FirstPersonGameMode`; `TraceRange` / `FireCooldown` on `BP_FirstPersonPlayerController`; `PopDuration` / `HitSoundPitch` / `FlashMaterial` on `BP_TargetRushTarget`; and target placement in the level.
- `LogTargetRush` logs `Round started: N targets, 60s`, `Target hit: ...`, `Round won: ...` and `Round lost: ...` — use these lines for smoke tests in PIE or a Development package.

## Gameplay content (First Person template)

How the Blueprint gameplay fits together:

- **Framework:** `BP_FirstPersonGameMode` (global default game mode, reparented onto `ATargetRushGameMode`) with `BP_FirstPersonCharacter` as its pawn and `BP_FirstPersonPlayerController` (reparented onto `ATargetRushPlayerController`) as its controller, plus `BP_FirstPersonCameraManager`, all in `Content/FirstPerson/Blueprints/`. First-person arm animation uses `FirstPerson/Anims/ABP_FP_Copy` with the `CtrlRig_FPWarp` control rig on the `Characters/Mannequins` skeleton and anims.
- **Input:** Enhanced Input (`EnhancedPlayerInput` / `EnhancedInputComponent` set in `DefaultInput.ini`). Mapping contexts `Input/IMC_Default` and `IMC_MouseLook` bind the actions in `Input/Actions/` (`IA_Move`, `IA_Look`, `IA_MouseLook`, `IA_Jump`) and are handled in `BP_FirstPersonCharacter`; Target Rush's fire/restart input is in `/Game/TargetRush/Input/`. On-screen touch controls are in `Input/Touch/` (there is no touch fire button yet).
- **Weapons:** `Weapons/{Pistol,Rifle,GrenadeLauncher}` hold meshes, materials, textures and audio only. There is no weapon Blueprint or weapon mesh on the character; firing is the hitscan in `ATargetRushPlayerController`.
- **Level prototyping:** `LevelPrototyping/Interactable/` contains ready-made actors (`BP_DoorFrame`, `BP_JumpPad`, `BP_WobbleTarget`) plus greybox meshes and materials. `BP_WobbleTarget` reacts to overlaps rather than shots and references a missing `/Game/DemoTemplate/_Core/MI_Intro_Colorway` material; use `BP_TargetRushTarget` for shootable targets.
- **Level storage:** `Lvl_FirstPerson` uses World Partition with One File Per Actor, so placed actors are stored as separate files under `Content/__ExternalActors__/FirstPerson/` (and actor folders under `__ExternalObjects__`), not inside the `.umap`. Never delete or rename those files by hand; edit the level in the editor or through MCP.

## Unreal MCP integration

The project enables the experimental engine plugins `ModelContextProtocol` and `EditorToolset` (plus `Terminal` and `ModelingToolsEditorMode`). While the editor is running, it serves an MCP endpoint at `http://127.0.0.1:8000/mcp`, registered in `.mcp.json` as `unreal-mcp` and enabled in `.claude/settings.local.json`.

- The `mcp__unreal-mcp__*` tools only work while the editor is open with this project loaded.
- Use `list_toolsets` → `describe_toolset` → `call_tool` to discover and invoke editor operations (asset/level/actor manipulation etc.) instead of guessing tool names. `ProgrammaticToolset.execute_tool_script` batches many calls into one Python script.
- Prefer these tools for editing `.uasset`/`.umap` content, which is binary and cannot be edited as text.

Pitfalls found while building Target Rush:

- **Run one editor per project.** A second instance can't bind port 8000 (so it has no MCP) and keeps package files open, which makes saves from the MCP editor fail with Windows error 32.
- **Recompile after setting Blueprint class defaults.** After `ObjectTools.set_properties` on a `Default__<BP>_C` object, run `compile_blueprint` again. Otherwise spawned instances don't receive new values for parent-class properties, because the list of properties copied at construction is only rebuilt on compile. `set_parent` also needs a compile before the new parent's properties can be set.
- **Array properties:** `set_properties` can't change existing elements and resize an array in the same call. Update elements at the current size first, then append in a second call. When you append, repeat existing elements exactly as `get_properties` returns them (e.g. an `FKey` as `"LeftMouseButton"`, an empty object reference as `"None"`).
- `IA_Jump` (and anything duplicated from it) has Pressed + Released triggers, so a `Started` binding fires on both press and release. Clear `Triggers` on copies that should fire once per press.
- `SceneTools.save_actor` fails for newly placed actors that don't exist on disk yet; use `AssetTools.save_assets` with an empty list to save all dirty packages.
- PIE in an editor window that isn't focused is throttled to a few frames per second with capped frame time, so game time runs slower than wall-clock time. Allow extra time for timer-based checks.
- Viewport captures and some schemas exceed the tool-result size and get saved to a file; decode the base64 image with `py`.
