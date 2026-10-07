#include "PlayerOptions.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

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

    bool ApplyStringOption(CGameConfig &config, CmdlineParser &parser, const char *longopt, char shortopt,
                           XString CGameConfig::*member)
    {
        CmdlineArg arg;
        XString value;
        if (!longopt && shortopt == '\0')
            return false;
        if (!parser.Next(arg, longopt, shortopt, 1))
            return false;

        if (arg.GetValue(0, value))
            config.*member = value;
        return true;
    }

    bool IsDriverIndex(const XString &text)
    {
        if (text.IsEmpty())
            return false;

        errno = 0;
        char *end = NULL;
        const long value = strtol(text.CStr(), &end, 10);
        return errno == 0 && *end == '\0' && value >= 0 && value <= INT_MAX;
    }

    // --rasterizer and --video-driver select the same driver and may not be
    // combined. Both are validated before any option is applied.
    bool CheckRasterizerSelectors(CmdlineParser &parser, bool &numeric)
    {
        bool named = false;
        bool valid = true;
        numeric = false;

        while (!parser.Done())
        {
            CmdlineArg arg;
            XString value;
            if (parser.Next(arg, "--rasterizer", '\0', 1))
            {
                named = true;
                if (!arg.GetValue(0, value) || !playeroptions::IsRasterizerName(value.CStr()))
                    valid = false;
            }
            else if (parser.Next(arg, "--video-driver", 'v', 1))
            {
                numeric = true;
                if (!arg.GetValue(0, value) || !IsDriverIndex(value))
                    valid = false;
            }
            else
            {
                parser.Skip();
            }
        }

        parser.Reset();
        return valid && !(named && numeric);
    }
}

namespace playeroptions
{
    void ApplyPathOptions(CGameConfig &config, CmdlineParser &parser)
    {
        CmdlineArg arg;
        XString path;
        bool explicitPaths[ePathCategoryCount] = { false };

        while (!parser.Done())
        {
            bool matched = false;
#define X_PATH(category, defaultPath, cliLong, validateDir) \
            if (parser.Next(arg, cliLong, '\0', 1)) \
            { \
                if (arg.GetValue(0, path)) \
                { \
                    config.SetPath(category, path.CStr()); \
                    explicitPaths[category] = true; \
                } \
                matched = true; \
            } \
            if (matched) \
                continue;
            GAMECONFIG_PATH_FIELDS
#undef X_PATH

            parser.Skip();
        }

        parser.Reset();

        if (explicitPaths[eRootPath])
        {
            for (int i = ePluginPath; i < ePathCategoryCount; ++i)
            {
                if (!explicitPaths[i])
                    config.ResetPath((PathCategory)i);
            }
        }
    }

    bool IsRasterizerName(const char *name)
    {
        return OptionNameEquals(name, "sdlgpu") || OptionNameEquals(name, "null");
    }

    const char *RasterizerNameForDriver(const char *description)
    {
        // Driver descriptions registered by the RenderEngine rasterizers.
        if (OptionNameEquals(description, "SDL_gpu Driver"))
            return "sdlgpu";
        if (OptionNameEquals(description, "NULL Rasterizer"))
            return "null";
        return "";
    }

    bool ApplyConfigOptions(CGameConfig &config, CmdlineParser &parser)
    {
        bool numeric = false;
        if (!CheckRasterizerSelectors(parser, numeric))
            return false;

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
#define X_STRING(sec,key,member,def,cliLong,cliShort) \
            if (ApplyStringOption(config, parser, cliLong, cliShort, &CGameConfig::member)) \
                matched = true; \
            if (matched) \
                continue;
            GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT
#undef X_STRING

            parser.Skip();
        }

        parser.Reset();

        // A numeric driver on the command line overrides a configured name.
        if (numeric)
            config.rasterizer.Clear();
        return config.rasterizer.IsEmpty() || IsRasterizerName(config.rasterizer.CStr());
    }

    bool ApplyRuntimeOptions(CGameConfig &config, CmdlineParser &parser)
    {
        ApplyPathOptions(config, parser);
        return ApplyConfigOptions(config, parser);
    }

    int GetConfigOptionCount()
    {
        int count = 0;
#define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) if (cliLong || cliShort != '\0') ++count;
#define X_INT(sec,key,member,def,cliLong,cliShort) if (cliLong || cliShort != '\0') ++count;
#define X_STRING(sec,key,member,def,cliLong,cliShort) if (cliLong || cliShort != '\0') ++count;
        GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT
#undef X_STRING
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

    bool HasConfigOption(const char *longopt, char shortopt)
    {
#define X_BOOL(sec,key,member,def,cliLong,cliShort,cliValue) \
        if ((OptionNameEquals(cliLong, longopt) || !longopt) && cliShort == shortopt) return true;
#define X_INT(sec,key,member,def,cliLong,cliShort) \
        if ((OptionNameEquals(cliLong, longopt) || !longopt) && cliShort == shortopt) return true;
#define X_STRING(sec,key,member,def,cliLong,cliShort) \
        if ((OptionNameEquals(cliLong, longopt) || !longopt) && cliShort == shortopt) return true;
        GAMECONFIG_FIELDS
#undef X_BOOL
#undef X_INT
#undef X_STRING
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
