#ifndef PLAYER_SOUNDMANAGER_H
#define PLAYER_SOUNDMANAGER_H

#include "CKSoundManager.h"
#include "CKWaveSound.h"
#include "XObjectArray.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#include <mmeapi.h>

#define DIRECTSOUND_VERSION 0x0800
#include <dsound.h>

class SoundManager : public CKSoundManager {
    friend class CKWaveSound;
public:
    // Get the caps of the sound manager
    CK_SOUNDMANAGER_CAPS GetCaps() override;

    // Creation
    void *CreateSource(CK_WAVESOUND_TYPE flags, CKWaveFormat *wf, CKDWORD bytes, CKBOOL streamed) override;
    void *DuplicateSource(void *source) override;
    void ReleaseSource(void *source) override;

    // Control
    void Play(CKWaveSound *, void *source, CKBOOL loop) override;
    void Pause(CKWaveSound *, void *source) override;
    void SetPlayPosition(void *source, int pos) override;
    int GetPlayPosition(void *source) override;
    CKBOOL IsPlaying(void *source) override;

    // PCM Buffer Information
    CKERROR SetWaveFormat(void *source, CKWaveFormat &wf) override;
    CKERROR GetWaveFormat(void *source, CKWaveFormat &wf) override;
    int GetWaveSize(void *source) override;

    // Buffer access
    CKERROR Lock(void *source, CKDWORD dwWriteCursor, CKDWORD dwNumBytes, void **pvAudioPtr1, CKDWORD *dwAudioBytes1, void **pvAudioPtr2, CKDWORD *dwAudioBytes2, CK_WAVESOUND_LOCKMODE dwFlags) override;
    CKERROR Unlock(void *source, void *pvAudioPtr1, CKDWORD dwNumBytes1, void *pvAudioPtr2, CKDWORD dwAudioBytes2) override;

    // 2D/3D Members Functions
    void SetType(void *source, CK_WAVESOUND_TYPE type) override;
    CK_WAVESOUND_TYPE GetType(void *source) override;

    //----------------------------------------------------------
    // 2D/3D Settings
    void UpdateSettings(void *source, CK_SOUNDMANAGER_CAPS settingsoptions, CKWaveSoundSettings &settings, CKBOOL set) override;

    //----------------------------------------------------------
    // 3D Settings
    void Update3DSettings(void *source, CK_SOUNDMANAGER_CAPS settingsoptions, CKWaveSound3DSettings &settings, CKBOOL set) override;

    //-----------------------------------------------------
    // Listener settings
    void UpdateListenerSettings(CK_SOUNDMANAGER_CAPS settingsoptions, CKListenerSettings &settings, CKBOOL set) override;

    // Initialization
    CKERROR OnCKInit() override;
    CKERROR OnCKEnd() override;

    // Pause Resume
    CKERROR OnCKReset() override;
    CKERROR OnCKPause() override;
    CKERROR OnCKPlay() override;

    CKERROR PostClearAll() override;
    CKERROR PostProcess() override;

    CKERROR PreLaunchScene(CKScene *OldScene, CKScene *NewScene) override;
    CKERROR SequenceToBeDeleted(CK_ID *objids, int count) override;

    CKDWORD GetValidFunctionsMask() override {
        return CKSoundManager::GetValidFunctionsMask() |
               CKMANAGER_FUNC_PostClearAll |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKReset |
               CKMANAGER_FUNC_OnCKPause |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_PostProcess |
               CKMANAGER_FUNC_OnSequenceToBeDeleted |
               CKMANAGER_FUNC_PreLaunchScene;
    }

    CKBOOL IsInitialized() override;

    explicit SoundManager(CKContext *Context);

protected:
    void InternalPause(void *source) override;
    void InternalPlay(void *source, CKBOOL loop) override;

    // List of sound to resume
    XObjectArray m_SoundsPlaying;

    // Top level Root Interface for A3D
    LPDIRECTSOUND m_Root;
    // Primary Buffer
    LPDIRECTSOUNDBUFFER m_Primary;
    // Interface to control the listener parameters
    LPDIRECTSOUND3DLISTENER m_Listener;
};

long FloatToDb(float f);
float DbToFloat(long d);
long FloatPanningToDb(float Panning);
float DbPanningToFloat(long d);

#endif // PLAYER_SOUNDMANAGER_H
