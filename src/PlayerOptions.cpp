#include "PlayerOptions.h"

#include <string.h>

#include "CmdlineParser.h"
#include "Utils.h"

namespace
{
    bool OptionNameEquals(const char *lhs, const char *rhs)
    {
        if (!lhs || !rhs)
            return false;
        return strcmp(lhs, rhs) == 0;
    }

    bool ApplyBoolOption(CGameConfig &config, CmdlineParser &parser, const char *longopt, char shortopt,
                         bool CGameConfig::*member, bool value)
    {
        CmdlineArg arg;
        if (!longopt && shortopt == '\0')
            return false;
        if (!parser.Next(arg, longopt, shortopt))
            return false;

        config.*member = value;
        return true;
    }

    bool ApplyIntOption(CGameConfig &config, CmdlineParser &parser, const char *longopt, char shortopt,
                        int CGameConfig::*member)
    {
        CmdlineArg arg;
        long value = 0;
        if (!longopt && shortopt == '\0')
            return false;
        if (!parser.Next(arg, longopt, shortopt, 1))
            return false;

        if (arg.GetValue(0, value))
            config.*member = (int)value;
        return true;
    }

    bool ApplyPixelFormatOption(CGameConfig &config, CmdlineParser &parser, const char *longopt, char shortopt,
                                VX_PIXELFORMAT CGameConfig::*member)
    {
        CmdlineArg arg;
        std::string value;
        if (!longopt && shortopt == '\0')
            return false;
        if (!parser.Next(arg, longopt, shortopt, 1))
            return false;

        if (arg.GetValue(0, value))
            config.*member = utils::String2PixelFormat(value.c_str(), 16);
        return true;
    }
}

namespace playeroptions
{
    static bool HasConfigPathOption(CmdlineParser &parser)
    {
        bool found = false;
        CmdlineArg arg;

        while (!parser.Done())
        {
            if (parser.Next(arg, "--config", '\0', 1))
            {
                found = true;
                break;
            }
            parser.Skip();
        }

        parser.Reset();
        return found;
    }

    void ApplyPathOptions(CGameConfig &config, CmdlineParser &parser)
    {
        CmdlineArg arg;
        std::string path;

        while (!parser.Done())
        {
            bool matched = false;
#define X_PATH(category, defaultPath, cliLong, validateDir) \
            if (parser.Next(arg, cliLong, '\0', 1)) \
            { \
                if (arg.GetValue(0, path)) \
                    config.SetPath(category, path.c_str()); \
                matched = true; \
            } \
            if (matched) \
                continue;
            GAMECONFIG_PATH_FIELDS
#undef X_PATH

            parser.Skip();
        }

        parser.Reset();

#define X_PATH(category, defaultPath, cliLong, validateDir) \
        if (validateDir && !utils::DirectoryExists(config.GetPath(category))) \
            config.ResetPath(category);
        GAMECONFIG_PATH_FIELDS
#undef X_PATH
    }

    void ApplyConfigToolPathOptions(CGameConfig &config, CmdlineParser &parser)
    {
        if (!HasConfigPathOption(parser))
            config.SetPath(eConfigPath, "");

        ApplyPathOptions(config, parser);
    }

    void ApplyConfigOptions(CGameConfig &config, CmdlineParser &parser)
    {
        while (!parser.Done())
        {
            bool matched = false;

#define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
            if (ApplyBoolOption(config, parser, cliLong, cliShort, &CGameConfig::member, cliValue)) \
                matched = true; \
            if (matched) \
                continue;
#define X_INT(sec,key,member,def,cliLong,cliShort) \
            if (ApplyIntOption(config, parser, cliLong, cliShort, &CGameConfig::member)) \
                matched = true; \
            if (matched) \
                continue;
#define X_PF(sec,key,member,def,cliLong,cliShort) \
            if (ApplyPixelFormatOption(config, parser, cliLong, cliShort, &CGameConfig::member)) \
                matched = true; \
            if (matched) \
                continue;
            GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT
#undef X_PF

            parser.Skip();
        }

        parser.Reset();
    }

    int GetConfigOptionCount()
    {
        int count = 0;
#define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) if (cliLong || cliShort != '\0') ++count;
#define X_INT(sec,key,member,def,cliLong,cliShort) if (cliLong || cliShort != '\0') ++count;
#define X_PF(sec,key,member,def,cliLong,cliShort) if (cliLong || cliShort != '\0') ++count;
        GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT
#undef X_PF
        return count;
    }

    int GetPathOptionCount()
    {
        int count = 0;
#define X_PATH(category, defaultPath, cliLong, validateDir) ++count;
        GAMECONFIG_PATH_FIELDS
#undef X_PATH
        return count;
    }

    bool IsConfigToolMode(CmdlineParser &parser)
    {
        bool found = false;
        CmdlineArg arg;

        while (!parser.Done())
        {
            if (parser.Next(arg, "--config-tool", '\0'))
            {
                found = true;
                break;
            }
            parser.Skip();
        }

        parser.Reset();
        return found;
    }

    bool HasConfigOption(const char *longopt, char shortopt)
    {
#define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        if ((OptionNameEquals(cliLong, longopt) || !longopt) && cliShort == shortopt) return true;
#define X_INT(sec,key,member,def,cliLong,cliShort) \
        if ((OptionNameEquals(cliLong, longopt) || !longopt) && cliShort == shortopt) return true;
#define X_PF(sec,key,member,def,cliLong,cliShort) \
        if ((OptionNameEquals(cliLong, longopt) || !longopt) && cliShort == shortopt) return true;
        GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT
#undef X_PF
        return false;
    }

    bool HasPathOption(const char *longopt)
    {
#define X_PATH(category, defaultPath, cliLong, validateDir) if (OptionNameEquals(cliLong, longopt)) return true;
        GAMECONFIG_PATH_FIELDS
#undef X_PATH
        return false;
    }
}
