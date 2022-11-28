#ifndef PLAYER_INTERFACEMANAGER_H
#define PLAYER_INTERFACEMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

#include "GameInfo.h"

#define TT_INTERFACE_MANAGER_GUID CKGUID(0x30833801, 0x6DEE620D)

enum TT_MSG
{
    TT_MSG_NO_GAMEINFO = 0x5F7,
    TT_MSG_CMO_RESTART = 0x5F8,
    TT_MSG_CMO_LOAD = 0x5F9,
    TT_MSG_EXIT_TO_SYS = 0x5FA,
    TT_MSG_EXIT_TO_TITLE = 0x5FB,
    TT_MSG_LIMIT_FPS = 0x5FC,
    TT_MSG_SCREEN_MODE_CHG = 0x5FD,
    TT_MSG_GO_FULLSCREEN = 0x678,
    TT_MSG_STOP_FULLSCREEN = 0x679,
};

class InterfaceManager : public CKBaseManager
{
public:
    InterfaceManager(CKContext *ctx);
    ~InterfaceManager();

    int GetScreenMode() const
    {
        return m_ScreenMode;
    }

    void SetScreenMode(int screenMode)
    {
        m_ScreenMode = screenMode;
    }

    int GetDriver() const
    {
        return m_Driver;
    }

    void SetDriver(int driver)
    {
        m_Driver = driver;
    }

    bool IsTaskSwitchEnabled() const
    {
        return m_TaskSwitchEnabled;
    }

    void SetTaskSwitchEnabled(bool enable)
    {
        m_TaskSwitchEnabled = enable;
    }

    bool IsRookie() const
    {
        return m_Rookie;
    }

    void SetRookie(bool rookie)
    {
        m_Rookie = rookie;
    }

    CGameInfo *GetGameInfo() const
    {
        return m_GameInfo;
    }

    void SetGameInfo(CGameInfo *gameInfo)
    {
        m_GameInfo = gameInfo;
    }

    char *GetCmoName()
    {
        return &m_CmoName[0];
    }

    void SetCmoName(const char *name)
    {
        strncpy(m_CmoName, name, sizeof(m_CmoName));
    }

    bool IsWindowActivated() const
    {
        return m_WindowActivated;
    }

    void SetWindowActivated(bool enable = true)
    {
        m_WindowActivated = enable;
    }

    static InterfaceManager *GetManager(CKContext *context)
    {
        return (InterfaceManager *)context->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    }

protected:
    int m_ScreenMode;
    int m_Driver;
    bool m_TaskSwitchEnabled;
    bool m_Rookie;
    CGameInfo *m_GameInfo;
    char m_CmoName[640];
    bool m_WindowActivated;
    int m_ArrayList[4];
};

#endif // PLAYER_INTERFACEMANAGER_H