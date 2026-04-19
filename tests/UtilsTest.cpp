#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <chrono>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "Utils.h"
#include "VxMathDefines.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

namespace fs = std::filesystem;

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "utils_test";
        fs::create_directories(testDir);
        testFile = testDir / "test_file.txt";
        testIniPath = testDir / "test.ini";
    }

    void TearDown() override {
        fs::remove_all(testDir);
    }

    void CreateTestFile(const fs::path& path, const std::string& content = "test content") {
        std::ofstream file(path);
        file << content;
        file.close();
    }

    void CreateTestIni(const std::string& content) {
        std::ofstream file(testIniPath);
        file << content;
        file.close();
    }

    fs::path testDir;
    fs::path testFile;
    fs::path testIniPath;
};

// File and Directory Tests
TEST_F(UtilsTest, FileOrDirectoryExists) {
    // Test non-existent file
    EXPECT_FALSE(utils::FileOrDirectoryExists("non_existent_file.txt"));

    // Test null/empty paths
    EXPECT_FALSE(utils::FileOrDirectoryExists(nullptr));
    EXPECT_FALSE(utils::FileOrDirectoryExists(""));

    // Create a test file and test it exists
    CreateTestFile(testFile);
    EXPECT_TRUE(utils::FileOrDirectoryExists(testFile.string().c_str()));

    // Test directory exists
    EXPECT_TRUE(utils::FileOrDirectoryExists(testDir.string().c_str()));
}

TEST_F(UtilsTest, DirectoryExists) {
    // Test non-existent directory
    auto nonExistentDir = testDir / "non_existent";
    EXPECT_FALSE(utils::DirectoryExists(nonExistentDir.string().c_str()));

    // Test null/empty paths
    EXPECT_FALSE(utils::DirectoryExists(nullptr));
    EXPECT_FALSE(utils::DirectoryExists(""));

    // Test existing directory
    EXPECT_TRUE(utils::DirectoryExists(testDir.string().c_str()));

    // Test file (should return false for files)
    CreateTestFile(testFile);
    EXPECT_FALSE(utils::DirectoryExists(testFile.string().c_str()));
}

// Path Tests
TEST_F(UtilsTest, GetCurrentPath) {
    char buffer[MAX_PATH];
    size_t result = utils::GetCurrentPath(buffer, MAX_PATH);

    EXPECT_GT(result, 0);
    EXPECT_GT(strlen(buffer), 0);
}

TEST_F(UtilsTest, IsAbsolutePath) {
    // Test absolute paths
    EXPECT_TRUE(utils::IsAbsolutePath("C:\\test\\path"));
    EXPECT_TRUE(utils::IsAbsolutePath("D:\\another\\path"));
    EXPECT_TRUE(utils::IsAbsolutePath("Z:\\"));

    // Test relative paths
    EXPECT_FALSE(utils::IsAbsolutePath("relative\\path"));
    EXPECT_FALSE(utils::IsAbsolutePath("..\\parent"));
    EXPECT_FALSE(utils::IsAbsolutePath(".\\current"));
    EXPECT_FALSE(utils::IsAbsolutePath("file.txt"));

    // Test invalid/edge cases
    EXPECT_FALSE(utils::IsAbsolutePath(nullptr));
    EXPECT_FALSE(utils::IsAbsolutePath(""));
    EXPECT_FALSE(utils::IsAbsolutePath("C"));
    EXPECT_TRUE(utils::IsAbsolutePath("C:")); // Actually returns true for drive letter with colon
    EXPECT_FALSE(utils::IsAbsolutePath("123:\\test")); // Non-letter drive
}

TEST_F(UtilsTest, GetAbsolutePath) {
    char buffer[MAX_PATH];

    // Test null/empty inputs
    EXPECT_FALSE(utils::GetAbsolutePath(nullptr, MAX_PATH, "test"));
    EXPECT_FALSE(utils::GetAbsolutePath(buffer, MAX_PATH, nullptr));
    EXPECT_FALSE(utils::GetAbsolutePath(buffer, MAX_PATH, ""));

    // Test absolute path (should copy as-is)
    EXPECT_TRUE(utils::GetAbsolutePath(buffer, MAX_PATH, "C:\\test\\path"));
    EXPECT_STREQ(buffer, "C:\\test\\path");

    // Test relative path (should be converted to absolute)
    EXPECT_TRUE(utils::GetAbsolutePath(buffer, MAX_PATH, "relative\\path"));
    EXPECT_NE(strstr(buffer, "relative\\path"), nullptr);

    // Test trailing separator behavior
    EXPECT_TRUE(utils::GetAbsolutePath(buffer, MAX_PATH, "C:\\test", true));
    EXPECT_TRUE(utils::HasTrailingPathSeparator(buffer));

    EXPECT_TRUE(utils::GetAbsolutePath(buffer, MAX_PATH, "C:\\test\\", false));
    EXPECT_FALSE(utils::HasTrailingPathSeparator(buffer));
}

