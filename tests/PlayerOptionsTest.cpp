#include <gtest/gtest.h>
#include <filesystem>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "PlayerOptions.h"
#include "Utils.h"

namespace fs = std::filesystem;

namespace {
struct PathOptionCase {
    const char *option;
    PathCategory category;
    const char *value;
};

const PathOptionCase kPathOptionCases[] = {
    {"--config", eConfigPath, "custom.ini"},
    {"--log", eLogPath, "custom.log"},
    {"--cmo", eCmoPath, "custom.cmo"},
    {"--root-path", eRootPath, "root"},
    {"--plugin-path", ePluginPath, "plugins"},
    {"--render-engine-path", eRenderEnginePath, "renderengines"},
    {"--manager-path", eManagerPath, "managers"},
    {"--building-block-path", eBuildingBlockPath, "buildingblocks"},
    {"--sound-path", eSoundPath, "sounds"},
    {"--bitmap-path", eBitmapPath, "textures"},
    {"--data-path", eDataPath, "data"},
};
}

TEST(PlayerOptionsTest, AppliesConfigOptionsFromLongAndShortOptions) {
    CmdlineParser parser("--verbose --width=800 --height 600 --clip-cursor -C");
    CGameConfig config;

    playeroptions::ApplyConfigOptions(config, parser);

    EXPECT_TRUE(config.verbose);
    EXPECT_EQ(config.width, 800);
    EXPECT_EQ(config.height, 600);
    EXPECT_TRUE(config.clipCursor);
    EXPECT_TRUE(config.childWindowRendering);
}

TEST(PlayerOptionsTest, CommandLineConfigOptionsApplyOnlyToRuntimeCopy) {
    CmdlineParser parser("--width=800 --height 600 --fullscreen");
    CGameConfig persistentConfig;
    persistentConfig.width = 640;
    persistentConfig.height = 480;
    persistentConfig.fullscreen = false;

    CGameConfig runtimeConfig = persistentConfig;

    playeroptions::ApplyConfigOptions(runtimeConfig, parser);

    EXPECT_EQ(runtimeConfig.width, 800);
    EXPECT_EQ(runtimeConfig.height, 600);
    EXPECT_TRUE(runtimeConfig.fullscreen);
    EXPECT_EQ(persistentConfig.width, 640);
    EXPECT_EQ(persistentConfig.height, 480);
    EXPECT_FALSE(persistentConfig.fullscreen);
}

TEST(PlayerOptionsTest, RuntimeOptionsCanOverrideCompositionPath) {
    CmdlineParser parser("--cmo custom.cmo --width=800");
    CGameConfig persistentConfig;
    persistentConfig.SetPath(eCmoPath, "base.cmo");
    persistentConfig.width = 640;

    CGameConfig runtimeConfig = persistentConfig;

    playeroptions::ApplyRuntimeOptions(runtimeConfig, parser);

    EXPECT_STREQ(runtimeConfig.GetPath(eCmoPath), "custom.cmo");
    EXPECT_EQ(runtimeConfig.width, 800);
    EXPECT_STREQ(persistentConfig.GetPath(eCmoPath), "base.cmo");
    EXPECT_EQ(persistentConfig.width, 640);
}

TEST(PlayerOptionsTest, AppliesPathOptionsWithoutResettingFilePaths) {
    CmdlineParser parser("--log custom.log --config custom.ini --root-path ..\\");
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    EXPECT_STREQ(config.GetPath(eLogPath), "custom.log");
    EXPECT_STREQ(config.GetPath(eConfigPath), "custom.ini");
}

TEST(PlayerOptionsTest, ConfigPathDoesNotStopFurtherPathScanning) {
    CmdlineParser parser("--config custom.ini --log custom.log");
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    EXPECT_STREQ(config.GetPath(eConfigPath), "custom.ini");
    EXPECT_STREQ(config.GetPath(eLogPath), "custom.log");
}

TEST(PlayerOptionsTest, AppliesEveryDocumentedPathOption) {
    fs::path testRoot = fs::temp_directory_path() / "playeroptions_all_paths";
    fs::remove_all(testRoot);
    fs::create_directories(testRoot);

    std::string cmdline;
    for (const auto &testCase : kPathOptionCases) {
        fs::path valuePath = testRoot / testCase.value;
        if (testCase.category >= eRootPath)
            fs::create_directories(valuePath);

        cmdline += testCase.option;
        cmdline += " \"";
        cmdline += valuePath.string();
        cmdline += "\" ";
    }

    CmdlineParser parser(cmdline.c_str());
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    for (const auto &testCase : kPathOptionCases) {
        fs::path valuePath = testRoot / testCase.value;
        EXPECT_STREQ(config.GetPath(testCase.category), valuePath.string().c_str()) << testCase.option;
        EXPECT_TRUE(playeroptions::HasPathOption(testCase.option)) << testCase.option;
    }

    fs::remove_all(testRoot);
}

TEST(PlayerOptionsTest, ExplicitDirectoryPathOptionsDoNotRequireExistingDirectories) {
    fs::path testRoot = fs::temp_directory_path() / "playeroptions_missing_paths";
    fs::remove_all(testRoot);

    const PathOptionCase directoryPathOptions[] = {
        {"--root-path", eRootPath, "missing-root"},
        {"--plugin-path", ePluginPath, "missing-plugins"},
        {"--render-engine-path", eRenderEnginePath, "missing-renderengines"},
        {"--manager-path", eManagerPath, "missing-managers"},
        {"--building-block-path", eBuildingBlockPath, "missing-buildingblocks"},
        {"--sound-path", eSoundPath, "missing-sounds"},
        {"--bitmap-path", eBitmapPath, "missing-textures"},
        {"--data-path", eDataPath, "missing-data"},
    };

    std::string cmdline;
    for (const auto &testCase : directoryPathOptions) {
        fs::path valuePath = testRoot / testCase.value;
        cmdline += testCase.option;
        cmdline += " \"";
        cmdline += valuePath.string();
        cmdline += "\" ";
    }

    CmdlineParser parser(cmdline.c_str());
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    for (const auto &testCase : directoryPathOptions) {
        fs::path valuePath = testRoot / testCase.value;
        EXPECT_STREQ(config.GetPath(testCase.category), valuePath.string().c_str()) << testCase.option;
    }
}

