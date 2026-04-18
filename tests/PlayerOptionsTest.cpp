#include <gtest/gtest.h>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "PlayerOptions.h"

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

TEST(PlayerOptionsTest, ExposesMetadataForConfigAndPathOptions) {
    EXPECT_GT(playeroptions::GetConfigOptionCount(), 20);
    EXPECT_GT(playeroptions::GetPathOptionCount(), 5);
    EXPECT_TRUE(playeroptions::HasConfigOption("--disable-sort-transparent-objects", 'o'));
    EXPECT_TRUE(playeroptions::HasConfigOption("--child-window-rendering", 'C'));
    EXPECT_FALSE(playeroptions::HasConfigOption("--child-window-rendering", 's'));
    EXPECT_TRUE(playeroptions::HasPathOption("--log"));
}
