#include "CKAll.h"

#include "bp/Logger.h"
#include "ScriptUtils.h"
#include "InterfaceManager.h"
#include "GameConfig.h"

static bool SetDebugMode(CKBehavior *setDebugMode) {
    if (!setDebugMode)
        return false;

    CKBehavior *bs = scriptutils::GetBehavior(setDebugMode, "Binary Switch");
    if (!bs)
        return false;

    CKBOOL debug = TRUE;
    scriptutils::GenerateInputParameter(setDebugMode, bs, 0, debug);

    return true;
}

static bool SetLanguage(CKBehavior *setLanguage, int langId) {
    if (!setLanguage)
        return false;

    CKBehavior *gc = scriptutils::GetBehavior(setLanguage, "Get Cell");
    if (!gc)
        return false;

    CKBehaviorLink *linkIn0 = nullptr;
    const int linkCount = setLanguage->GetSubBehaviorLinkCount();
    for (int i = 0; i < linkCount; ++i) {
        CKBehaviorLink *link = setLanguage->GetSubBehaviorLink(i);
        CKBehaviorIO *outIO = link->GetOutBehaviorIO();
        if (outIO->GetOwner() == gc && gc->GetInput(0) == outIO)
            linkIn0 = link;
    }
    if (!linkIn0)
        return false;

    CKBehaviorLink *linkRrOp = scriptutils::RemoveBehaviorLink(setLanguage, "TT_ReadRegistry", "Op", 0, 0);
    if (!linkRrOp)
        return false;

    CKBehavior *op2 = linkRrOp->GetOutBehaviorIO()->GetOwner();
    scriptutils::SetInputParameterValue(op2, 0, langId);
    CKDestroyObject(linkRrOp);

    linkIn0->SetOutBehaviorIO(op2->GetInput(0));

    return true;
}

