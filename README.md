# firstproject

A first-person game prototype built in Unreal Engine 5.8. It started from the Blank C++ template, and Epic's First Person template content was added on top. It currently has one 60-second gameplay loop, **Target Rush**, with the rules in C++ and Blueprints for assets and tuning.

## Target Rush

You spawn on the central platform of the greybox arena with 60 seconds on the clock. Eight targets are spread around the arena: some in plain sight, some tucked under the platform's edges, and some up on corner blocks and the wall rim. Shoot all eight before time runs out.

- **Hit:** the target flashes, pops, plays a sound, the crosshair shows a hitmarker, and the counter goes up.
- **Win:** all 8 down in time — the banner shows your clear time.
- **Lose:** the clock hits 0 — the banner shows how many you got.
- When the round ends, movement freezes. Press **R** (or gamepad **Start**) to play again.

## Controls

| Action | Keyboard / mouse | Gamepad |
| --- | --- | --- |
| Move | WASD | Left stick |
| Look | Mouse | Right stick |
| Jump | Space | A / Cross |
| Fire | Left mouse button | Right trigger |
| Restart (after a round ends) | R | Start |

Movement bindings are in `Content/Input/IMC_Default` and `IMC_MouseLook`; fire and restart are in `Content/TargetRush/Input/IMC_TargetRush`.

## Requirements

- Windows 10/11
- Unreal Engine **5.8** (installed through the Epic Games Launcher)
- Visual Studio 2022 with the "Game development with C++" workload (`.vsconfig` in the repo root lists the required components)
- [Git LFS](https://git-lfs.com/): binary assets (`.uasset`, `.umap`, textures, audio) are stored in LFS. Run `git lfs install --local` after cloning.
- A DirectX 12 GPU that supports SM6. The project uses hardware ray tracing, Lumen and virtual shadow maps, so a recent graphics card is recommended.

## Getting started

1. Double-click `firstproject.uproject`. If asked to rebuild the `firstproject` module, choose **Yes**.
2. The editor opens `Content/FirstPerson/Lvl_FirstPerson`. Press **Play** to try it.

Open only one editor per project at a time; a second instance locks asset files and stops saves in the first.

To work on C++ code, right-click `firstproject.uproject` → **Generate Visual Studio project files**, then open `firstproject.sln`.

## Building from the command line

Close the editor before building the editor target from the command line, or the module DLL will be locked. While the editor is open, use Live Coding (Ctrl+Alt+F11) for changes to existing functions; new classes or properties need a full build with the editor closed.

```powershell
$UE = "C:\Program Files\Epic Games\UE_5.8"

# Editor build
& "$UE\Engine\Build\BatchFiles\Build.bat" firstprojectEditor Win64 Development -Project="$PWD\firstproject.uproject" -WaitMutex

# Standalone game build
& "$UE\Engine\Build\BatchFiles\Build.bat" firstproject Win64 Development -Project="$PWD\firstproject.uproject" -WaitMutex
```

## Tests

Run the automation tests (editor closed):

```powershell
& "$UE\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "$PWD\firstproject.uproject" -ExecCmds="Automation RunTests Project." -TestExit="Automation Test Queue Empty" -unattended -nop4 -nosplash -NullRHI -log
```

Results are written to `Saved/Logs/firstproject.log`.

## Packaging

```powershell
# Development build (logging and console enabled)
& "$UE\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="$PWD\firstproject.uproject" -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive -archivedirectory="$PWD\Build\Development" -utf8output

# Shipping build: same command with -clientconfig=Shipping -archivedirectory="$PWD\Build\Shipping"
```

Run `Build\<Config>\Windows\firstproject.exe`. The first cook compiles a lot of shaders and can take a long time.

## Project layout

| Path | Contents |
| --- | --- |
| `Source/firstproject/` | C++ game module |
| `Source/firstproject/TargetRush/` | Target Rush rules, game mode, player controller, target and HUD |
| `Source/firstproject/Tests/` | Automation tests |
| `Config/` | Project settings: rendering, input, the default map and game mode, and packaging |
| `Content/FirstPerson/` | Main level (`Lvl_FirstPerson`), the game mode, character, player controller and camera manager Blueprints, and first-person animation |
| `Content/TargetRush/` | Target Blueprint and the fire/restart input assets |
| `Content/Input/` | Enhanced Input mapping contexts and actions (move, look, jump), plus touch controls |
| `Content/Characters/Mannequins/` | Mannequin meshes and animations |
| `Content/Weapons/` | Pistol, rifle and grenade launcher art assets |
| `Content/LevelPrototyping/` | Greybox meshes and materials, plus interactive actors (door, jump pad, wobble target) |
| `Content/__ExternalActors__/` | Actors placed in the level, one file each (World Partition). Don't edit these by hand. |
| `Build/` | Packaged builds (generated, not committed) |

Everything under `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, `Build/`, and `.vs/`, and the generated `.sln`/`.slnx` files, can be regenerated and isn't committed.

## AI-assisted editing (Unreal MCP)

The project enables Unreal's experimental **ModelContextProtocol** and **EditorToolset** plugins. While the editor is running, it serves an MCP server at `http://127.0.0.1:8000/mcp`. `.mcp.json` registers that server so Claude Code can inspect and edit levels and assets through the editor. It only works while the editor is open with this project loaded. See `CLAUDE.md` for details.
