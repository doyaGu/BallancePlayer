# BallancePlayer

## Overview

BallancePlayer is a new player for the game "Ballance" created by decompiling and improving the original version. It enhances the gaming experience with several modern features and optimizations.

## Features

- Out-of-the-box and portable, zero-configuration to start game
- Support .ini file configurations
- Support command-line options
- Support 32-bit color mode
- Support the monitor without 640x480 resolution
- Support high refresh rate monitor
- Support in-game resolution switching
- Add hotkeys for switching fullscreen and closing game
- No registry operation
- No dependency on `Dsetup.dll`
- Integrated `ResDll.dll`
- Fix some bugs that exist in the original player
- Performance improvement

## Environment

Supports Windows XP, Vista, 7, 8, 8.1, 10, 11.

## Instructions

1. Extract the package into `Bin` directory of your game folder.
2. Start the game by clicking on `Player.exe` without setting compatibility mode.

## Building from Source

### Pre-requisites

Virtools SDK: Required for building the project. Obtainable from [Virtools-SDK-2.1](https://github.com/doyaGu/Virtools-SDK-2.1).

Please set the environment variable VIRTOOLS_SDK_PATH to the path of the Virtools SDK before building the project.

### Building with CMake

- Install CMake: Ensure CMake is installed on your system.
- Navigate to Project Directory: Change to the directory where the BallancePlayer source code resides.
- Run the Command in Console: `cmake -B build -G "Visual Studio 16 2022" -A Win32`. This command generates Visual Studio project files and solutions tailored for 32-bit architecture.
- Open Solution in Visual Studio: Navigate to the `build` directory, then open `BallancePlayer.sln`.
- Build the Solution: Use Visual Studio's build tools to compile the project.

### Building with Visual Studio 6.0

- Install Visual Studio 6.0: Ensure it's installed on your system.
- Open the Project: Locate `Player.dsw` in the project directory and open it with Visual Studio 6.0.
- Build the Project: Use the build tools in Visual Studio 6.0 to compile the project.

### Notes

The release package is built with Visual Studio 6.0 for maximum compatibility.

## Hotkeys

- **[Alt] + [Enter]**: Switch between windowed and fullscreen mode.
- **[Alt] + [F4]**:  Force close the game.
- **[Alt] + [/]**:  Display an about box.

## INI Settings

There are several settings in `Player.ini`.

### Startup

- `ManualSetup`:
  - Disable: `0`
  - Enable: `1`
- `LoadAllManagers`:
  - Disable: `0`
  - Enable: `1`
- `LoadAllBuildingBlocks`:
  - Disable: `0`
  - Enable: `1`
- `LoadAllPlugins`:
  - Disable: `0`
  - Enable: `1`

### Graphics

- `Driver`:
  - Driver ID: `The id of the graphics card driver to display`
- `BitsPerPixel`:
  - 32-bit: `32`
  - 16-bit: `16`
- `Width`:
  - Screen Width: `The screen width which can be found in your monitor resolutions`
- `Height`:
  - Screen Height: `The screen height which can be found in your monitor resolutions`
- `FullScreen`:
  - Windowed Mode: `0`
  - FullScreen Mode: `1`
- `UnlockFramerate`:
  - Lock: `0`
  - Unlock: `1`
- `UnlockWidescreen`:
  - Lock: `0`
  - Unlock: `1`
- `UnlockHighResolution`:
  - Lock: `0`
  - Unlock: `1`
- `Antialias`:
  - Anti-aliasing Level: `The number of multi-sample for anti-aliasing (minimum: 2)`
- `DisableFilter`:
  - OFF: `0`
  - ON: `1`
- `DisableDithering`:
  - OFF: `0`
  - ON: `1`
- `DisableMipmap`:
  - OFF: `0`
  - ON: `1`
- `DisableSpecular`:
  - OFF: `0`
  - ON: `1`

### Window

- `Borderless`:
  - Disable: `0`
  - Enable: `1`
- `AlwaysHandleInput`
  - Disable: `0`
  - Enable: `1`
- `PauseOnDeactivated`
  - Disable: `0`
  - Enable: `1`
- `X`:
  - Window Horizontal Coordinate: `Any integer between negative window width and screen width`
- `Y`:
  - Window Vertical Coordinate: `Any integer between negative window height and screen height`

### Game

- `Language`:
  - German: `0`
  - English: `1`
  - Spanish: `2`
  - Italian: `3`
  - French: `4`
- `SkipOpening`:
  - Disable: `0`
  - Enable: `1`

## Command-line Options

```bash
Player.exe [OPTIONS]
```

- `-m`, `--manual-setup`: Always show setup dialog box at startup
- `--load-all-managers`: Control whether player loads all managers
- `--load-all-building-blocks`: Control whether player loads all building blocks
- `--load-all-plugins`: Control whether player loads all plugins
- `-v <driver>`, `--video-driver <driver>`: Set the id of the graphics card driver to display
- `-b <bpp>`, `--bpp <bpp>`: Set the bit per pixel of the screen (32 or 16)
- `-w <width>`, `--width <width>`: Set the screen width
- `-h <height>`, `--height <height>`: Set the screen height
- `-f`, `--fullscreen`: Startup in fullscreen mode
- `-u`, `--unlock-framerate`: Unlock the frame rate limitation
- `--unlock-widescreen`: Unlock non-4:3 resolutions
- `--unlock-high-resolution`: Unlock resolutions higher than 1600x1200
- `--antialias <level>`: Enable image antialiasing (minimum: 2)
- `--disable-filter`: Disable texture filtering
- `--disable-dithering`: Disable image dithering
- `--disable-mipmap`: Disable mipmap
- `--disable-specular`: Disable specular highlights
- `-c`, `--borderless`: Startup in borderless mode
- `--always-handle-input`: Allow player handle input while window is in background
- `-p`, `--pause-on-deactivated`: Pause the game once the window is deactivated
- `-x <X>`, `--position-x <X>`: Set the position x of the window
- `-y <Y>`, `--position-y <Y>`: Set the position y of the window
- `-l <lang>`, `--lang <lang>`: Set the language id
- `--skip-opening`: Skip the opening animation

## Contact

If you have any bugs or requests, please open an issue in this repository: [BallancePlayer](https://github.com/doyaGu/BallancePlayer).

## ChangeLog

### v0.3.0 (2023-05-20)

**News**

- Add a new setting `ChildWindowRendering` and corresponding command-line option `--child-window-rendering` to control whether to render in a child window.

**Bug Fixes**

- Fix a possible black screen problem during full-screen switching.

**Changed**

- No longer to rendering in a separate window by default.

### v0.2.4 (2023-03-23)

**Bug Fixes**

- Fix game initialization failure when no screen mode is selected in setup dialog.
- Fix improper render driver initialization process.

### v0.2.3 (2023-03-13)

**News**

- Allow to run multiple instance.

**Bug Fixes**

- Fix the bug that the game crashes upon exit.

**Changed**

- Deprecate `Resizable`.
- Deprecate `ClipMouse`.
- Improve error messages.
- Refactor codebase to simplify implementation.

### v0.2.2 (2023-01-23)

**News**

- Add a new setting `ManualSetup` and corresponding command-line option `--manual-setup` to control whether to show setup dialog box at startup.

**Bug Fixes**

- Fix broken command line short option parsing.

**Changed**

- Deprecate `AdaptiveCamera`.
- Deprecate delay-loaded DLL.
- Deprecate path customization.
- Simplify the workaround for enumeration of driver and screen mode.

### v0.2.1 (2022-11-24)

**News**

- Support console log.
- Add a new setting `LoadAllManagers` and corresponding command-line option `--load-all-managers` to control whether player loads all managers.
- Add a new setting `LoadAllBuildingBlocks` and corresponding command-line option `--load-all-building-blocks` to control whether player loads all building blocks.
- Add a new setting `LoadAllPlugins` and corresponding command-line option `--load-all-plugins` to control whether player loads all plugins.

**Bug Fixes**

- Fix the problem that player changes screen mode incorrectly.
- Fix the problem that player can not startup if a wrong path setting exists.

**Changed**

- Remove dependency on the modified version of `TT_InterfaceManager_RT.dll`. Avoid to modify original files.

### v0.2.0 (2022-10-15)

**News**

- Add the Chinese version of README.

### v0.2.0-rc1 (2022-10-09)

**Bug Fixes**

- Fix possible crash problem upon exit.

**Changed**

- Improve game loop.

### v0.2.0-beta4 (2022-10-02)

**Bug Fixes**

- Fix the problem that player can not switch between fullscreen and windowed mode when using OpenGL.
- Fix error-prone task switching.

### v0.2.0-beta3 (2022-10-01)

**Bug Fixes**

- Fix the problem that UI will crash on task switching in fullscreen mode.
- Fix the problem that specifying a display driver has no effect.

**Changed**

- Swap the positions of `Driver` and `Screen Mode` in FullScreen Setup Dialog.

### v0.2.0-beta2 (2022-09-28)

**Bug Fixes**

- Fix pop-up black screen after exiting.

### v0.2.0-beta1 (2022-09-28)

**Bug Fixes**

- Fix forced fullscreen Vsync.
- Fix window position restoration.

### v0.2.0-alpha4 (2022-09-25)

**Bug Fixes**

- Fix the black screen error on the monitor which is not support 640x480 resolution.

### v0.2.0-alpha2 (2022-09-23)

**News**

- Support in-game debug mode.
- Support new command line options style like `--root-path=<dir>`.

### v0.2.0-alpha1 (2022-09-17)

**News**

- Support more resolutions.
- Support skipping the opening animation.
- Support mouse clip.
- Support off-site startup.
- Support game path customization.
- Support Virtools console output.
- Add many new settings and corresponding command-line options.

**Bug Fixes**

- Fix the problematic fullscreen setup.
- Fix the problem that player will be stuck when using custom resolution.

**Changed**

- Unify log and error output.
- Rearrange settings and corresponding command-line options.

### v0.1.9 (2022-08-30)

**Bug Fixes**

- Fix the incorrect position saving mechanism when it comes to exiting full screen.

**Changed**

- If the window is resizable (window has a title), it shows in the upper-left corner with a title and a slim margin from the screen edge when the window position is set to 0.
- The saved value of window position can be negative.
- Save last game mode (fullscreen or windowed) and restore it next time the game startups.

### v0.1.8 (2022-08-17)

**Bug Fixes**

- Fix the bug that the game locks at 60 FPS on 144Hz monitor when v-sync is enabled.
- Fix the bug that the game can not find multiple drivers.

### v0.1.7 (2022-08-15)

**News**

- Add two new settings `X` and `Y`, and corresponding command-line options `-x` and `-y` to set the screen coordinate of the upper-left corner of the window.

**Bug Fixes**

- Fix the bug that the game window resizes unnaturally before starting.

**Changed**

- Save last window position and restore it next time the game startups.

### v0.1.6 (2022-08-05)

**News**

- Add a new setting `Borderless` and corresponding command-line option `-c` to startup in borderless mode.
- Add a new setting `Resizable` and corresponding command-line option `-s` to make window resizable.

**Changed**

- Make window not resizable by default.

### v0.1.5 (2022-07-20)

**News**

- Support 32-bit color mode.
- Add command-line long option support.
- Add a new setting `Language` and corresponding command-line option `-l` to set game language.
- Add a new setting `UnlockFramerate` and corresponding command-line option `-u` to unlock the frame-rate limitation.

**Bug Fixes**

- Fix the bug that the game UI will be disordered between task switching.

**Changed**

- Change the command-line option for disabling task switching from `-d` to `-e`.
- Reimplement initialization configuration mechanism.
- Rewritten the command-line support.
- Integrate the function of ResDll.

### v0.1.4 (2022-05-12)

**News**

- Add a new setting `PauseOnTaskSwitch` and corresponding command-line option `-p` to enable game pause on task switching.

**Bug Fixes**

- Fix the bug that the game exits early.
- Fix the bug that the game crashes upon exit.

**Changed**

- Remove unnecessary virtual functions to improve performance.
- Remove the Interface Sprite that will not be used in the game.
- Improve Error-Handling.

### v0.1.3 (2022-05-09)

**News**

- Support command-line options.

**Bug Fixes**

- Fix the bug that the game will crash in fullscreen mode.
- Fix the bug that the game will be stuck between task switching.
- Fix the bug that the game will show resolution duplicates in Graphics Options.

**Changed**

- Remove dependency on `Dsetup.dll` since it is normally needless to check the DirectX version on a modern PC.
- Disable Exception Handling to improve performance.

### v0.1.2 (2022-05-08)

**Bug Fixes**

- Fix the bug that the game maybe crashes when upon exit.

**Changed**

- Change to generate default config file when no configuration file is found.
- Replace the remaining registry operation in the main function.
- Disable Exception Handling to improve performance.
- Remove an unnecessary class `FixedString`.

### v0.1.1 (2022-05-04)

**News**

- Support initialization file configuration.

**Bug Fixes**

- Fix the bug that the game displays incompletely in windowed mode.

**Changed**

- Remove some unknown class members.

### v0.1.0 (2022-05-03)

**Bug Fixes**

- Fix the bug that the memory checking gets the wrong result.
