#include <iostream>
#include <string>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "PlayerOptions.h"

static int g_Failures = 0;

static void Check(const char *name, bool passed)
{
    std::cout << name << ": " << (passed ? "PASSED" : "FAILED") << std::endl;
    if (!passed)
        ++g_Failures;
}

static void TestConfigOptions()
{
    CmdlineParser parser("--log=vc6-test.log --verbose --width=800 --height 600 --disable-sort-transparent-objects -C --position-x -25");
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);
    playeroptions::ApplyConfigOptions(config, parser);

    Check("VC6 log path", std::string(config.GetPath(eLogPath)) == "vc6-test.log");
    Check("VC6 verbose", config.verbose);
    Check("VC6 width", config.width == 800);
    Check("VC6 height", config.height == 600);
    Check("VC6 new sort short option", !config.sortTransparentObjects);
    Check("VC6 new child short option", config.childWindowRendering);
    Check("VC6 negative value", config.posX == -25);
}

static void TestUnknownOptionsAreSkipped()
{
    CmdlineParser parser("--unknown value --width=1024 --unknown-flag");
    CGameConfig config;

    playeroptions::ApplyConfigOptions(config, parser);

    Check("VC6 unknown skipped", config.width == 1024);
}

static void TestConfigPathDoesNotStopPathScan()
{
    CmdlineParser parser("--config vc6.ini --log vc6.log");
    CGameConfig config;

    playeroptions::ApplyPathOptions(config, parser);

    Check("VC6 config path", std::string(config.GetPath(eConfigPath)) == "vc6.ini");
    Check("VC6 config continues path scan", std::string(config.GetPath(eLogPath)) == "vc6.log");
}

int main()
{
    TestConfigOptions();
    TestUnknownOptionsAreSkipped();
    TestConfigPathDoesNotStopPathScan();
    return g_Failures == 0 ? 0 : 1;
}