TEST_F(UtilsTest, ConcatPath) {
    char buffer[MAX_PATH];

    // Test normal concatenation
    char* result = utils::ConcatPath(buffer, MAX_PATH, "C:\\base", "subdir");
    EXPECT_EQ(result, buffer);
    EXPECT_STREQ(buffer, "C:\\base\\subdir");

    // Test with trailing separator in path1
    utils::ConcatPath(buffer, MAX_PATH, "C:\\base\\", "subdir");
    EXPECT_STREQ(buffer, "C:\\base\\subdir");

    // Test with null/empty path1
    utils::ConcatPath(buffer, MAX_PATH, nullptr, "path2");
    EXPECT_STREQ(buffer, "path2");

    utils::ConcatPath(buffer, MAX_PATH, "", "path2");
    EXPECT_STREQ(buffer, "path2");

    // Test with null buffer
    EXPECT_EQ(utils::ConcatPath(nullptr, MAX_PATH, "path1", "path2"), nullptr);
}

TEST_F(UtilsTest, FindLastPathSeparator) {
    // Test with backslash
    const char* testPath1 = "C:\\path\\to\\file.txt";
    const char* result = utils::FindLastPathSeparator(testPath1);
    const char* expected1 = strrchr(testPath1, '\\');
    EXPECT_EQ(result, expected1);

    // Test with forward slash
    const char* testPath2 = "C:/path/to/file.txt";
    result = utils::FindLastPathSeparator(testPath2);
    const char* expected2 = strrchr(testPath2, '/');
    EXPECT_EQ(result, expected2);

    // Test with mixed separators (should find the last one)
    result = utils::FindLastPathSeparator("C:\\path/to\\file.txt");
    EXPECT_EQ(*result, '\\');

    // Test with no separators
    result = utils::FindLastPathSeparator("filename.txt");
    EXPECT_EQ(result, nullptr);

    // Test with null/empty
    EXPECT_EQ(utils::FindLastPathSeparator(nullptr), nullptr);
    EXPECT_EQ(utils::FindLastPathSeparator(""), nullptr);
}

TEST_F(UtilsTest, GetFileDirectory) {
    char buffer[MAX_PATH];

    EXPECT_TRUE(utils::GetFileDirectory(buffer, sizeof(buffer), "C:\\Games\\Ballance\\Bin\\Player.exe"));
    EXPECT_STREQ(buffer, "C:\\Games\\Ballance\\Bin\\");

    EXPECT_TRUE(utils::GetFileDirectory(buffer, sizeof(buffer), "C:/Games/Ballance/Bin/Player.exe"));
    EXPECT_STREQ(buffer, "C:/Games/Ballance/Bin/");

    EXPECT_TRUE(utils::GetFileDirectory(buffer, sizeof(buffer), "C:\\Games\\Ballance\\Bin\\Player.exe", false));
    EXPECT_STREQ(buffer, "C:\\Games\\Ballance\\Bin");

    EXPECT_FALSE(utils::GetFileDirectory(buffer, sizeof(buffer), "Player.exe"));
    EXPECT_FALSE(utils::GetFileDirectory(nullptr, sizeof(buffer), "C:\\Player.exe"));
    EXPECT_FALSE(utils::GetFileDirectory(buffer, 3, "C:\\Player.exe"));
}