TEST(PlayerOptionsTest, CompositionPathDoesNotRecomputeRootDerivedPaths) {
    CmdlineParser parser("--cmo custom.cmo");
    CGameConfig config;
    const std::string rootPath = config.GetPath(eRootPath);
    const std::string dataPath = config.GetPath(eDataPath);
    const std::string soundPath = config.GetPath(eSoundPath);
    const std::string bitmapPath = config.GetPath(eBitmapPath);

    playeroptions::ApplyPathOptions(config, parser);

    EXPECT_STREQ(config.GetPath(eCmoPath), "custom.cmo");
    EXPECT_EQ(config.GetPath(eRootPath), rootPath);
    EXPECT_EQ(config.GetPath(eDataPath), dataPath);
    EXPECT_EQ(config.GetPath(eSoundPath), soundPath);
    EXPECT_EQ(config.GetPath(eBitmapPath), bitmapPath);
}

TEST(PlayerOptionsTest, RootPathRecomputesImplicitDependentPaths) {
    fs::path testRoot = fs::temp_directory_path() / "playeroptions_root";
    fs::create_directories(testRoot / "Plugins");
    fs::create_directories(testRoot / "RenderEngines");
    fs::create_directories(testRoot / "Managers");
    fs::create_directories(testRoot / "BuildingBlocks");
    fs::create_directories(testRoot / "Sounds");
    fs::create_directories(testRoot / "Textures");

    std::string cmdline = "--root-path \"" + testRoot.string() + "\"";
    CmdlineParser parser(cmdline.c_str());
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    char expected[MAX_PATH];
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "Plugins\\");
    EXPECT_STREQ(config.GetPath(ePluginPath), expected);
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "RenderEngines\\");
    EXPECT_STREQ(config.GetPath(eRenderEnginePath), expected);
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "Managers\\");
    EXPECT_STREQ(config.GetPath(eManagerPath), expected);
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "BuildingBlocks\\");
    EXPECT_STREQ(config.GetPath(eBuildingBlockPath), expected);
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "Sounds\\");
    EXPECT_STREQ(config.GetPath(eSoundPath), expected);
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "Textures\\");
    EXPECT_STREQ(config.GetPath(eBitmapPath), expected);
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "");
    EXPECT_STREQ(config.GetPath(eDataPath), expected);

    fs::remove_all(testRoot);
}

TEST(PlayerOptionsTest, LongRootPathRecomputesImplicitDependentPathsWithoutTruncation) {
    std::string longName(220, 'r');
    fs::path testRoot = fs::temp_directory_path() / longName;
    std::string rootString = testRoot.string();
    ASSERT_LT(rootString.size(), static_cast<size_t>(MAX_PATH));

    std::string cmdline = "--root-path \"" + rootString + "\"";
    CmdlineParser parser(cmdline.c_str());
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    std::string expectedPluginPath = rootString + "\\Plugins\\";
    ASSERT_GT(expectedPluginPath.size(), static_cast<size_t>(MAX_PATH));
    EXPECT_STREQ(config.GetPath(ePluginPath), expectedPluginPath.c_str());
}

TEST(PlayerOptionsTest, ExplicitDependentPathOverridesRootPathDerivedPath) {
    fs::path testRoot = fs::temp_directory_path() / "playeroptions_root_explicit";
    fs::path customPlugins = fs::temp_directory_path() / "playeroptions_custom_plugins";
    fs::create_directories(testRoot / "RenderEngines");
    fs::create_directories(testRoot / "Managers");
    fs::create_directories(testRoot / "BuildingBlocks");
    fs::create_directories(testRoot / "Sounds");
    fs::create_directories(testRoot / "Textures");
    fs::create_directories(customPlugins);

    std::string cmdline = "--root-path \"" + testRoot.string() + "\" --plugin-path \"" + customPlugins.string() + "\"";
    CmdlineParser parser(cmdline.c_str());
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    EXPECT_STREQ(config.GetPath(ePluginPath), customPlugins.string().c_str());

    char expected[MAX_PATH];
    utils::ConcatPath(expected, MAX_PATH, testRoot.string().c_str(), "RenderEngines\\");
    EXPECT_STREQ(config.GetPath(eRenderEnginePath), expected);

    fs::remove_all(testRoot);
    fs::remove_all(customPlugins);
}

TEST(PlayerOptionsTest, ExposesMetadataForConfigAndPathOptions) {
    EXPECT_GT(playeroptions::GetConfigOptionCount(), 20);
    EXPECT_GT(playeroptions::GetPathOptionCount(), 5);
    EXPECT_TRUE(playeroptions::HasConfigOption("--clip-cursor", '\0'));
    EXPECT_TRUE(playeroptions::HasConfigOption("--child-window-rendering", 'C'));
    EXPECT_FALSE(playeroptions::HasConfigOption("--child-window-rendering", 's'));
    EXPECT_TRUE(playeroptions::HasPathOption("--log"));
}
