# firstproject

A first-person game prototype built in Unreal Engine 5.8. It started from the Blank C++ template, and Epic's First Person template content was added on top. So far all gameplay is in Blueprints and assets; the C++ module only has the default setup code.

## Requirements

- Windows 10/11
- Unreal Engine **5.8** (installed through the Epic Games Launcher)
- Visual Studio 2022 with the "Game development with C++" workload (`.vsconfig` in the repo root lists the required components)
- A DirectX 12 GPU that supports SM6. The project uses hardware ray tracing, Lumen and virtual shadow maps, so a recent graphics card is recommended.

## Getting started

1. Double-click `firstproject.uproject`. If asked to rebuild the `firstproject` module, choose **Yes**.
2. The editor opens `Content/FirstPerson/Lvl_FirstPerson`. Press **Play** to try it.

To work on C++ code, right-click `firstproject.uproject` → **Generate Visual Studio project files**, then open `firstproject.sln`.

## Building from the command line

Close the editor before building the editor target from the command line, or the module DLL will be locked. While the editor is open, use Live Coding (Ctrl+Alt+F11) instead.

```powershell
$UE = "C:\Program Files\Epic Games\UE_5.8"

# Editor build
& "$UE\Engine\Build\BatchFiles\Build.bat" firstprojectEditor Win64 Development -Project="$PWD\firstproject.uproject" -WaitMutex

# Standalone game build
& "$UE\Engine\Build\BatchFiles\Build.bat" firstproject Win64 Development -Project="$PWD\firstproject.uproject" -WaitMutex
```

## Project layout

| Path | Contents |
| --- | --- |
| `Source/firstproject/` | C++ game module (currently just the default module setup code) |
| `Config/` | Project settings: rendering, input, and the default map and game mode |
| `Content/FirstPerson/` | Main level (`Lvl_FirstPerson`), the game mode, character, player controller and camera manager Blueprints, and first-person animation |
| `Content/Input/` | Enhanced Input mapping contexts and actions (move, look, jump), plus touch controls |
| `Content/Characters/Mannequins/` | Mannequin meshes and animations |
| `Content/Weapons/` | Pistol, rifle and grenade launcher art assets |
| `Content/LevelPrototyping/` | Greybox meshes and materials, plus interactive actors (door, jump pad, wobble target) |
| `Content/__ExternalActors__/` | Actors placed in the level, one file each (World Partition). Don't edit these by hand. |

Everything under `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, and `.vs/`, and the generated `.sln`/`.slnx` files, can be regenerated and shouldn't be committed.

## Controls

Enhanced Input with the First Person template's defaults: move, look (mouse or gamepad), and jump. On touch devices there are on-screen controls. The key bindings are set in `Content/Input/IMC_Default` and `IMC_MouseLook`.

## AI-assisted editing (Unreal MCP)

The project enables Unreal's experimental **ModelContextProtocol** and **EditorToolset** plugins. While the editor is running, it serves an MCP server at `http://127.0.0.1:8000/mcp`. `.mcp.json` registers that server so Claude Code can inspect and edit levels and assets through the editor. It only works while the editor is open with this project loaded. See `CLAUDE.md` for details.