TEST_F(UtilsTest, SetCurrentDirectoryToFileDirectory) {
    char originalDir[MAX_PATH];
    ASSERT_GT(utils::GetCurrentPath(originalDir, sizeof(originalDir)), 0);

    fs::path exeDir = testDir / "Bin";
    fs::create_directories(exeDir);
    fs::path fakeExe = exeDir / "Player.exe";
    CreateTestFile(fakeExe);

    ASSERT_TRUE(utils::SetCurrentDirectoryToFileDirectory(fakeExe.string().c_str()));

    char currentDir[MAX_PATH];
    ASSERT_GT(utils::GetCurrentPath(currentDir, sizeof(currentDir)), 0);
    EXPECT_EQ(fs::weakly_canonical(currentDir), fs::weakly_canonical(exeDir));

    ASSERT_TRUE(::SetCurrentDirectoryA(originalDir));
}

TEST_F(UtilsTest, HasTrailingPathSeparator) {
    // Test with trailing backslash
    EXPECT_TRUE(utils::HasTrailingPathSeparator("C:\\path\\"));

    // Test with trailing forward slash
    EXPECT_TRUE(utils::HasTrailingPathSeparator("C:/path/"));

    // Test without trailing separator
    EXPECT_FALSE(utils::HasTrailingPathSeparator("C:\\path"));
    EXPECT_FALSE(utils::HasTrailingPathSeparator("filename.txt"));

    // Test edge cases
    EXPECT_FALSE(utils::HasTrailingPathSeparator(nullptr));
    EXPECT_FALSE(utils::HasTrailingPathSeparator(""));
}

TEST_F(UtilsTest, RemoveTrailingPathSeparator) {
    char path1[] = "C:\\path\\";
    EXPECT_TRUE(utils::RemoveTrailingPathSeparator(path1));
    EXPECT_STREQ(path1, "C:\\path");

    char path2[] = "C:/path/";
    EXPECT_TRUE(utils::RemoveTrailingPathSeparator(path2));
    EXPECT_STREQ(path2, "C:/path");

    char path3[] = "C:\\path";
    EXPECT_FALSE(utils::RemoveTrailingPathSeparator(path3));
    EXPECT_STREQ(path3, "C:\\path");
}

// Character conversion tests
TEST_F(UtilsTest, CharToWchar) {
    const char* testStr = "Hello World";
    wchar_t wcharBuffer[256];

    int result = utils::CharToWchar(testStr, wcharBuffer, 256);
    EXPECT_GT(result, 0);

    // Convert back to verify
    char charBuffer[256];
    utils::WcharToChar(wcharBuffer, charBuffer, 256);
    EXPECT_STREQ(testStr, charBuffer);
}

TEST_F(UtilsTest, WcharToChar) {
    const wchar_t* testStr = L"Hello World";
    char charBuffer[256];

    int result = utils::WcharToChar(testStr, charBuffer, 256);
    EXPECT_GT(result, 0);

    // Convert back to verify
    wchar_t wcharBuffer[256];
    utils::CharToWchar(charBuffer, wcharBuffer, 256);
    EXPECT_EQ(wcscmp(testStr, wcharBuffer), 0);
}

// CRC32 tests
TEST_F(UtilsTest, CRC32) {
    const char* testData = "Hello, World!";
    size_t result1, result2;

    // Test basic CRC calculation
    utils::CRC32(testData, strlen(testData), 0, &result1);
    utils::CRC32(testData, strlen(testData), 0, &result2);

    // Same input should produce same output
    EXPECT_EQ(result1, result2);

    // Different input should produce different output
    const char* differentData = "Hello, World?";
    size_t result3;
    utils::CRC32(differentData, strlen(differentData), 0, &result3);
    EXPECT_NE(result1, result3);

    // Different seed should produce different output
    size_t result4;
    utils::CRC32(testData, strlen(testData), 12345, &result4);
    EXPECT_NE(result1, result4);
}

// Pixel format conversion tests
TEST_F(UtilsTest, String2PixelFormat) {
    // Test known formats
    EXPECT_EQ(utils::String2PixelFormat("_32_ARGB8888", 12), _32_ARGB8888);
    EXPECT_EQ(utils::String2PixelFormat("_32_RGB888", 10), _32_RGB888);
    EXPECT_EQ(utils::String2PixelFormat("_24_RGB888", 10), _24_RGB888);
    EXPECT_EQ(utils::String2PixelFormat("_16_RGB565", 10), _16_RGB565);
    EXPECT_EQ(utils::String2PixelFormat("_16_RGB555", 10), _16_RGB555);
    EXPECT_EQ(utils::String2PixelFormat("565", 3), _16_RGB565);
    EXPECT_EQ(utils::String2PixelFormat("1555", 4), _16_ARGB1555);
    EXPECT_EQ(utils::String2PixelFormat("_DXT1", 5), _DXT1);
    EXPECT_EQ(utils::String2PixelFormat("_DXT5", 5), _DXT5);

    // Test unknown format
    EXPECT_EQ(utils::String2PixelFormat("_UNKNOWN_FORMAT", 15), UNKNOWN_PF);
    EXPECT_EQ(utils::String2PixelFormat("", 1), UNKNOWN_PF);
}

