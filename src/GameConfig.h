#ifndef BP_GAMECONFIG_PRIVATE_H
#define BP_GAMECONFIG_PRIVATE_H

#include "bp/GameConfig.h"
#include "bp/Value.h"

struct BpGameConfig {
    BpValue Values[BP_CONFIG_COUNT];
    BpValue Paths[BP_PATH_CATEGORY_COUNT];

    BpGameConfig();

    void Reset();

    bool Load(const char *filename);
    bool Save(const char *filename) const;

    const BpValue &operator[](BpConfigType type) const {
        return Get(type);
    }

    BpValue &operator[](BpConfigType type) {
        return Get(type);
    }

    const BpValue &Get(BpConfigType type) const {
        if (type < 0 || type >= BP_CONFIG_COUNT)
            return Values[BP_CONFIG_RESERVED];
        return Values[type];
    }

    BpValue &Get(BpConfigType type) {
        if (type < 0 || type >= BP_CONFIG_COUNT)
            return Values[BP_CONFIG_RESERVED];
        return Values[type];
    }

    bool HasPath(BpPathCategory category) const {
        if (category < 0 || category >= BP_PATH_CATEGORY_COUNT)
            return false;
        return !Paths[category].IsNone();
    }

    const char *GetPath(BpPathCategory category) const {
        if (category < 0 || category >= BP_PATH_CATEGORY_COUNT)
            return nullptr;
        return Paths[category].GetString();
    }

    void SetPath(BpPathCategory category, const char *path) {
        if (category < 0 || category >= BP_PATH_CATEGORY_COUNT)
            return;
        Paths[category] = path;
    }

    bool ResetPath(BpPathCategory category);
};

#endif // BP_GAMECONFIG_PRIVATE_H
