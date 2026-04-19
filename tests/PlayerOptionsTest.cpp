#include <gtest/gtest.h>
#include <filesystem>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "PlayerOptions.h"
#include "Utils.h"

namespace fs = std::filesystem;

TEST(PlayerOptionsTest, AppliesConfigOptionsFromLongAndShortOptions) {
    CmdlineParser parser("--verbose --width=800 --height 600 --disable-sort-transparent-objects -C");
    CGameConfig config;

    playeroptions::ApplyConfigOptions(config, parser);

    EXPECT_TRUE(config.verbose);
    EXPECT_EQ(config.width, 800);
    EXPECT_EQ(config.height, 600);
    EXPECT_FALSE(config.sortTransparentObjects);
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

TEST(PlayerOptionsTest, DetectsConfigToolOptionWithoutBreakingConfigPath) {
    CmdlineParser parser("--config-tool --config custom.ini");
    CGameConfig config;

    EXPECT_TRUE(playeroptions::IsConfigToolMode(parser));

    playeroptions::ApplyPathOptions(config, parser);

    EXPECT_STREQ(config.GetPath(eConfigPath), "custom.ini");
}

TEST(PlayerOptionsTest, ConfigToolModeAllowsDialogToChooseDefaultConfigPath) {
    CmdlineParser parser("--config-tool");
    CGameConfig config;

    playeroptions::ApplyConfigToolPathOptions(config, parser);

    EXPECT_FALSE(config.HasPath(eConfigPath));
    EXPECT_STREQ(config.GetPath(eConfigPath), "");
}

TEST(PlayerOptionsTest, ConfigToolModePreservesExplicitConfigPath) {
    CmdlineParser parser("--config-tool --config custom.ini");
    CGameConfig config;

    playeroptions::ApplyConfigToolPathOptions(config, parser);

    EXPECT_STREQ(config.GetPath(eConfigPath), "custom.ini");
}

TEST(PlayerOptionsTest, ExposesMetadataForConfigAndPathOptions) {
    EXPECT_GT(playeroptions::GetConfigOptionCount(), 20);
    EXPECT_GT(playeroptions::GetPathOptionCount(), 5);
    EXPECT_TRUE(playeroptions::HasConfigOption("--disable-sort-transparent-objects", 'o'));
    EXPECT_TRUE(playeroptions::HasConfigOption("--child-window-rendering", 'C'));
    EXPECT_FALSE(playeroptions::HasConfigOption("--child-window-rendering", 's'));
    EXPECT_TRUE(playeroptions::HasPathOption("--log"));
}
