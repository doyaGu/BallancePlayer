#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

#include "GameConfig.h"
#include "Utils.h"

#include "VxMathDefines.h"

namespace fs = std::filesystem;

class GameConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "gameconfig_test";
        fs::create_directories(testDir);
        testIniPath = testDir / "test_config.ini";
    }

    void TearDown() override {
        fs::remove_all(testDir);
    }

    void CreateTestIni(const std::string& content) {
        std::ofstream file(testIniPath);
        file << content;
        file.close();
    }

    std::string ReadFile(const fs::path& path) {
        std::ifstream file(path);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    }

    fs::path testDir;
    fs::path testIniPath;
};

// Test default constructor initialization
TEST_F(GameConfigTest, DefaultConstructor) {
    CGameConfig config;
    
    // Test auto-generated defaults from GAMECONFIG_FIELDS
    EXPECT_EQ(config.logMode, 1);
    EXPECT_FALSE(config.verbose);
    EXPECT_FALSE(config.manualSetup);
    EXPECT_EQ(config.driver, 0);
    EXPECT_EQ(config.bpp, PLAYER_DEFAULT_BPP);
    EXPECT_EQ(config.width, PLAYER_DEFAULT_WIDTH);
    EXPECT_EQ(config.height, PLAYER_DEFAULT_HEIGHT);
    EXPECT_EQ(config.antialias, 0);
    EXPECT_EQ(config.vertexCache, 16);
    EXPECT_EQ(config.posX, 2147483647);
    EXPECT_EQ(config.posY, 2147483647);
    EXPECT_EQ(config.langId, 1);
    EXPECT_FALSE(config.fullscreen);
    EXPECT_FALSE(config.disablePerspectiveCorrection);
    EXPECT_FALSE(config.forceLinearFog);
    EXPECT_FALSE(config.forceSoftware);
    EXPECT_FALSE(config.disableFilter);
    EXPECT_FALSE(config.ensureVertexShader);
    EXPECT_FALSE(config.useIndexBuffers);
    EXPECT_FALSE(config.disableDithering);
    EXPECT_FALSE(config.disableMipmap);
    EXPECT_FALSE(config.disableSpecular);
    EXPECT_FALSE(config.enableScreenDump);
    EXPECT_FALSE(config.enableDebugMode);
    EXPECT_TRUE(config.textureCacheManagement);
    EXPECT_TRUE(config.sortTransparentObjects);
    EXPECT_FALSE(config.childWindowRendering);
    EXPECT_FALSE(config.borderless);
    EXPECT_FALSE(config.clipCursor);
    EXPECT_FALSE(config.alwaysHandleInput);
    EXPECT_FALSE(config.skipOpening);
    EXPECT_TRUE(config.applyHotfix);
    EXPECT_FALSE(config.unlockFramerate);
    EXPECT_FALSE(config.unlockWidescreen);
    EXPECT_FALSE(config.unlockHighResolution);
    EXPECT_EQ(config.textureVideoFormat, UNKNOWN_PF);
    EXPECT_EQ(config.spriteVideoFormat, UNKNOWN_PF);
    
    // Test non-INI members
    EXPECT_EQ(config.screenMode, -1);
    EXPECT_FALSE(config.debug);
    EXPECT_FALSE(config.rookie);
}

// Test assignment operator
TEST_F(GameConfigTest, AssignmentOperator) {
    CGameConfig config1;
    CGameConfig config2;
    
    // Modify config1
    config1.verbose = true;
    config1.driver = 5;
    config1.width = 1920;
    config1.height = 1080;
    config1.fullscreen = true;
    config1.antialias = 4;
    config1.screenMode = 2;
    config1.debug = true;
    config1.SetPath(eConfigPath, "custom_config.ini");
    
    // Test assignment
    config2 = config1;
    
    EXPECT_TRUE(config2.verbose);
    EXPECT_EQ(config2.driver, 5);
    EXPECT_EQ(config2.width, 1920);
    EXPECT_EQ(config2.height, 1080);
    EXPECT_TRUE(config2.fullscreen);
    EXPECT_EQ(config2.antialias, 4);
    EXPECT_EQ(config2.screenMode, 2);
    EXPECT_TRUE(config2.debug);
    EXPECT_STREQ(config2.GetPath(eConfigPath), "custom_config.ini");
    
    // Test self-assignment
    config1 = config1;
    EXPECT_TRUE(config1.verbose);
    EXPECT_EQ(config1.driver, 5);
}