TEST_F(UtilsTest, PixelFormat2String) {
    // Test known formats
    EXPECT_STREQ(utils::PixelFormat2String(_32_ARGB8888), "_32_ARGB8888");
    EXPECT_STREQ(utils::PixelFormat2String(_32_RGB888), "_32_RGB888");
    EXPECT_STREQ(utils::PixelFormat2String(_24_RGB888), "_24_RGB888");
    EXPECT_STREQ(utils::PixelFormat2String(_16_RGB565), "_16_RGB565");
    EXPECT_STREQ(utils::PixelFormat2String(_16_RGB555), "_16_RGB555");
    EXPECT_STREQ(utils::PixelFormat2String(_DXT1), "_DXT1");
    EXPECT_STREQ(utils::PixelFormat2String(_DXT5), "_DXT5");

    // Test unknown format
    EXPECT_STREQ(utils::PixelFormat2String(UNKNOWN_PF), "UNKNOWN_PF");
}

TEST_F(UtilsTest, PixelFormatRoundTrip) {
    // Test that conversion is symmetric
    const VX_PIXELFORMAT formats[] = {
        _32_ARGB8888, _32_RGB888, _24_RGB888, _16_RGB565, _16_RGB555,
        _16_ARGB1555, _16_ARGB4444, _8_RGB332, _8_ARGB2222,
        _DXT1, _DXT2, _DXT3, _DXT4, _DXT5
    };

    for (VX_PIXELFORMAT format : formats) {
        const char* str = utils::PixelFormat2String(format);
        VX_PIXELFORMAT converted = utils::String2PixelFormat(str, strlen(str));
        EXPECT_EQ(format, converted) << "Failed for format: " << str;
    }
}

// INI file operation tests
TEST_F(UtilsTest, IniGetString) {
    CreateTestIni(R"(
[Section1]
Key1=Value1
Key2=Value with spaces
Key3=

[Section2]
AnotherKey=AnotherValue
)");

    char buffer[256];

    // Test successful retrieval
    EXPECT_TRUE(utils::IniGetString("Section1", "Key1", buffer, 256, testIniPath.string().c_str()));
    EXPECT_STREQ(buffer, "Value1");

    EXPECT_TRUE(utils::IniGetString("Section1", "Key2", buffer, 256, testIniPath.string().c_str()));
    EXPECT_STREQ(buffer, "Value with spaces");

    // Test empty value - GetPrivateProfileString returns 0 for empty values, so this will be false
    EXPECT_FALSE(utils::IniGetString("Section1", "Key3", buffer, 256, testIniPath.string().c_str()));

    // Test non-existent key
    EXPECT_FALSE(utils::IniGetString("Section1", "NonExistent", buffer, 256, testIniPath.string().c_str()));

    // Test non-existent section
    EXPECT_FALSE(utils::IniGetString("NonExistent", "Key1", buffer, 256, testIniPath.string().c_str()));
}

