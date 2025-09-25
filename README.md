# BallancePlayer

## Overview

BallancePlayer is a modern, enhanced player for the game Ballance, developed by decompiling and improving the original version. It offers various new features and optimizations to deliver an improved gaming experience while maintaining compatibility with the original game.

## Features

- Portable and ready to use, no configuration needed to start the game
- Support for `.ini` file configurations
- Command-line options for flexibility
- 32-bit color mode support
- Compatibility with monitors that don't support 640x480 resolution
- High refresh rate monitor support
- In-game resolution switching support
- Hotkeys for toggling fullscreen mode and closing the game
- No registry modifications
- No dependency on `Dsetup.dll`
- Integrated `ResDll.dll` for seamless operation
- Fixes for bugs present in the original player
- Various performance improvements

## System Requirements

Supports the following Windows versions:
- Windows XP
- Windows Vista
- Windows 7
- Windows 8 / 8.1
- Windows 10
- Windows 11

## Installation

1. Extract the provided package into the `Bin` directory of your game folder.
2. Launch the game by running `Player.exe`. No need to set compatibility mode.

## Building from Source

### Prerequisites

To build BallancePlayer, you’ll need the Virtools SDK, which can be obtained from [Virtools-SDK-2.1](https://github.com/doyaGu/Virtools-SDK-2.1). Set the `VIRTOOLS_SDK_PATH` environment variable to the path where the SDK is installed before starting the build process.

### Building with CMake

1. **Install CMake**: Ensure CMake is installed on your system.
2. **Navigate to Project Directory**: Open a console and navigate to the directory containing the BallancePlayer source code.
3. **Generate Build Files**: Run the following command to generate Visual Studio project files for 32-bit architecture:
   ```
   cmake -B build -G "Visual Studio 16 2022" -A Win32
   ```
4. **Open in Visual Studio**: Navigate to the `build` directory and open the solution file `BallancePlayer.sln` in Visual Studio.
5. **Build the Solution**: Use Visual Studio to compile the project.

### Building with Visual Studio 6.0

1. **Install Visual Studio 6.0**: Ensure Visual Studio 6.0 is installed.
2. **Open the Project**: Locate `Player.dsw` in the project directory and open it with Visual Studio 6.0.
3. **Build the Project**: Use the build tools in Visual Studio 6.0 to compile the project.

### Notes

The official release package is built with Visual Studio 6.0 for maximum compatibility with older systems.

## Hotkeys

- **[Alt] + [Enter]**: Switch between windowed and fullscreen mode.
- **[Alt] + [F4]**:  Force close the game.
- **[Alt] + [/]**:  Display an about box.

## INI Settings

The `Player.ini` file contains several settings that control the behavior of the game.

### Startup

- `LogMode`: Controls how logs are handled.
  - `0`: Append to the log file.
  - `1`: Overwrite the log file.
- `Verbose`: Toggles verbose logging.
  - `0`: Disabled.
  - `1`: Enabled.
- `ManualSetup`: Controls whether the setup dialog appears on startup.
  - `0`: Disabled.
  - `1`: Enabled.

### Graphics

- `Driver`: Specifies the driver ID of the graphics card to use.
- `BitsPerPixel`: Specifies color depth.
  - `32`: 32-bit color.
  - `16`: 16-bit color.
- `Width`: Specifies screen width (based on your monitor resolution).
- `Height`: Specifies screen height (based on your monitor resolution).
- `FullScreen`: Controls display mode.
  - `0`: Windowed mode.
  - `1`: Fullscreen mode.
- `DisablePerspectiveCorrection`: Toggles perspective correction.
  - `0`: Off.
  - `1`: On.
- `ForceLinearFog`: Forces linear fog modes.
  - `0`: Off.
  - `1`: On.
- `ForceSoftware`: Disables hardware rendering.
  - `0`: Off.
  - `1`: On.
- `DisableFilter`: Disables texture filtering.
  - `0`: Off.
  - `1`: On.
- `EnsureVertexShader`: Ensures vertex shader support.
  - `0`: Off.
  - `1`: On.
- `UseIndexBuffers`: Enables index buffer usage.
  - `0`: Off.
  - `1`: On.
- `DisableDithering`: Disables dithering.
  - `0`: Off.
  - `1`: On.
- `Antialias`: Sets the level of anti-aliasing (minimum value: 2).
- `DisableMipmap`: Disables mipmaps.
  - `0`: Off.
  - `1`: On.
- `DisableSpecular`: Disables specular highlights.
  - `0`: Off.
  - `1`: On.
- `EnableScreenDump`: Enables screen dumping functionality.
  - `0`: Off.
  - `1`: On.
- `EnableDebugMode`: Enables debug mode for step-by-step rendering.
  - `0`: Off.
  - `1`: On.
- `VertexCache`: Sets the size of the vertex cache. A value of `0` disables sorting.
- `TextureCacheManagement`: Enables or disables texture cache management.
  - `0`: Off.
  - `1`: On.
- `SortTransparentObjects`: Toggles sorting of transparent objects.
  - `0`: Off.
  - `1`: On.
- `TextureVideoFormat`: Specifies the default pixel format for textures (e.g., `_32_ARGB8888`).
- `SpriteVideoFormat`: Specifies the default pixel format for sprites (e.g., `_32_ARGB8888`).

### Window

- `ChildWindowRendering`: Controls whether rendering occurs in a child window.
  - `0`: Disabled.
  - `1`: Enabled.
- `Borderless`: Controls window borders.
  - `0`: Disabled.
  - `1`: Enabled.
- `ClipCursor`: Controls whether the cursor is clipped to the window.
  - `0`: Disabled.
  - `1`: Enabled.
- `AlwaysHandleInput`: Allows the player to handle input even when the window is in the background.
  - `0`: Disabled.
  - `1`: Enabled.
- `X`: Specifies the horizontal window coordinate (can be negative).
- `Y`: Specifies the vertical window coordinate (can be negative).

### Game

- `Language`: Sets the game language.
  - `0`: German.
  - `1`: English.
  - `2`: Spanish.
  - `3`: Italian.
  - `4`: French.
- `SkipOpening`: Controls whether to skip the opening animation.
  - `0`: Disabled.
  - `1`: Enabled.
- `ApplyHotfix`: Controls whether hotfixes are applied.
  - `0`: Disabled.
  - `1`: Enabled.
- `UnlockFramerate`: Unlocks the frame rate limitation.
  - `0`: Disabled.
  - `1`: Enabled.
- `UnlockWidescreen`: Unlocks non-4:3 resolutions.
  - `0`: Disabled.
  - `1`: Enabled.
- `UnlockHighResolution`: Unlocks resolutions higher than 1600x1200.
  - `0`: Disabled.
  - `1`: Enabled.
- `Debug`: Enables in-game debug mode.
  - `0`: Disabled.
  - `1`: Enabled.
- `Rookie`: Enables in-game rookie mode.
  - `0`: Disabled.
  - `1`: Enabled.

## Command-line Options

You can also use command-line options to customize game behavior:

```bash
Player.exe [OPTIONS]
```
- `--verbose`: Enable verbose logging.
- `-m`, `--manual-setup`: Always show the setup dialog box at startup.
- `-v <driver>`, `--video-driver <driver>`: Set the graphics card driver ID.
- `-b <bpp>`, `--bpp <bpp>`: Set the bits per pixel (32 or 16).
- `-w <width>`, `--width <width>`: Set the screen width.
- `-h <height>`, `--height <height>`: Set the screen height.
- `-f`, `--fullscreen`: Start the game in fullscreen mode.
- `--disable--perspective-correction`: Disable perspective correction.
- `--force-linear-fog`: Force the fog mode to linear.
- `--force-software`: Disable hardware rendering and force software mode.
- `--disable-filter`: Disable texture filtering.
- `--ensure-vertex-shader`: Ensure vertex shader support.
- `--use-index-buffers`: Enable index buffer usage.
- `--disable-dithering`: Disable image dithering.
- `--antialias <level>`: Enable image antialiasing (minimum: 2).
- `--disable-mipmap`: Disable mipmaps.
- `--disable-specular`: Disable specular highlights.
- `--enable-screen-dump`: Dump screen content using (CTRL + ALT + F10).
- `--enable-debug-mode`: Enable debug mode using (CTRL + ALT + F11).
- `--vertex-cache <size>`: Set the vertex cache size (0 disables sorting).
- `--disable-texture-cache-management`: Disable texture cache management.
- `--disable-sort-transparent-objects`: Disable sorting of transparent objects.
- `--texture-video-format <pixel-format>`: Set the texture pixel format (e.g., `_32_ARGB8888`).
- `--sprite-video-format <pixel-format>`: Set the sprite pixel format (e.g., `_32_ARGB8888`).
- `-s`, `--child-window-rendering`:Enable child window rendering.
- `-c`, `--borderless`: Start the game in borderless mode.
- `--clip-cursor`: Clip the cursor to the window.
- `--always-handle-input`:  Allow input handling when the window is in the background.
- `-x <X>`, `--position-x <X>`: Set the window's X coordinate.
- `-y <Y>`, `--position-y <Y>`: Set the window's Y coordinate.
- `-l <lang>`, `--lang <lang>`: Set the game language.
- `--skip-opening`: Skip the opening animation.
- `--disable-hotfix`: Disable script hotfixes.
- `-u`, `--unlock-framerate`: Unlock the frame rate limitation.
- `--unlock-widescreen`: Unlock non-4:3 resolutions.
- `--unlock-high-resolution`: Unlock resolutions higher than 1600x1200.
- `d`, `--debug`: Enable in-game debug mode.
- `r`, `--rookie`: Enable in-game rookie mode.

## Contact

If you have any bugs or requests, please open an issue in this repository: [BallancePlayer](https://github.com/doyaGu/BallancePlayer).

## ChangeLog

### v0.3.8 (2025-09-25)

**Bug Fixes**

- Fixed an issue where the window became invisible when switching from fullscreen to windowed mode

### v0.3.7 (2025-09-23)

**New Features**

- Added support for external configuration file change detection and merging.

**Bug Fixes**

- Fixed an issue where plugin calls to ClipCursor could be disrupted when the ClipCursor setting was disabled.

### v0.3.6 (2025-08-20)

**Changes**

- Switched from ANSI to Unicode to improve compatibility and internationalization support.

### v0.3.5 (2025-05-23)

**New Features**

- Introduced a standalone configuration tool for easier management of game settings.
- Added support for loading custom game compositions.

**Bug Fixes**

- Fixed a crash that could occur when enabling verbose logging.

**Changes**

- Removed deprecated settings: `LoadAllManagers`, `LoadAllBuildingBlocks`, and `LoadAllPlugins`.
- The game no longer pauses by default when the window is deactivated.

### v0.3.4 (2025-04-24)

**New Features**

- Added support for DPI awareness handling to improve display scaling across different monitor configurations.

**Bug Fixes**

- Fixed an issue with the command line parser that could cause incorrect parameter handling.
- Resolved an irregular game acceleration issue that occurred when frequently switching between windowed and fullscreen modes.

**Changes**

- Removed the deprecated `PauseOnDeactivated` setting.
- Enhanced error handling with more descriptive messages and improved recovery from common failure states.

### v0.3.3 (2024-10-01)

**New Features**

- Added the `LogMode` setting to control log mode.
- Added the `Verbose` setting with the `--verbose` option to control debug log output.
- Introduced the `ApplyHotfix` setting with the `--no-hotfix` option to control whether script hotfixes are applied.
- Added the `ClipCursor` setting with the `--clip-cursor` option to manage cursor clipping behavior.
- Added more graphics settings and options.
- Improved error messaging for better clarity.

**Changes**

- Logs now overwrite the existing file instead of appending to it.
- Reorganized configuration categories for better structure.
- Where possible, relative paths are now used.

### v0.3.2 (2024-07-16)

**Bug Fixes**

- Fixed an issue with broken plugin registration.

**Changes**

- The application window now opens in the center of the screen by default.

### v0.3.1 (2024-03-04)

**Bug Fixes**

- Fixed a potential issue with abnormal resolution settings.

### v0.3.0 (2023-05-20)

**New Features**

- Added the `ChildWindowRendering` setting with the `--child-window-rendering` option to control rendering in a child window.

**Bug Fixes**

- Resolved a possible black screen issue when switching to full-screen mode.

**Changes**

- By default, rendering is no longer done in a separate window.

### v0.2.4 (2023-03-23)

**Bug Fixes**

- Fixed a game initialization failure that occurred when no screen mode was selected in the setup dialog.
- Corrected the render driver initialization process.

### v0.2.3 (2023-03-13)

**New Features**

- Added support for running multiple instances of the game.

**Bug Fixes**

- Fixed a crash that occurred when exiting the game.

**Changes**

- Deprecated the `Resizable` and `ClipMouse` settings.
- Enhanced error messages for better clarity.
- Refactored the codebase to simplify the implementation.

### v0.2.2 (2023-01-23)

**New Features**

- Added the `ManualSetup` setting and corresponding `--manual-setup` command-line option to control whether the setup dialog box is shown at startup.

**Bug Fixes**

- Fixed issues with short option parsing in the command line.

**Changes**

- Deprecated the `AdaptiveCamera` setting.
- Deprecated delay-loaded DLL.
- Deprecated path customization.
- Simplified the workaround for driver and screen mode enumeration.

### v0.2.1 (2022-11-24)

**New Features**

- Added support for console logging.
- Added the `LoadAllManagers` setting with the `--load-all-managers` command-line option to control whether all managers are loaded.
- Added the `LoadAllBuildingBlocks` setting with the `--load-all-building-blocks` option to control whether all building blocks are loaded.
- Added the `LoadAllPlugins` setting with the `--load-all-plugins` command-line option to control whether all plugins are loaded.

**Bug Fixes**

- Fixed an issue where the screen modes will be changed incorrectly.
- Resolved a startup failure caused by incorrect path settings.

**Changes**

- Removed the dependency on the modified version of `TT_InterfaceManager_RT.dll`.

### v0.2.0 (2022-10-15)

**New Features**

- Added a Chinese version of the README.

### v0.2.0-rc1 (2022-10-09)

**Bug Fixes**

- Fixed a potential crash issue that occurred upon exiting the game.

**Changes**

- Improved the game loop for better performance.

### v0.2.0-beta4 (2022-10-02)

**Bug Fixes**

- Fixed an issue preventing the player from switching between fullscreen and windowed mode when using OpenGL.
- Resolved problems with task switching that could lead to errors.

### v0.2.0-beta3 (2022-10-01)

**Bug Fixes**

- Fixed a UI crash that occurred during task switching in fullscreen mode.
- Resolved an issue where specifying a display driver had no effect.

**Changes**

- Swapped the positions of `Driver` and `Screen Mode` in the `FullScreen Setup` dialog.

### v0.2.0-beta2 (2022-09-28)

**Bug Fixes**

- Fixed a pop-up black screen issue after exiting.

### v0.2.0-beta1 (2022-09-28)

**Bug Fixes**

- Fixed forced fullscreen Vsync.
- Fixed the issue that window position can not be restored correctly.

### v0.2.0-alpha4 (2022-09-25)

**Bug Fixes**

- Resolved a black screen error on monitors that do not support 640x480 resolution.

### v0.2.0-alpha2 (2022-09-23)

**New Features**

- Added support for in-game debug mode.
- Introduced new command-line options like `--root-path=<dir>`.

### v0.2.0-alpha1 (2022-09-17)

**New Features**

- Added support for more resolutions, skipping the opening animation, mouse clipping, off-site startup, game path customization, and Virtools console output.
- Introduced many new settings and corresponding command-line options.

**Bug Fixes**

- Fixed issues with fullscreen setup.
- Resolved an issue where the player would get stuck when using custom resolutions.

**Changes**

- Unified log and error output.
- Rearranged settings and corresponding command-line options.

### v0.1.9 (2022-08-30)

**Bug Fixes**

- Fixed an issue with the position-saving mechanism when exiting fullscreen mode.

**Changes**

- If the window is resizable (with a title), it now appears in the upper-left corner with a slim margin from the screen edge when the window position is set to 0.
- The saved window position value can now be negative.
- The game will now save the last used game mode (fullscreen or windowed) and restore it the next time it starts up.

### v0.1.8 (2022-08-17)

**Bug Fixes**

- Fixed an issue where the game was locked at 60 FPS on 144Hz monitors when v-sync was enabled.
- Resolved a bug preventing the game from finding multiple display drivers.

### v0.1.7 (2022-08-15)

**New Features**

- Added `X` and `Y` settings with corresponding `-x` and `-y` options to set the screen coordinates of the upper-left corner of the window.

**Bug Fixes**

- Fixed an issue where the game window resized unnaturally before starting.

**Changes**

- The last window position is now saved and restored on the next startup.

### v0.1.6 (2022-08-05)

**New Features**

- Added the `Borderless` setting with the `-c` option to start in borderless mode.
- Added the `Resizable` setting with the `-s` option to make the window resizable.

**Changes**

- The window is now non-resizable by default.

### v0.1.5 (2022-07-20)

**New Features**

- Added support for 32-bit color mode.
- Added support for command-line long options.
- Added the `Language` setting with the `-l` option to set the game language.
- Added the `UnlockFramerate` setting with the `-u` option to unlock the frame-rate limit.

**Bug Fixes**

- Fixed a UI disorder bug during task switching.

**Changes**

- Changed the command-line option for disabling task switching from `-d` to `-e`.
- Reimplemented the initialization configuration mechanism.
- Rewrote command-line support.
- Integrated the functions of `ResDll`.

### v0.1.4 (2022-05-12)

**New Features**

- Added the `PauseOnTaskSwitch` setting with the `-p` option to enable game pause during task switching.

**Bug Fixes**

- Fixed bugs where the game would exit early and crash upon exit.

**Changes**

- Removed unnecessary virtual functions to improve performance.
- Removed the unused Interface Sprite.
- Improved error handling.

### v0.1.3 (2022-05-09)

**New Features**

- Added support for command-line options.

**Bug Fixes**

- Fixed fullscreen crash.
- Fixed task switching issues.
- Fixed resolution duplicates in Graphics Options.

**Changes**

- Removed dependency on `Dsetup.dll` since checking DirectX is unnecessary on modern PCs.
- Removed exception handling to improve performance.

### v0.1.2 (2022-05-08)

**Bug Fixes**

- Fixed an issue where the game might crash upon exit.

**Changes**

- Default config files are now generated when no configuration file is found.
- Replaced remaining registry operations in the main function.
- Removed `FixedString`.

### v0.1.1 (2022-05-04)

**New Features**

- Added support for initialization file configuration.

**Bug Fixes**

- Fixed an issue where the game displayed incompletely in windowed mode.

**Changes**

- Removed some unknown class members.

### v0.1.0 (2022-05-03)

**Bug Fixes**

- Fixed a memory checking bug that caused incorrect results.