// Test path management
TEST_F(GameConfigTest, PathManagement) {
    CGameConfig config;
    
    // Test initial state
    EXPECT_TRUE(config.HasPath(eConfigPath));
    EXPECT_STREQ(config.GetPath(eConfigPath), "Player.ini");
    
    // Test setting custom path
    config.SetPath(eConfigPath, "custom.ini");
    EXPECT_STREQ(config.GetPath(eConfigPath), "custom.ini");
    
    // Test invalid category
    EXPECT_FALSE(config.HasPath(static_cast<PathCategory>(-1)));
    EXPECT_EQ(config.GetPath(static_cast<PathCategory>(-1)), nullptr);
    
    config.SetPath(static_cast<PathCategory>(-1), "invalid");
    EXPECT_EQ(config.GetPath(static_cast<PathCategory>(-1)), nullptr);
    
    // Test reset specific path
    EXPECT_TRUE(config.ResetPath(eConfigPath));
    EXPECT_STREQ(config.GetPath(eConfigPath), "Player.ini");
    
    // Test reset all paths
    config.SetPath(eConfigPath, "custom.ini");
    EXPECT_TRUE(config.ResetPath());
    EXPECT_STREQ(config.GetPath(eConfigPath), "Player.ini");
}

// Test loading from INI file
TEST_F(GameConfigTest, LoadFromIni) {
    std::string iniContent = R"(
[Startup]
LogMode=0
Verbose=1
ManualSetup=1

[Graphics]
Driver=2
BitsPerPixel=32
Width=1920
Height=1080
FullScreen=1
Antialias=4
VertexCache=32
DisablePerspectiveCorrection=1
ForceLinearFog=1
TextureVideoFormat=565
SpriteVideoFormat=1555

[Window]
X=100
Y=200
Borderless=1
ClipCursor=1

[Game]
Language=2
SkipOpening=1
UnlockFramerate=1
)";
    
    CreateTestIni(iniContent);
    
    CGameConfig config;
    config.LoadFromIni(testIniPath.string().c_str());
    
    // Verify loaded values
    EXPECT_EQ(config.logMode, 0);
    EXPECT_TRUE(config.verbose);
    EXPECT_TRUE(config.manualSetup);
    EXPECT_EQ(config.driver, 2);
    EXPECT_EQ(config.bpp, 32);
    EXPECT_EQ(config.width, 1920);
    EXPECT_EQ(config.height, 1080);
    EXPECT_TRUE(config.fullscreen);
    EXPECT_EQ(config.antialias, 4);
    EXPECT_EQ(config.vertexCache, 32);
    EXPECT_TRUE(config.disablePerspectiveCorrection);
    EXPECT_TRUE(config.forceLinearFog);
    EXPECT_EQ(config.posX, 100);
    EXPECT_EQ(config.posY, 200);
    EXPECT_TRUE(config.borderless);
    EXPECT_TRUE(config.clipCursor);
    EXPECT_EQ(config.langId, 2);
    EXPECT_TRUE(config.skipOpening);
    EXPECT_TRUE(config.unlockFramerate);
    
    // Test values not in INI keep defaults
    EXPECT_FALSE(config.forceSoftware);
    EXPECT_EQ(config.driver, 2); // This was set
}

// Test loading with empty/null filename
TEST_F(GameConfigTest, LoadFromIniEdgeCases) {
    CGameConfig config;
    
    // Test null filename
    config.LoadFromIni(nullptr);
    EXPECT_EQ(config.driver, 0); // Should remain default
    
    // Test empty filename without config path set
    config.LoadFromIni("");
    EXPECT_EQ(config.driver, 0); // Should remain default
    
    // Test empty filename with config path set
    CreateTestIni("[Graphics]\nDriver=5\n");
    config.SetPath(eConfigPath, testIniPath.string().c_str());
    config.LoadFromIni("");
    EXPECT_EQ(config.driver, 5); // Should load from config path
    
    // Test non-existent file
    config.LoadFromIni("non_existent_file.ini");
    EXPECT_EQ(config.driver, 5); // Should remain unchanged
}

