# BallancePlayer

## 概述

BallancePlayer 是为游戏《Ballance》设计的现代化增强版主程序，通过反编译和改进原始版本开发而成。它在保持与原版游戏兼容的同时，提供了多项新功能和优化，带来更好的游戏体验。

## 特点

- 开箱即用，无需配置即可启动游戏
- 支持 `.ini` 文件配置
- 支持灵活的命令行选项
- 支持 32 位色彩模式
- 兼容不支持 640x480 分辨率的显示器
- 支持高刷新率显示器
- 支持游戏内分辨率切换
- 提供切换全屏模式和关闭游戏的快捷键
- 无需修改注册表
- 不依赖于 `Dsetup.dll`
- 集成 `ResDll.dll`，便捷运行
- 修复了原版播放器中的一些问题
- 提升了性能

## 系统要求

支持以下 Windows 版本：
- Windows XP
- Windows Vista
- Windows 7
- Windows 8 / 8.1
- Windows 10
- Windows 11

## 安装说明

1. 将提供的压缩包解压至游戏文件夹中的 `Bin` 目录。
2. 运行 `Player.exe` 启动游戏，无需设置兼容模式。

## 从源码构建

### 依赖项

构建 BallancePlayer 需要 Virtools SDK，可以从 [Virtools-SDK-2.1](https://github.com/doyaGu/Virtools-SDK-2.1) 获取。在开始构建之前，请将环境变量 `VIRTOOLS_SDK_PATH` 设置为 SDK 的安装路径。

### 使用 CMake 构建

1. **安装 CMake**：确保你的系统上已安装 CMake。
2. **进入项目目录**：打开控制台，进入包含 BallancePlayer 源代码的目录。
3. **生成构建文件**：运行以下命令生成 Visual Studio 项目文件：
   ```
   cmake -B build -G "Visual Studio 16 2022" -A Win32
   ```
4. **在 Visual Studio 中打开**：进入 `build` 目录，打开 `BallancePlayer.sln` 解决方案文件。
5. **构建解决方案**：使用 Visual Studio 的构建工具编译项目。

### 使用 Visual Studio 6.0 构建

1. **安装 Visual Studio 6.0**：确保已安装 Visual Studio 6.0。
2. **打开项目**：在项目目录中找到 `Player.dsw` 并使用 Visual Studio 6.0 打开。
3. **构建项目**：使用 Visual Studio 6.0 的构建工具编译项目。

### 注意事项

官方发行版本为最大限度地提高兼容性，使用 Visual Studio 6.0 进行构建。

## 快捷键

- **[Alt] + [Enter]**：切换窗口模式和全屏模式。
- **[Alt] + [F4]**：强制关闭游戏。
- **[Alt] + [/]**：显示关于窗口。

## INI 设置

`Player.ini` 文件包含多个用于控制游戏行为的设置。

### 启动

- `LogMode`：控制日志处理方式。
  - `0`：追加到日志文件。
  - `1`：覆盖日志文件。
- `Verbose`：控制是否启用详细日志。
  - `0`：禁用。
  - `1`：启用。
- `ManualSetup`：控制是否在启动时显示设置对话框。
  - `0`：禁用。
  - `1`：启用。
- `LoadAllManagers`：控制是否加载所有管理器。
  - `0`：禁用。
  - `1`：启用。
- `LoadAllBuildingBlocks`：控制是否加载所有构建模块。
  - `0`：禁用。
  - `1`：启用。
- `LoadAllPlugins`：控制是否加载所有插件。
  - `0`：禁用。
  - `1`：启用。

### 图形设置

- `Driver`：指定要使用的显卡驱动 ID。
- `BitsPerPixel`：指定色彩深度。
  - `32`：32 位色。
  - `16`：16 位色。
- `Width`：指定屏幕宽度（根据显示器分辨率）。
- `Height`：指定屏幕高度（根据显示器分辨率）。
- `FullScreen`：控制显示模式。
  - `0`：窗口模式。
  - `1`：全屏模式。
- `DisablePerspectiveCorrection`：控制是否禁用透视修正。
  - `0`：关闭。
  - `1`：开启。
- `ForceLinearFog`：强制使用线性雾模式。
  - `0`：关闭。
  - `1`：开启。
- `ForceSoftware`：禁用硬件渲染。
  - `0`：关闭。
  - `1`：开启。
- `DisableFilter`：禁用纹理过滤。
  - `0`：关闭。
  - `1`：开启。
- `EnsureVertexShader`：确保启用顶点着色器支持。
  - `0`：关闭。
  - `1`：开启。
- `UseIndexBuffers`：启用索引缓冲区。
  - `0`：关闭。
  - `1`：开启。
- `DisableDithering`：禁用抖动。
  - `0`：关闭。
  - `1`：开启。
- `Antialias`：设置抗锯齿级别（最小值为 2）。
- `DisableMipmap`：禁用 Mipmap。
  - `0`：关闭。
  - `1`：开启。
- `DisableSpecular`：禁用高光。
  - `0`：关闭。
  - `1`：开启。
- `EnableScreenDump`：启用屏幕转储功能。
  - `0`：关闭。
  - `1`：开启。
- `EnableDebugMode`：启用逐步渲染的调试模式。
  - `0`：关闭。
  - `1`：开启。
- `VertexCache`：设置顶点缓存大小。`0` 值表示禁用排序。
- `TextureCacheManagement`：控制是否启用纹理缓存管理。
  - `0`：关闭。
  - `1`：开启。
- `SortTransparentObjects`：控制是否对透明对象进行排序。
  - `0`：关闭。
  - `1`：开启。
- `TextureVideoFormat`：指定纹理的默认像素格式（例如 `_32_ARGB8888`）。
- `SpriteVideoFormat`：指定精灵的默认像素格式（例如 `_32_ARGB8888`）。

### 窗口设置

- `ChildWindowRendering`：控制是否在子窗口中渲染。
  - `0`：禁用。
  - `1`：启用。
- `Borderless`：控制窗口是否无边框。
  - `0`：禁用。
  - `1`：启用。
- `ClipCursor`：控制是否限制鼠标光标在窗口内。
  - `0`：禁用。
  - `1`：启用。
- `AlwaysHandleInput`：允许窗口在后台时仍然处理输入。
  - `0`：禁用。
  - `1`：启用。
- `PauseOnDeactivated`：当窗口失去焦点时暂停游戏。
  - `0`：禁用。
  - `1`：启用。
- `X`：设置窗口的水平坐标（可以为负值）。
- `Y`：设置窗口的垂直坐标（可以为负值）。

### 游戏设置

- `Language`：设置游戏语言。
  - `0`：德语。
  - `1`：英语。
  - `2`：西班牙语。
  - `3`：意大利语。
  - `4`：法语。
- `SkipOpening`：控制是否跳过开场动画。
  - `0`：禁用。
  - `1`：启用。
- `ApplyHotfix`：控制是否应用热修复。
  - `0`：禁用。
  - `1`：启用。
- `UnlockFramerate`：解锁帧率限制。
  - `0`：禁用。
  - `1`：启用。
- `UnlockWidescreen`：解锁非 4:3 分辨率。
  - `0`：禁用。
  - `1`：启用。
- `UnlockHighResolution`：解锁高于 1600x1200 的分辨率。
  - `0`：禁用。
  - `1`：启用。
- `Debug`：启用游戏内调试模式。
  - `0`：禁用。
  - `1`：启用。
- `Rookie`：启用游戏内新手模式。
  - `0`：禁用。
  - `1`：启用。

## 命令行选项

你还可以使用命令行选项来自定义游戏行为：

```bash
Player.exe [OPTIONS]
```
- `--verbose`：启用详细日志记录。
- `-m`, `--manual-setup`：启动时总是显示设置对话框。
- `--load-all-managers`：启动时加载所有管理器。
- `--load-all-building-blocks`：启动时加载所有构建模块。
- `--load-all-plugins`：启动时加载所有插件。
- `-v <driver>`, `--video-driver <driver>`：设置显卡驱动 ID。
- `-b <bpp>`, `--bpp <bpp>`：设置屏幕的色彩深度（32 或 16）。
- `-w <width>`, `--width <width>`：设置屏幕宽度。
- `-h <height>`, `--height <height>`：设置屏幕高度。
- `-f`, `--fullscreen`：启动时进入全屏模式。
- `--disable-perspective-correction`：禁用透视修正。
- `--force-linear-fog`：强制使用线性雾。
- `--force-software`：禁用硬件渲染并强制使用软件模式。
- `--disable-filter`：禁用纹理过滤。
- `--ensure-vertex-shader`：确保顶点着色器支持。
- `--use-index-buffers`：启用索引缓冲区。
- `--disable-dithering`：禁用抖动。
- `--antialias <level>`：启用抗锯齿（最小值：2）。
- `--disable-mipmap`：禁用 Mipmap。
- `--disable-specular`：禁用高光。
- `--enable-screen-dump`：使用 (CTRL + ALT + F10) 保存屏幕内容。
- `--enable-debug-mode`：使用 (CTRL + ALT + F11) 启用调试模式。
- `--vertex-cache <size>`：设置顶点缓存大小（0 禁用排序）。
- `--disable-texture-cache-management`：禁用纹理缓存管理。
- `--disable-sort-transparent-objects`：禁用透明对象排序。
- `--texture-video-format <pixel-format>`：设置纹理的像素格式（如 `_32_ARGB8888`）。
- `--sprite-video-format <pixel-format>`：设置精灵的像素格式（如 `_32_ARGB8888`）。
- `-s`, `--child-window-rendering`：启用子窗口渲染。
- `-c`, `--borderless`：启动时进入无边框模式。
- `--clip-cursor`：限制鼠标光标在窗口内。
- `--always-handle-input`：允许窗口在后台时处理输入。
- `-p`, `--pause-on-deactivated`：窗口失去焦点时暂停游戏。
- `-x <X>`, `--position-x <X>`：设置窗口的 X 坐标。
- `-y <Y>`, `--position-y <Y>`：设置窗口的 Y 坐标。
- `-l <lang>`, `--lang <lang>`：设置游戏语言。
- `--skip-opening`：跳过开场动画。
- `--disable-hotfix`：禁用脚本热修复。
- `-u`, `--unlock-framerate`：解除帧率限制。
- `--unlock-widescreen`：解锁非 4:3 分辨率。
- `--unlock-high-resolution`：解锁高于 1600x1200 的分辨率。
- `d`, `--debug`：启用游戏内调试模式。
- `r`, `--rookie`：启用游戏内新手模式。

## 联系方式

如果你有任何问题或功能请求，请在 GitHub 上进行反馈：[BallancePlayer](https://github.com/doyaGu/BallancePlayer)。

## 更新日志

请参考英文更新记录。