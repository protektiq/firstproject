# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

`firstproject` is an Unreal Engine **5.8** project created from the Blank C++ template (config still has `TP_Blank` → `firstproject` redirects), with Epic's **First Person template content pack added on top**. All gameplay currently lives in **Blueprints and assets**, not C++: the single runtime module `firstproject` only contains the default `IMPLEMENT_PRIMARY_GAME_MODULE` boilerplate. Gameplay changes therefore mostly happen in `.uasset`/`.umap` files (see Unreal MCP integration below); C++ is only needed when adding new native classes.

- Engine: launcher install at `C:\Program Files\Epic Games\UE_5.8` (the `EngineAssociation` GUID in `firstproject.uproject` maps to this path via `HKCU\SOFTWARE\Epic Games\Unreal Engine\Builds`).
- Not a git repository.
- `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, `.vs/`, and the `.sln`/`.slnx` files are generated — don't edit them by hand.

## Build and run

Close the editor (or use Live Coding with Ctrl+Alt+F11 in-editor) before building the editor target from the command line, otherwise the DLL is locked.

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

No project tests exist yet. Unreal automation tests run through the editor commandlet:

```powershell
# Run tests whose name starts with a filter (e.g. "Project." or a specific test path)
& "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "D:\firstproject\firstproject.uproject" -ExecCmds="Automation RunTests Project.; Quit" -unattended -nop4 -nosplash -NullRHI -log
```

Results land in `Saved/Logs/firstproject.log`.

## Code layout

- `firstproject.uproject` — module list and enabled plugins.
- `Source/firstproject.Target.cs` / `Source/firstprojectEditor.Target.cs` — Game and Editor targets (`BuildSettingsVersion.V7`, `EngineIncludeOrderVersion.Unreal5_8`).
- `Source/firstproject/firstproject.Build.cs` — module dependencies: `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`. Add new engine modules here (e.g. `Slate`/`SlateCore`, `UMG`) when code needs them.
- `Config/DefaultEngine.ini` `[GameMapsSettings]` — game and editor startup map is `/Game/FirstPerson/Lvl_FirstPerson`; global default game mode is `BP_FirstPersonGameMode`.
- `Config/Default*.ini` — project settings. Rendering is configured for high-end desktop: DX12 / SM6, Lumen GI and reflections, virtual shadow maps, hardware ray tracing, Substrate materials, static lighting disabled.

## Gameplay content (First Person template)

How the Blueprint gameplay fits together:

- **Framework:** `BP_FirstPersonGameMode` (global default game mode) and, by template convention, its `BP_FirstPersonCharacter`, `BP_FirstPersonPlayerController`, and `BP_FirstPersonCameraManager` classes, all in `Content/FirstPerson/Blueprints/` (confirm the class defaults in the editor before relying on them). First-person arm animation uses `FirstPerson/Anims/ABP_FP_Copy` with the `CtrlRig_FPWarp` control rig on the `Characters/Mannequins` skeleton and anims.
- **Input:** Enhanced Input (`EnhancedPlayerInput` / `EnhancedInputComponent` set in `DefaultInput.ini`). Mapping contexts `Input/IMC_Default` and `IMC_MouseLook` bind the actions in `Input/Actions/` (`IA_Move`, `IA_Look`, `IA_MouseLook`, `IA_Jump`). On-screen touch controls are in `Input/Touch/`. To add an input, create an `IA_*` action, map it in an IMC, and handle it in the character or controller Blueprint.
- **Weapons:** `Weapons/{Pistol,Rifle,GrenadeLauncher}` hold meshes, materials, textures and audio only (no weapon Blueprints), so any firing logic has to live in, or be added to, the FirstPerson Blueprints.
- **Level prototyping:** `LevelPrototyping/Interactable/` contains ready-made actors (`BP_DoorFrame`, `BP_JumpPad`, `BP_WobbleTarget`) plus greybox meshes and materials.
- **Level storage:** `Lvl_FirstPerson` uses World Partition with One File Per Actor, so placed actors are stored as separate files under `Content/__ExternalActors__/FirstPerson/` (and `__ExternalObjects__`), not inside the `.umap`. Never delete or rename those files by hand; edit the level in the editor or through MCP.

## Unreal MCP integration

The project enables the experimental engine plugins `ModelContextProtocol` and `EditorToolset` (plus `Terminal` and `ModelingToolsEditorMode`). While the editor is running, it serves an MCP endpoint at `http://127.0.0.1:8000/mcp`, registered in `.mcp.json` as `unreal-mcp` and enabled in `.claude/settings.local.json`.

- The `mcp__unreal-mcp__*` tools only work while the editor is open with this project loaded.
- Use `list_toolsets` → `describe_toolset` → `call_tool` to discover and invoke editor operations (asset/level/actor manipulation etc.) instead of guessing tool names.
- Prefer these tools for editing `.uasset`/`.umap` content, which is binary and cannot be edited as text.