// Test saving to INI file
TEST_F(GameConfigTest, SaveToIni) {
    CGameConfig config;
    
    // Set some values
    config.logMode = 0;
    config.verbose = true;
    config.driver = 3;
    config.width = 1600;
    config.height = 900;
    config.fullscreen = true;
    config.antialias = 2;
    config.posX = 50;
    config.posY = 75;
    config.langId = 3;
    config.borderless = true;
    config.unlockWidescreen = true;
    config.debug = true;
    config.rookie = true;
    
    config.SaveToIni(testIniPath.string().c_str());
    
    // Verify file was created and contains expected content
    EXPECT_TRUE(fs::exists(testIniPath));
    
    std::string content = ReadFile(testIniPath);
    
    // Check for presence of saved values (exact format depends on utils::IniSet* implementation)
    EXPECT_NE(content.find("LogMode"), std::string::npos);
    EXPECT_NE(content.find("Verbose"), std::string::npos);
    EXPECT_NE(content.find("Driver"), std::string::npos);
    EXPECT_NE(content.find("Width"), std::string::npos);
    
    // Test loading back the saved file
    CGameConfig config2;
    config2.LoadFromIni(testIniPath.string().c_str());
    
    EXPECT_EQ(config2.logMode, 0);
    EXPECT_TRUE(config2.verbose);
    EXPECT_EQ(config2.driver, 3);
    EXPECT_EQ(config2.width, 1600);
    EXPECT_EQ(config2.height, 900);
    EXPECT_TRUE(config2.fullscreen);
    EXPECT_EQ(config2.antialias, 2);
    EXPECT_EQ(config2.posX, 50);
    EXPECT_EQ(config2.posY, 75);
    EXPECT_EQ(config2.langId, 3);
    EXPECT_TRUE(config2.borderless);
    EXPECT_TRUE(config2.unlockWidescreen);
    EXPECT_TRUE(config2.debug);
    EXPECT_TRUE(config2.rookie);
}

// Test external change merging
TEST_F(GameConfigTest, ExternalChangeMerging) {
    // Create initial INI
    CreateTestIni("[Graphics]\nDriver=1\nWidth=800\n[Game]\nLanguage=1\n");
    
    CGameConfig config;
    config.LoadFromIni(testIniPath.string().c_str());
    
    EXPECT_EQ(config.driver, 1);
    EXPECT_EQ(config.width, 800);
    EXPECT_EQ(config.langId, 1);
    
    // Simulate external modification of the file
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    CreateTestIni("[Graphics]\nDriver=2\nWidth=1024\n[Game]\nLanguage=2\n");
    
    // Change some values in memory
    config.driver = 5; // This should not be overridden (we changed it)
    config.height = 768; // This wasn't in external change, should remain
    
    // Save should trigger merge
    config.SaveToIni(testIniPath.string().c_str());
    
    // Values we didn't change should be updated from external file
    EXPECT_EQ(config.width, 1024); // Should be updated from external
    EXPECT_EQ(config.langId, 2);   // Should be updated from external
    
    // Values we changed should remain our values
    EXPECT_EQ(config.driver, 5);   // Should keep our change
    EXPECT_EQ(config.height, 768); // Should keep our change
}

// Test relative vs absolute paths
TEST_F(GameConfigTest, PathHandling) {
    CGameConfig config;
    
    // Test relative path (should be converted to absolute)
    config.LoadFromIni("relative_config.ini");
    
    // Test absolute path
    config.LoadFromIni(testIniPath.string().c_str());
    
    // The actual behavior depends on utils::IsAbsolutePath and utils::GetCurrentPath
    // This test mainly ensures no crashes occur with different path formats
}

// Test snapshot functionality
TEST_F(GameConfigTest, SnapshotManagement) {
    CGameConfig config;
    
    // Load from INI to create snapshots
    CreateTestIni("[Graphics]\nDriver=2\nWidth=1024\n");
    config.LoadFromIni(testIniPath.string().c_str());
    
    EXPECT_EQ(config.driver, 2);
    EXPECT_EQ(config.width, 1024);
    
    // Modify external file
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    CreateTestIni("[Graphics]\nDriver=3\nWidth=1280\n");
    
    // Change one value in memory, leave other unchanged
    config.driver = 10;
    // config.width remains 1024
    
    // Save should merge external changes for unchanged values
    config.SaveToIni(testIniPath.string().c_str());
    
    // The width should be updated from external file since we didn't change it
    // The driver should remain our value since we changed it
    EXPECT_EQ(config.driver, 10);
    EXPECT_EQ(config.width, 1280);
}

// Test all configuration field types
TEST_F(GameConfigTest, AllFieldTypes) {
    std::string iniContent = R"(
[Startup]
LogMode=1
Verbose=1

[Graphics]
Driver=1
BitsPerPixel=16
TextureVideoFormat=565
SpriteVideoFormat=1555
FullScreen=1
)";
    
    CreateTestIni(iniContent);
    
    CGameConfig config;
    config.LoadFromIni(testIniPath.string().c_str());
    
    // Test that all field types are handled correctly
    EXPECT_EQ(config.logMode, 1);        // X_INT
    EXPECT_TRUE(config.verbose);         // X_BOOL
    EXPECT_EQ(config.driver, 1);         // X_INT
    EXPECT_EQ(config.bpp, 16);          // X_INT
    EXPECT_TRUE(config.fullscreen);      // X_BOOL
    // X_PF types would need actual VX_PIXELFORMAT values to test properly
}

