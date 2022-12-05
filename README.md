# BallancePlayer
# Overview

This repository contains the source files of the brand-new player for Ballance. 

The new player is an enhanced version created on the decompilation of the original version.

## Features

- Out-of-the-box and portable, zero-configuration to start game
- Support `.ini` file configurations
- Support command-line options
- Support 32-bit color mode
- Support the monitor without 640x480 resolution
- Support high refresh rate monitor
- Support OpenGL renderer
- Support in-game resolution switching
- Support in-game resizing in windowed mode
- Support off-site startup
- Support game path customization
- Add hotkeys for switching fullscreen and closing game
- No registry operation
- No dependency on `Dsetup.dll`
- Merge `ResDll.dll` into the player
- Fix some bugs that exist in the original player
- Improve performance

## Environment

Supports Windows XP, Vista, 7, 8, 10, 11.

## Instructions

1. Extract the latest build package into your game folder.
2. Start the game by clicking on `Player.exe`.

## Hotkeys

- **[Alt] + [Enter]**: Switch between windowed and fullscreen mode.
- **[Alt] + [F4]**:  Force close the game.
- **[Alt] + [/]**:  Display an about box.

## INI Settings

There are several settings in `Player.ini`.

- `Language`:
  - German: `0`
  - English: `1`
  - Spanish: `2`
  - Italian: `3`
  - French: `4`

- `LoadAllManagers`:
  - Disable: `0`
  - Enable: `1`

- `LoadAllBuildingBlocks`:
  - Disable: `0`
  - Enable: `1`

- `LoadAllPlugins`:
  - Disable: `0`
  - Enable: `1`

- `AdaptiveCamera`:
  - Disable: `0`
  - Enable: `1`

- `UnlockWidescreen`:
  - Lock: `0`
  - Unlock: `1`

- `UnlockHighResolution`:
  - Lock: `0`
  - Unlock: `1`

- `SkipOpening`:
  - Disable: `0`
  - Enable: `1`

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
  - Fullscreen Mode: `1`

- `UnlockFramerate`:
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

- `Borderless`:
  - Disable: `0`
  - Enable: `1`

- `Resizable`:
  - Disable: `0`
  - Enable: `1`

- `ClipMouse`:
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

## Command-line Options

```bash
Player.exe [-f] [-c] [-s] [-u] [-e] [-p] [-x <X>] [-y <Y>] [-w <width>] [-h <height>] [-b <bbp>] [-v <driver>] [-l <lang>] 
```

- `-l <lang>` or `--lang <lang>`: Set the language id
- `--load-all-managers`: Control whether player loads all managers
- `--load-all-building-blocks`: Control whether player loads all building blocks
- `--load-all-plugins`: Control whether player loads all plugins
- `--adaptive-camera`: Enable camera correction
- `--unlock-widescreen`: Unlock non-4:3 resolutions
- `--unlock-high-resolution`: Unlock resolutions higher than 1600x1200
- `--skip-opening`: Skip the opening animation
- `-v <driver>` or `--video-driver <driver>`: Set the id of the graphics card driver to display
- `-b <bpp>` or `--bpp <bpp>`: Set the bit per pixel of the screen (32 or 16)
- `-w <width>` or `--width <width>`: Set the screen width
- `-h <height>` or `--height <height>`: Set the screen height
- `-f` or `--fullscreen`: Startup in fullscreen mode
- `-u` or `--unlock-framerate`: Unlock the frame rate limitation
- `--antialias <level>`: Enable image antialiasing (minimum: 2)
- `--disable-filter`: Disable texture filtering
- `--disable-dithering`: Disable image dithering
- `--disable-mipmap`: Disable mipmap
- `--disable-specular`: Disable specular highlights
- `-c` or `--borderless`: Startup in borderless mode
- `-s` or `--resizable`: Make window resizable
- `--clip-mouse`: Clip mouse in window
- `--always-handle-input`: Allow player handle input while window is in background
- `-p` or `--pause-on-deactivated`: Pause the game once the window is deactivated
- `-x <X>` or `--position-x <X>`: Set the position x of the window
- `-y <Y>` or `--position-y <Y>`: Set the position y of the window

## Contact

If you have any bugs or requests, please open an issue in this repository: [BallancePlayer](https://github.com/doyaGu/BallancePlayer).

## ChangeLog

### v0.2.1 (2022-11-24)

**News**

- Support console log.
- Add a new setting `LoadAllManagers` and corresponding command-line option `--load-all-managers` to control whether player loads all managers.
- Add a new setting `LoadAllBuildingBlocks` and corresponding command-line option `--load-all-building-blocks` to control whether player loads all building blocks.
- Add a new setting `LoadAllPlugins` and corresponding command-line option `--load-all-plugins` to control whether player loads all plugins.

**Bug Fixes**

- Fix the problem that player change screen mode incorrectly.
- Fix the problem that player can not startup if a wrong path setting exists.

**Changed**

- Remove dependency on the modified version of `TT_InterfaceManager_RT.dll`. Avoid to modify original files.

### v0.2.0 (2022-10-15)

**News**

- Add the Chinese version of README.

### v0.2.0-rc1 (2022-10-09)

**Bug Fixes**

- Fix possible crash problem if BML+ enabled when upon exit.

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
- Support off-site startup. Now you can run the player outside the game folder.
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