static int ListDriver(const CKBehaviorContext &behcontext) {
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKDataArray *drivers = (CKDataArray *) beh->GetInputParameterObject(0);
    if (!drivers) {
        context->OutputToConsoleExBeep("ListDriver: No DataArray Object is found.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    drivers->GetColumnCount();
    drivers->Clear();
    while (drivers->GetColumnCount() > 0)
        drivers->RemoveColumn(0);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man) {
        context->OutputToConsoleExBeep("ListDriver: im == nullptr");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    drivers->InsertColumn(-1, CKARRAYTYPE_STRING, "DriverDesc");
    drivers->InsertColumn(0, CKARRAYTYPE_STRING, "DriverName");
    drivers->InsertColumn(1, CKARRAYTYPE_INT, "DriverID");

    const int driverCount = context->GetRenderManager()->GetRenderDriverCount();
    for (int i = 0; i < driverCount; ++i) {
        VxDriverDesc *drDesc = context->GetRenderManager()->GetRenderDriverDescription(i);
        drivers->InsertRow();
#if CKVERSION == 0x13022002
        drivers->SetElementStringValue(i, 0, drDesc->DriverName);
#else
        drivers->SetElementStringValue(i, 0, drDesc->DriverName.Str());
#endif
        drivers->SetElementValue(i, 1, &i, sizeof(int));
#if CKVERSION == 0x13022002
        drivers->SetElementStringValue(i, 2, drDesc->DriverDesc);
#else
        drivers->SetElementStringValue(i, 2, drDesc->DriverDesc.Str());
#endif
    }

    int driver = man->GetDriver();
    beh->SetOutputParameterValue(0, &driver, sizeof(int));
    beh->ActivateOutput(0);
    return CKBR_OK;
}

static bool ReplaceListDriver(CKBehavior *screenModes) {
    if (!screenModes)
        return false;

    CKBehavior *ld = scriptutils::GetBehavior(screenModes, "TT List Driver");
    if (!ld)
        return false;

    ld->SetFunction(ListDriver);
    return true;
}

static int ListScreenModes(const CKBehaviorContext &behcontext) {
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int driverId = 0;
    beh->GetInputParameterValue(0, &driverId);

    CKDataArray *screenModes = (CKDataArray *) beh->GetInputParameterObject(1);
    if (!screenModes) {
        context->OutputToConsoleExBeep("ListScreenModes: No DataArray Object is found.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    screenModes->Clear();
    while (screenModes->GetColumnCount() > 0)
        screenModes->RemoveColumn(0);

    screenModes->InsertColumn(-1, CKARRAYTYPE_INT, "Bpp");
    screenModes->InsertColumn(0, CKARRAYTYPE_INT, "Mode");
    screenModes->InsertColumn(1, CKARRAYTYPE_INT, "Width");
    screenModes->InsertColumn(2, CKARRAYTYPE_INT, "Height");

    VxDriverDesc *drDesc = context->GetRenderManager()->GetRenderDriverDescription(driverId);
    if (!drDesc) {
        context->OutputToConsoleExBeep("ListScreenModes: No Driver Description for Driver-ID '%d' is found", driverId);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

#if CKVERSION == 0x13022002
    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
#else
    XArray<VxDisplayMode> &dm = drDesc->DisplayModes;
    const int dmCount = dm.Size();
#endif
    int i = 0, row = 0;
    while (i < dmCount) {
        int width = dm[i].Width;
        int height = dm[i].Height;

        int maxRefreshRate = 0;
        for (int j = i; j < dmCount && dm[j].Width == width && dm[j].Height == height; ++j) {
            if (dm[j].Bpp > 8 && dm[j].RefreshRate > maxRefreshRate)
                maxRefreshRate = dm[j].RefreshRate;
        }

        while (i < dmCount && dm[i].Width == width && dm[i].Height == height) {
            if (dm[i].Bpp > 8 && dm[i].RefreshRate == maxRefreshRate) {
                screenModes->InsertRow();
                screenModes->SetElementValue(row, 0, &i, sizeof(int));
                screenModes->SetElementValue(row, 1, &dm[i].Width, sizeof(int));
                screenModes->SetElementValue(row, 2, &dm[i].Height, sizeof(int));
                screenModes->SetElementValue(row, 3, &dm[i].Bpp, sizeof(int));
                ++row;
            }
            ++i;
        }
    }

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man) {
        context->OutputToConsoleExBeep("ListScreenModes: im == nullptr");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    int screenMode = man->GetScreenMode();
    beh->SetOutputParameterValue(0, &screenMode, sizeof(int));
    beh->ActivateOutput(0);
    return CKBR_OK;
}

static bool ReplaceListScreenModes(CKBehavior *screenModes) {
    if (!screenModes)
        return false;

    CKBehavior *ls = scriptutils::GetBehavior(screenModes, "TT List ScreenModes");
    if (!ls)
        return false;

    ls->SetFunction(ListScreenModes);
    return true;
}

static bool UnlockWidescreen(CKBehavior *screenModes, CKBehavior *minWidth) {
    if (!screenModes || !minWidth)
        return false;

    CKBehavior *ic = scriptutils::GetBehavior(screenModes, "Insert Column", "ScreenModes");
    if (!ic)
        return false;

    CKBehaviorLink *linkScIc = scriptutils::RemoveBehaviorLink(screenModes, "Set Cell", ic, 0, 0);
    if (!linkScIc)
        return false;

    CKBehavior *sc = linkScIc->GetInBehaviorIO()->GetOwner();
    screenModes->RemoveSubBehaviorLink(linkScIc);
    CKDestroyObject(linkScIc);

    scriptutils::RemoveBehaviorLink(screenModes, "Remove Column", minWidth, 0, 0, true);
    scriptutils::CreateBehaviorLink(screenModes, sc, minWidth, 0, 0);

    return true;
}

static bool UnlockHighResolution(CKBehavior *screenModes, CKBehavior *bppFilter, CKBehavior *minWidth,
                                 CKBehavior *maxWidth, bool unlockWidescreen) {
    if (!screenModes || !bppFilter || !minWidth || !maxWidth)
        return false;

    const char *inBehName = (unlockWidescreen) ? "Set Cell" : "Remove Column";
    CKBehaviorLink *linkRri = scriptutils::RemoveBehaviorLink(screenModes, inBehName, minWidth, 0, 0);
    if (!linkRri)
        return false;

    CKBehavior *inBeh = linkRri->GetInBehaviorIO()->GetOwner();
    CKDestroyObject(linkRri);

    scriptutils::RemoveBehaviorLink(screenModes, maxWidth, bppFilter, 0, 0, true);
    scriptutils::CreateBehaviorLink(screenModes, inBeh, bppFilter, 0, 0);

    return true;
}

static bool UnlockFramerate(CKBehavior *synchToScreen) {
    if (!synchToScreen)
        return false;

    bool res = false;

    for (int i = 0; i < synchToScreen->GetSubBehaviorCount(); ++i) {
        CKBehavior *beh = synchToScreen->GetSubBehavior(i);
        if (strcmp(beh->GetName(), "Time Settings") == 0) {
            CKDWORD *frameRate = (CKDWORD *) beh->GetInputParameterReadDataPtr(0);
            if (*frameRate == 3) // Frame Rate == Limit
            {
                *frameRate = 1; // Frame Rate = Free
                res = true;
                break;
            }
        }
    }

    return res;
}

static bool SkipResolutionCheck(CKBehavior *synchToScreen) {
    if (!synchToScreen)
        return false;

    CKBehavior *ii = scriptutils::GetBehavior(synchToScreen, "Iterator If");
    CKBehavior *delayer = scriptutils::GetBehavior(synchToScreen, "Delayer");
    if (!(ii && delayer))
        return false;

    CKBehaviorLink *linkTsIi1 = scriptutils::GetBehaviorLink(synchToScreen, "Time Settings", ii, 0, 0);
    CKBehaviorLink *linkTsIi2 = scriptutils::GetBehaviorLink(synchToScreen, "Time Settings", ii, 0, 0, linkTsIi1);
    if (!(linkTsIi1 && linkTsIi2))
        return false;

    CKBehaviorLink *linkDelayerCsm = scriptutils::RemoveBehaviorLink(synchToScreen, delayer, "TT Change ScreenMode", 0,
                                                                     0);
    if (!linkDelayerCsm)
        return false;

    linkTsIi1->SetOutBehaviorIO(linkDelayerCsm->GetOutBehaviorIO());
    linkTsIi2->SetOutBehaviorIO(linkDelayerCsm->GetOutBehaviorIO());
    CKDestroyObject(linkDelayerCsm);

    return true;
}

static bool SkipOpeningAnimation(CKBehavior *defaultLevel, CKBehavior *synchToScreen) {
    if (!defaultLevel || !synchToScreen)
        return false;

    CKBehavior *is = scriptutils::GetBehavior(defaultLevel, "Intro Start");
    CKBehavior *ie = scriptutils::GetBehavior(defaultLevel, "Intro Ende");
    CKBehavior *ml = scriptutils::GetBehavior(defaultLevel, "Main Loading");
    CKBehavior *ps = scriptutils::GetBehavior(defaultLevel, "Preload Sound");
    if (!(is && ie && ml && ps))
        return false;

    CKBehaviorLink *linkStsIs = scriptutils::GetBehaviorLink(defaultLevel, synchToScreen, is, 0, 0);
    if (!linkStsIs)
        return false;

    linkStsIs->SetOutBehaviorIO(ml->GetInput(0));

    CKBehaviorLink *linkIeAs = scriptutils::GetBehaviorLink(defaultLevel, ie, "Activate Script", 0, 0);
    if (!linkIeAs)
        return false;

    CKBehaviorLink *linkPsWfa = scriptutils::GetBehaviorLink(defaultLevel, ps, "Wait For All", 0, 0);
    if (!linkPsWfa)
        return false;

    linkPsWfa->SetOutBehaviorIO(linkIeAs->GetOutBehaviorIO());
    return true;
}

bool EditScript(CKLevel *level, const BpGameConfig &config) {
    if (!level)
        return false;

    CKBehavior *defaultLevel = scriptutils::GetBehavior(level->ComputeObjectList(CKCID_BEHAVIOR), "Default Level");
    if (!defaultLevel) {
        bpLogWarn("Unable to find Default Level");
        return false;
    }

    // Set debug mode
    if (config.Debug->GetBool()) {
        if (!SetDebugMode(scriptutils::GetBehavior(defaultLevel, "set DebugMode")))
            bpLogWarn("Failed to set debug mode");
    }

    // Bypass "Set Language" script and set our language id
    if (!SetLanguage(scriptutils::GetBehavior(defaultLevel, "Set Language"), config.LangId->GetInt32()))
        bpLogWarn("Failed to set language id");

    int i;

    CKBehavior *sm = scriptutils::GetBehavior(defaultLevel, "Screen Modes");
    if (!sm) {
        bpLogWarn("Unable to find script Screen Modes");
        return false;
    }

    if (!ReplaceListDriver(sm)) {
        bpLogWarn("Failed to set driver");
        return false;
    }

    if (!ReplaceListScreenModes(sm)) {
        bpLogWarn("Failed to set screen mode");
        return false;
    }

    CKBehavior *bbpFilter = nullptr;
    CKBehavior *minWidth = nullptr;
    CKBehavior *maxWidth = nullptr;
    for (i = 0; i < sm->GetSubBehaviorCount(); ++i) {
        CKBehavior *beh = sm->GetSubBehavior(i);
        if (strcmp(beh->GetName(), "Remove Row If") == 0) {
            switch (scriptutils::GetInputParameterValue<int>(beh, 2)) {
                case 16: // BBP filter
                    bbpFilter = beh;
                    break;
                case 640: // Minimum width
                    minWidth = beh;
                    break;
                case 1600: // Maximum width
                    maxWidth = beh;
                    break;
                default:
                    break;
            }
        }
    }

    // Correct the bbp filter
    if (!bbpFilter) {
        bpLogWarn("Failed to correct the bbp filter");
    } else {
        scriptutils::SetInputParameterValue(bbpFilter, 2, config.Bpp->GetInt32());
    }

    // Unlock widescreen (Not 4:3)
    if (config.UnlockWidescreen->GetBool()) {
        if (!UnlockWidescreen(sm, minWidth))
            bpLogWarn("Failed to unlock widescreen");
    }

    // Unlock high resolution
    if (config.UnlockHighResolution->GetBool()) {
        if (!UnlockHighResolution(sm, bbpFilter, minWidth, maxWidth, config.UnlockWidescreen->GetBool()))
            bpLogWarn("Failed to unlock high resolution");
    }

    CKBehavior *sts = scriptutils::GetBehavior(defaultLevel, "Synch to Screen");
    if (!sts) {
        bpLogWarn("Unable to find script Synch to Screen");
        return false;
    }

    // Unlock frame rate limitation
    if (config.UnlockFramerate->GetBool()) {
        if (!UnlockFramerate(sts))
            bpLogWarn("Failed to unlock frame rate limitation");
    }

    // Make it not to test 640x480 resolution
    if (!SkipResolutionCheck(sts))
        bpLogWarn("Failed to bypass 640x480 resolution test");

    // Skip Opening Animation
    if (config.SkipOpening->GetBool()) {
        if (!SkipOpeningAnimation(defaultLevel, sts))
            bpLogWarn("Failed to skip opening animation");
    }

    return true;
}
