#include <gtest/gtest.h>
#include <filesystem>

#include "GameConfig.h"
#include "PlayerStartup.h"

namespace fs = std::filesystem;

class PlayerStartupTest : public ::testing::Test
{
protected:
    fs::path testDir;

    void SetUp() override
    {
        testDir = fs::temp_directory_path() / "playerstartup_test";
        fs::remove_all(testDir);
        fs::create_directories(testDir);
    }

    void TearDown() override
    {
        fs::remove_all(testDir);
    }

    CGameConfig ConfigFor(const fs::path &path)
    {
        CGameConfig config;
        config.SetPath(eConfigPath, path.string().c_str());
        return config;
    }
};

TEST_F(PlayerStartupTest, ShowsInitialConfigDialogWhenConfigFileIsMissing)
{
    CGameConfig config = ConfigFor(testDir / "Player.ini");

    EXPECT_EQ(GetStartupConfigAction(config), eShowInitialConfigDialog);
}

TEST_F(PlayerStartupTest, UsesExistingConfigWhenConfigFileExists)
{
    fs::path configPath = testDir / "Player.ini";
    {
        FILE *file = fopen(configPath.string().c_str(), "w");
        ASSERT_NE(file, nullptr);
        fclose(file);
    }
    CGameConfig config = ConfigFor(configPath);

    EXPECT_EQ(GetStartupConfigAction(config), eUseExistingConfig);
}

TEST_F(PlayerStartupTest, ShowsInitialConfigDialogForMissingCustomConfigPath)
{
    CGameConfig config = ConfigFor(testDir / "custom.ini");

    EXPECT_EQ(GetStartupConfigAction(config), eShowInitialConfigDialog);
}