TEST_F(UtilsTest, IniGetInteger) {
    CreateTestIni(R"(
[Numbers]
PositiveInt=42
NegativeInt=-17
Zero=0
InvalidInt=abc
EmptyInt=
)");

    int value;

    // Test successful retrieval
    EXPECT_TRUE(utils::IniGetInteger("Numbers", "PositiveInt", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, 42);

    EXPECT_TRUE(utils::IniGetInteger("Numbers", "NegativeInt", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, -17);

    EXPECT_TRUE(utils::IniGetInteger("Numbers", "Zero", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, 0);

    // Test invalid integer
    EXPECT_FALSE(utils::IniGetInteger("Numbers", "InvalidInt", value, testIniPath.string().c_str()));

    // Test empty value
    EXPECT_FALSE(utils::IniGetInteger("Numbers", "EmptyInt", value, testIniPath.string().c_str()));

    // Test non-existent key
    EXPECT_FALSE(utils::IniGetInteger("Numbers", "NonExistent", value, testIniPath.string().c_str()));
}

TEST_F(UtilsTest, IniGetBoolean) {
    CreateTestIni(R"(
[Booleans]
True1=1
True2=100
False1=0
NonExistent=
)");

    bool value;

    // Test true values
    EXPECT_TRUE(utils::IniGetBoolean("Booleans", "True1", value, testIniPath.string().c_str()));
    EXPECT_TRUE(value);

    EXPECT_TRUE(utils::IniGetBoolean("Booleans", "True2", value, testIniPath.string().c_str()));
    EXPECT_TRUE(value);

    // Test false value
    EXPECT_TRUE(utils::IniGetBoolean("Booleans", "False1", value, testIniPath.string().c_str()));
    EXPECT_FALSE(value);

    // Test non-existent key
    EXPECT_FALSE(utils::IniGetBoolean("Booleans", "NonExistent", value, testIniPath.string().c_str()));
}

TEST_F(UtilsTest, IniGetPixelFormat) {
    CreateTestIni(R"(
[Graphics]
Format1=_32_ARGB8888
Format2=_16_RGB565
Format3=_DXT1
InvalidFormat=_INVALID_FORMAT
EmptyFormat=
)");

    VX_PIXELFORMAT value;

    // Test successful retrieval
    EXPECT_TRUE(utils::IniGetPixelFormat("Graphics", "Format1", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, _32_ARGB8888);

    EXPECT_TRUE(utils::IniGetPixelFormat("Graphics", "Format2", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, _16_RGB565);

    EXPECT_TRUE(utils::IniGetPixelFormat("Graphics", "Format3", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, _DXT1);

    // Test invalid format (should still return true but with UNKNOWN_PF)
    EXPECT_TRUE(utils::IniGetPixelFormat("Graphics", "InvalidFormat", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, UNKNOWN_PF);

    // Test empty format
    EXPECT_FALSE(utils::IniGetPixelFormat("Graphics", "EmptyFormat", value, testIniPath.string().c_str()));

    // Test non-existent key
    EXPECT_FALSE(utils::IniGetPixelFormat("Graphics", "NonExistent", value, testIniPath.string().c_str()));
}

TEST_F(UtilsTest, IniSetString) {
    // Test setting string values
    EXPECT_TRUE(utils::IniSetString("TestSection", "TestKey", "TestValue", testIniPath.string().c_str()));

    // Verify the value was set
    char buffer[256];
    EXPECT_TRUE(utils::IniGetString("TestSection", "TestKey", buffer, 256, testIniPath.string().c_str()));
    EXPECT_STREQ(buffer, "TestValue");

    // Test overwriting existing value
    EXPECT_TRUE(utils::IniSetString("TestSection", "TestKey", "NewValue", testIniPath.string().c_str()));
    EXPECT_TRUE(utils::IniGetString("TestSection", "TestKey", buffer, 256, testIniPath.string().c_str()));
    EXPECT_STREQ(buffer, "NewValue");
}

TEST_F(UtilsTest, IniSetInteger) {
    // Test setting integer values
    EXPECT_TRUE(utils::IniSetInteger("Numbers", "TestInt", 12345, testIniPath.string().c_str()));

    // Verify the value was set
    int value;
    EXPECT_TRUE(utils::IniGetInteger("Numbers", "TestInt", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, 12345);

    // Test negative number
    EXPECT_TRUE(utils::IniSetInteger("Numbers", "NegativeInt", -999, testIniPath.string().c_str()));
    EXPECT_TRUE(utils::IniGetInteger("Numbers", "NegativeInt", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, -999);
}

TEST_F(UtilsTest, IniSetBoolean) {
    // Test setting boolean values
    EXPECT_TRUE(utils::IniSetBoolean("Flags", "TrueFlag", true, testIniPath.string().c_str()));
    EXPECT_TRUE(utils::IniSetBoolean("Flags", "FalseFlag", false, testIniPath.string().c_str()));

    // Verify the values were set
    bool value;
    EXPECT_TRUE(utils::IniGetBoolean("Flags", "TrueFlag", value, testIniPath.string().c_str()));
    EXPECT_TRUE(value);

    EXPECT_TRUE(utils::IniGetBoolean("Flags", "FalseFlag", value, testIniPath.string().c_str()));
    EXPECT_FALSE(value);
}

TEST_F(UtilsTest, IniSetPixelFormat) {
    // Test setting pixel format values
    EXPECT_TRUE(utils::IniSetPixelFormat("Graphics", "MainFormat", _32_ARGB8888, testIniPath.string().c_str()));
    EXPECT_TRUE(utils::IniSetPixelFormat("Graphics", "BackupFormat", _16_RGB565, testIniPath.string().c_str()));

    // Verify the values were set
    VX_PIXELFORMAT value;
    EXPECT_TRUE(utils::IniGetPixelFormat("Graphics", "MainFormat", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, _32_ARGB8888);

    EXPECT_TRUE(utils::IniGetPixelFormat("Graphics", "BackupFormat", value, testIniPath.string().c_str()));
    EXPECT_EQ(value, _16_RGB565);
}

// Integration tests
TEST_F(UtilsTest, IniRoundTripOperations) {
    std::string testFile = testIniPath.string();

    // Create the test file first (some Windows INI operations need the file to exist)
    CreateTestFile(testIniPath, "");

    // Set various types of values
    EXPECT_TRUE(utils::IniSetString("Test", "StringValue", "Hello World", testFile.c_str()));
    EXPECT_TRUE(utils::IniSetInteger("Test", "IntValue", 42, testFile.c_str()));
    EXPECT_TRUE(utils::IniSetBoolean("Test", "BoolValue", true, testFile.c_str()));
    EXPECT_TRUE(utils::IniSetPixelFormat("Test", "PixelValue", _32_ARGB8888, testFile.c_str()));

    // Read them back and verify
    char stringBuffer[256];
    int intValue;
    bool boolValue;
    VX_PIXELFORMAT pixelValue;

    EXPECT_TRUE(utils::IniGetString("Test", "StringValue", stringBuffer, 256, testFile.c_str()));
    EXPECT_STREQ(stringBuffer, "Hello World");

    EXPECT_TRUE(utils::IniGetInteger("Test", "IntValue", intValue, testFile.c_str()));
    EXPECT_EQ(intValue, 42);

    EXPECT_TRUE(utils::IniGetBoolean("Test", "BoolValue", boolValue, testFile.c_str()));
    EXPECT_TRUE(boolValue);

    EXPECT_TRUE(utils::IniGetPixelFormat("Test", "PixelValue", pixelValue, testFile.c_str()));
    EXPECT_EQ(pixelValue, _32_ARGB8888);
}

// Edge case and error handling tests
TEST_F(UtilsTest, EdgeCases) {
    // Test with very long paths
    std::string longPath(500, 'a');
    EXPECT_FALSE(utils::FileOrDirectoryExists(longPath.c_str()));

    // Test buffer size handling in path operations
    char smallBuffer[5];
    utils::ConcatPath(smallBuffer, 5, "ab", "cd");
    EXPECT_EQ(strlen(smallBuffer), 4); // Should be truncated but null-terminated

    // Test empty INI operations
    auto emptyIniPath = testDir / "empty.ini";
    CreateTestFile(emptyIniPath, "");

    char buffer[256];
    EXPECT_FALSE(utils::IniGetString("Section", "Key", buffer, 256, emptyIniPath.string().c_str()));

    int intVal;
    EXPECT_FALSE(utils::IniGetInteger("Section", "Key", intVal, emptyIniPath.string().c_str()));
}

// Performance test for operations that might be slow
TEST_F(UtilsTest, PerformanceTest) {
    std::string testFile = testIniPath.string();

    // Create the test file first
    CreateTestFile(testIniPath, "");

    auto start = std::chrono::high_resolution_clock::now();

    // Perform many INI operations
    for (int i = 0; i < 100; ++i) {
        std::string section = "Section" + std::to_string(i % 10);
        std::string key = "Key" + std::to_string(i);

        utils::IniSetInteger(section.c_str(), key.c_str(), i, testFile.c_str());

        int value;
        utils::IniGetInteger(section.c_str(), key.c_str(), value, testFile.c_str());
        EXPECT_EQ(value, i);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Should complete within reasonable time
    EXPECT_LT(duration.count(), 10000); // Less than 10 seconds for 100 operations
}