// Test empty INI file
TEST_F(GameConfigTest, EmptyIniFile) {
    CreateTestIni("");
    
    CGameConfig config;
    config.LoadFromIni(testIniPath.string().c_str());
    
    // All values should remain at defaults
    EXPECT_EQ(config.logMode, 1);
    EXPECT_FALSE(config.verbose);
    EXPECT_EQ(config.driver, 0);
    EXPECT_EQ(config.width, PLAYER_DEFAULT_WIDTH);
}

// Test malformed INI file
TEST_F(GameConfigTest, MalformedIniFile) {
    CreateTestIni("This is not a valid INI file\n[Graphics\nDriver=invalid\n");
    
    CGameConfig config;
    // Should not crash
    config.LoadFromIni(testIniPath.string().c_str());
    
    // Values should remain at defaults
    EXPECT_EQ(config.driver, 0);
}

// Performance test for large number of operations
TEST_F(GameConfigTest, PerformanceTest) {
    std::string iniContent = R"(
[Startup]
LogMode=1
Verbose=1
ManualSetup=0

[Graphics]
Driver=2
BitsPerPixel=32
Width=1920
Height=1080
FullScreen=1
Antialias=4
)";
    
    CreateTestIni(iniContent);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform many load/save operations
    for (int i = 0; i < 100; ++i) {
        CGameConfig config;
        config.LoadFromIni(testIniPath.string().c_str());
        
        config.driver = i % 5;
        config.width = 800 + (i % 400);
        
        std::string tempPath = testDir.string() + "\\" + ("temp_" + std::to_string(i) + ".ini");
        config.SaveToIni(tempPath.c_str());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete within reasonable time (adjust threshold as needed)
    // INI operations can be slow on Windows, especially with many disk writes
    EXPECT_LT(duration.count(), 15000); // Less than 15 seconds for 100 operations
}

// Test X-macro expansion correctness
TEST_F(GameConfigTest, XMacroExpansion) {
    // This test verifies that the X-macro expansion works correctly
    // by testing that all expected members exist and are accessible
    
    CGameConfig config;
    
    // Test that all members from GAMECONFIG_FIELDS are accessible
    // (This would fail to compile if the X-macro expansion was incorrect)
    
    // Test INT fields
    config.logMode = 1;
    config.driver = 2;
    config.bpp = 32;
    config.width = 1920;
    config.height = 1080;
    config.antialias = 4;
    config.vertexCache = 16;
    config.posX = 100;
    config.posY = 200;
    config.langId = 1;
    
    // Test BOOL fields
    config.verbose = true;
    config.manualSetup = false;
    config.fullscreen = true;
    config.disablePerspectiveCorrection = false;
    config.forceLinearFog = true;
    config.forceSoftware = false;
    config.disableFilter = true;
    config.ensureVertexShader = false;
    config.useIndexBuffers = true;
    config.disableDithering = false;
    config.disableMipmap = true;
    config.disableSpecular = false;
    config.enableScreenDump = true;
    config.enableDebugMode = false;
    config.textureCacheManagement = true;
    config.sortTransparentObjects = false;
    config.childWindowRendering = true;
    config.borderless = false;
    config.clipCursor = true;
    config.alwaysHandleInput = false;
    config.skipOpening = true;
    config.applyHotfix = false;
    config.unlockFramerate = true;
    config.unlockWidescreen = false;
    config.unlockHighResolution = true;
    
    // Test PF fields
    config.textureVideoFormat = UNKNOWN_PF;
    config.spriteVideoFormat = UNKNOWN_PF;
    
    // If we get here without compilation errors, the X-macro expansion worked
    SUCCEED();
}

// Test memory footprint
TEST_F(GameConfigTest, MemoryFootprint) {
    // Test that the memory layout is reasonable
    CGameConfig config;
    
    // The size should be reasonable (not excessive due to X-macro expansion)
    size_t configSize = sizeof(CGameConfig);
    
    // Should be less than 10KB (adjust based on actual requirements)
    EXPECT_LT(configSize, 10240);
    
    // Should be at least the size of all the members
    size_t minimumSize = sizeof(int) * 10 + sizeof(bool) * 25 + sizeof(VX_PIXELFORMAT) * 2;
    EXPECT_GE(configSize, minimumSize);
}
