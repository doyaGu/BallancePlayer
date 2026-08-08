#include "StaticPlugins.h"

#include "CKPluginManager.h"

#ifdef BALLANCE_STATIC_MODULES

typedef CKPluginInfo *(*StaticPluginGetInfoFunction)(int);
typedef int (*StaticPluginGetInfoCountFunction)();
typedef CKDataReader *(*StaticPluginGetReaderFunction)(int);
typedef void (*StaticPluginRegisterDeclarationsFunction)(XObjectDeclarationArray *);

struct StaticPlugin {
    const char *Name;
    StaticPluginGetInfoCountFunction GetInfoCount;
    StaticPluginGetInfoFunction GetInfo;
    StaticPluginGetReaderFunction GetReader;
    StaticPluginRegisterDeclarationsFunction RegisterDeclarations;
};

#include "StaticPluginRegistry.generated.h"

bool RegisterStaticPlugins(CKPluginManager *pluginManager)
{
    if (!pluginManager)
        return false;

    const int pluginCount = sizeof(kStaticPlugins) / sizeof(kStaticPlugins[0]);
    for (int i = 0; i < pluginCount; ++i) {
        const StaticPlugin &plugin = kStaticPlugins[i];
        CKERROR err = pluginManager->RegisterStaticPlugin(
                const_cast<CKSTRING>(plugin.Name),
                plugin.GetInfoCount,
                plugin.GetInfo,
                plugin.GetReader,
                plugin.RegisterDeclarations);
        if (err != CK_OK && err != CKERR_ALREADYPRESENT)
            return false;
    }

    return true;
}

#endif
