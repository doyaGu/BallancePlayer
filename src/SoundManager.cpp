#include "SoundManager.h"

#include "CKContext.h"
#include "CKTimeManager.h"
#include "CK3dEntity.h"
#include "VxMatrix.h"

long g_InitialVolume = -600;
CKBOOL g_InitialVolumeChanged = FALSE;

long FloatToDb(float f) {
    if (f <= 0.0f)
        return -10000;
    return (long) (2000.0 * log10(f));
}

float DbToFloat(long d) {
    if (d <= -10000)
        return 0.0f;
    else if (d > 0)
        return 1.0f;
    return powf(10, d / 2000.0f);
}

long FloatPanningToDb(float Panning) {
    if (Panning == 0.0f)
        return 0;
    long l;
    if (Panning > 0.0f)
        l = -FloatToDb(1.0f - Panning);
    else
        l = FloatToDb(1.0f + Panning);

    return l;
}

float DbPanningToFloat(long d) {
    if (d > 0) {
        return 1 - DbToFloat(-d);
    } else
        return -1.0f + DbToFloat(d);
}

CK_SOUNDMANAGER_CAPS SoundManager::GetCaps() {
    CKDWORD d = CK_WAVESOUND_SETTINGS_ALL | CK_WAVESOUND_3DSETTINGS_ALL | CK_LISTENERSETTINGS_ALL |
                CK_WAVESOUND_3DSETTINGS_DISTANCEFACTOR | CK_WAVESOUND_3DSETTINGS_DOPPLERFACTOR;
    d &= ~(CK_WAVESOUND_SETTINGS_EQUALIZATION | CK_WAVESOUND_SETTINGS_PRIORITY | CK_LISTENERSETTINGS_EQ |
           CK_LISTENERSETTINGS_PRIORITY | CK_SOUNDMANAGER_ONFLYTYPE);
    return (CK_SOUNDMANAGER_CAPS) d;
}

// Creation
void *SoundManager::CreateSource(CK_WAVESOUND_TYPE type, CKWaveFormat *wf, CKDWORD bytes, CKBOOL streamed) {
    if (m_Context->GetStartOptions() & CK_CONFIG_DISABLEDSOUND) {
        if (m_Context->IsInInterfaceMode())
            m_Context->OutputToConsole("Cannot create sound : Sound Disabled");
        return nullptr;
    }
    if (!m_Root) {
        if (m_Context->IsInInterfaceMode())
            m_Context->OutputToConsole("Cannot create sound : Sound Manager not initialized");
        return nullptr;
    }

    // Set up the direct sound buffer, only requesting the flags we need,
    // since each require overhead.
    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;

    // Buffer must be a multiple of samples
    dsbd.dwBufferBytes = bytes;
    dsbd.lpwfxFormat = (WAVEFORMATEX *) wf;

    // We test if we want a 2D or 3D Sound
    if (type == CK_WAVESOUND_BACKGROUND)
        dsbd.dwFlags |= DSBCAPS_CTRLPAN;
    else
        dsbd.dwFlags |= DSBCAPS_CTRL3D; //|DSBCAPS_MUTE3DATMAXDISTANCE;

    LPDIRECTSOUNDBUFFER lpdsb = nullptr;

    // Create the static DirectSound buffer
    HRESULT hr = m_Root->CreateSoundBuffer(&dsbd, &lpdsb, nullptr);
    if (hr)
        return nullptr;

    lpdsb->SetFrequency(wf->nSamplesPerSec);
    return lpdsb;
}

void *SoundManager::DuplicateSource(void *source) {
    if (!m_Root)
        return nullptr;

    LPDIRECTSOUNDBUFFER newSource = nullptr;
    HRESULT hr = m_Root->DuplicateSoundBuffer((LPDIRECTSOUNDBUFFER) source, &newSource);

    if (hr) { // Failed, create a new Buffer
        LPDIRECTSOUNDBUFFER src = (LPDIRECTSOUNDBUFFER) source;

        DWORD sz;
        WAVEFORMATEX wfe;
        src->GetFormat(&wfe, sizeof(WAVEFORMATEX), &sz);

        DSBCAPS caps;
        caps.dwSize = sizeof(DSBCAPS);
        src->GetCaps(&caps);

        DSBUFFERDESC dsbd;
        ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
        dsbd.dwSize = sizeof(DSBUFFERDESC);

        dsbd.dwFlags = caps.dwFlags & ~(DSBCAPS_LOCHARDWARE | DSBCAPS_LOCSOFTWARE | DSBCAPS_LOCDEFER);
        dsbd.dwBufferBytes = caps.dwBufferBytes;
        dsbd.lpwfxFormat = &wfe;

        hr = m_Root->CreateSoundBuffer(&dsbd, &newSource, nullptr);
        if (hr)
            return nullptr;

        LONG val;
        src->GetVolume(&val);
        newSource->SetVolume(val);
        src->GetPan(&val);
        newSource->SetPan(val);
        DWORD freq;
        src->GetFrequency(&freq);
        newSource->SetFrequency(freq);

        // Copy 3d param
        LPDIRECTSOUND3DBUFFER lp3dd = nullptr, lp3ds = nullptr;
        src->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *) &lp3ds);
        newSource->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID *) &lp3dd);

        if (lp3dd && lp3ds) {
            DS3DBUFFER buf;
            lp3ds->GetAllParameters(&buf);
            lp3dd->SetAllParameters(&buf, DS3D_IMMEDIATE);
        }

        if (lp3dd)
            lp3dd->Release();

        if (lp3ds)
            lp3ds->Release();

        // Copy Content
        BYTE *sbuf1, *sbuf2;
        DWORD ssz1, ssz2;
        hr = src->Lock(0, 0, (LPVOID *) &sbuf1, &ssz1, (LPVOID *) &sbuf2, &ssz2, DSBLOCK_ENTIREBUFFER);
        if (hr) {

            BYTE *dbuf1, *dbuf2;
            DWORD dsz1, dsz2;
            hr = newSource->Lock(0, 0, (LPVOID *) &dbuf1, &dsz1, (LPVOID *) &dbuf2, &dsz2, DSBLOCK_ENTIREBUFFER);
            if (hr) {
                if (ssz1)
                    memcpy(sbuf1, dbuf1, ssz1);
                if (ssz2)
                    memcpy(sbuf2, dbuf2, ssz2);
                newSource->Unlock(dbuf1, dsz1, dbuf2, dsz2);
            } else {
                newSource->Release();
                newSource = nullptr;
            }

            newSource->Unlock(sbuf1, ssz1, sbuf2, ssz2);
        } else {
            newSource->Release();
            newSource = nullptr;
        }
    }
    return newSource;
}

void SoundManager::ReleaseSource(void *source) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    m_Source->Stop();
    m_Source->Release();
}

// Control
void SoundManager::Play(CKWaveSound *ws, void *source, CKBOOL loop) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER m_Source;

    if (ws) { // it's a normal sound
        m_Source = (LPDIRECTSOUNDBUFFER) source;
        // We add the sound to the playing list
        m_SoundsPlaying.AddIfNotHere(ws->GetID());
    } else { // it's a minion playing
        SoundMinion *minion = (SoundMinion *) source;
        m_Source = (LPDIRECTSOUNDBUFFER) minion->m_Source;
    }

    InternalPlay(m_Source, loop);
}

void SoundManager::Pause(CKWaveSound *ws, void *source) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    // We remove the sound from the playing list
    // m_SoundsPlaying.erase(ws->GetID());
    InternalPause(m_Source);
}

void SoundManager::SetPlayPosition(void *source, int pos) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    m_Source->SetCurrentPosition(pos);
}

int SoundManager::GetPlayPosition(void *source) {
    if (!source)
        return 0;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    DWORD pos = 0, wpos = 0;
    m_Source->GetCurrentPosition(&pos, &wpos);
    return pos;
}

CKBOOL SoundManager::IsPlaying(void *source) {
    if (!source)
        return FALSE;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    DWORD status = 0;
    m_Source->GetStatus(&status);

    return status & DSBSTATUS_PLAYING; // & A3DSTATUS_PLAYING;
}

// PCM Buffer Information
CKERROR SoundManager::SetWaveFormat(void *source, CKWaveFormat &wf) {
    if (!source)
        return FALSE;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    return m_Source->SetFormat((WAVEFORMATEX *) &wf);
}

CKERROR SoundManager::GetWaveFormat(void *source, CKWaveFormat &wf) {
    if (!source)
        return FALSE;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    return m_Source->GetFormat((WAVEFORMATEX *) &wf, sizeof(CKWaveFormat), nullptr);
}

int SoundManager::GetWaveSize(void *source) {
    if (!source)
        return FALSE;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    DSBCAPS caps;
    ZeroMemory(&caps, sizeof(DSBCAPS));
    caps.dwSize = sizeof(DSBCAPS);
    m_Source->GetCaps(&caps);
    return caps.dwBufferBytes;
}

// Buffer access
CKERROR SoundManager::Lock(void *source, CKDWORD dwWriteCursor, CKDWORD dwNumBytes,
                           void **pvAudioPtr1, CKDWORD *dwAudioBytes1, void **pvAudioPtr2, CKDWORD *dwAudioBytes2,
                           CK_WAVESOUND_LOCKMODE dwFlags) {
    if (!source)
        return 1;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    return m_Source->Lock(dwWriteCursor, dwNumBytes, pvAudioPtr1, (DWORD *) dwAudioBytes1, pvAudioPtr2,
                          (DWORD *) dwAudioBytes2, dwFlags);
}

CKERROR SoundManager::Unlock(void *source, void *pvAudioPtr1, CKDWORD dwNumBytes1,
                             void *pvAudioPtr2, CKDWORD dwAudioBytes2) {
    if (!source)
        return 1;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    return m_Source->Unlock(pvAudioPtr1, dwNumBytes1, pvAudioPtr2, dwAudioBytes2);
}

// 2D/3D Members Functions
void SoundManager::SetType(void *source, CK_WAVESOUND_TYPE type) {
    if (!source)
        return;
    m_Context->OutputToConsole("Warning: DirectX SoundManager doesn't support on fly type changes");
}

CK_WAVESOUND_TYPE SoundManager::GetType(void *source) {
    if (!source)
        return CK_WAVESOUND_TYPE(0);
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    // conversion
    DSBCAPS caps;
    ZeroMemory(&caps, sizeof(DSBCAPS));
    caps.dwSize = sizeof(DSBCAPS);
    m_Source->GetCaps(&caps);

    if (caps.dwFlags & DSBCAPS_CTRL3D)
        return CK_WAVESOUND_POINT;

    return CK_WAVESOUND_BACKGROUND;
}

//----------------------------------------------------------
// 2D/3D Settings
void SoundManager::UpdateSettings(void *source, CK_SOUNDMANAGER_CAPS settingsoptions, CKWaveSoundSettings &settings,
                                  CKBOOL set) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;

    if (set) {
        if (settingsoptions & CK_WAVESOUND_SETTINGS_GAIN)
            m_Source->SetVolume(FloatToDb(settings.m_Gain));

        if (settingsoptions & CK_WAVESOUND_SETTINGS_PITCH) {
            WAVEFORMATEX wf;
            m_Source->GetFormat(&wf, sizeof(WAVEFORMATEX), nullptr);
            m_Source->SetFrequency((LONG) (wf.nSamplesPerSec * settings.m_Pitch));
        }
        if ((settingsoptions & CK_WAVESOUND_SETTINGS_PAN) &&
            (GetType(source) == CK_WAVESOUND_BACKGROUND)) // We set the pan only for 2D sound
            m_Source->SetPan(FloatPanningToDb(settings.m_Pan));
    } else {
        if (settingsoptions & CK_WAVESOUND_SETTINGS_GAIN) {
            long d;
            m_Source->GetVolume(&d);
            settings.m_Gain = DbToFloat(d);
        }
        if (settingsoptions & CK_WAVESOUND_SETTINGS_PITCH) {
            WAVEFORMATEX wf;
            m_Source->GetFormat(&wf, sizeof(WAVEFORMATEX), nullptr);
            DWORD d;
            m_Source->GetFrequency(&d);
            settings.m_Pitch = (float) d / wf.nSamplesPerSec;
        }
        if (settingsoptions & CK_WAVESOUND_SETTINGS_PAN) {
            long d;
            m_Source->GetPan(&d);
            settings.m_Pan = DbPanningToFloat(d);
        }
    }
}

//----------------------------------------------------------
// 3D Settings
void SoundManager::Update3DSettings(void *source, CK_SOUNDMANAGER_CAPS settingsoptions, CKWaveSound3DSettings &settings,
                                    CKBOOL set) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER lpdsb = (LPDIRECTSOUNDBUFFER) source;

    LPDIRECTSOUND3DBUFFER m_Source = nullptr;
    HRESULT err = lpdsb->QueryInterface(IID_IDirectSound3DBuffer, (VOID **) &m_Source);
    if (err)
        return;

    if (set) {
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_CONE) {
            m_Source->SetConeAngles((LONG) settings.m_InAngle, (LONG) settings.m_OutAngle, DS3D_IMMEDIATE);
            m_Source->SetConeOutsideVolume(FloatToDb(settings.m_OutsideGain), DS3D_IMMEDIATE);
        }
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_MINMAXDISTANCE) {
            m_Source->SetMinDistance(settings.m_MinDistance, DS3D_IMMEDIATE);
            m_Source->SetMaxDistance(settings.m_MaxDistance, DS3D_IMMEDIATE);
        }
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_POSITION)
            m_Source->SetPosition(settings.m_Position.x, settings.m_Position.y, settings.m_Position.z, DS3D_IMMEDIATE);
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_VELOCITY)
            m_Source->SetVelocity(settings.m_Velocity.x, settings.m_Velocity.y, settings.m_Velocity.z, DS3D_IMMEDIATE);
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_ORIENTATION)
            m_Source->SetConeOrientation(settings.m_OrientationDir.x, settings.m_OrientationDir.y,
                                         settings.m_OrientationDir.z, DS3D_IMMEDIATE);

        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_HEADRELATIVE) {
            if (settings.m_HeadRelative)
                m_Source->SetMode(DS3DMODE_HEADRELATIVE, DS3D_IMMEDIATE);
            else
                m_Source->SetMode(DS3DMODE_NORMAL, DS3D_IMMEDIATE);
        }
    } else {
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_CONE) {
            DWORD ia, oa;
            m_Source->GetConeAngles(&ia, &oa);
            settings.m_InAngle = (float) ia;
            settings.m_OutAngle = (float) oa;

            long og;
            m_Source->GetConeOutsideVolume(&og);
            settings.m_OutsideGain = DbToFloat(og);
        }
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_MINMAXDISTANCE) {
            m_Source->GetMinDistance(&settings.m_MinDistance);
            m_Source->GetMaxDistance(&settings.m_MaxDistance);
        }
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_HEADRELATIVE) {
            DWORD d;
            m_Source->GetMode(&d);
            settings.m_HeadRelative = (d == DS3DMODE_HEADRELATIVE) ? 1 : 0;
        }
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_POSITION) {
            D3DVECTOR v;
            m_Source->GetPosition(&v);
            settings.m_Position.Set(v.x, v.y, v.z);
        }
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_VELOCITY) {
            D3DVECTOR v;
            m_Source->GetVelocity(&v);
            settings.m_Velocity.Set(v.x, v.y, v.z);
        }
        if (settingsoptions & CK_WAVESOUND_3DSETTINGS_ORIENTATION) {
            D3DVECTOR v;
            m_Source->GetConeOrientation(&v);
            settings.m_OrientationDir.Set(v.x, v.y, v.z);
        }
    }

    m_Source->Release();
}

//----------------------------------------------------------
// 3D Settings
void SoundManager::UpdateListenerSettings(CK_SOUNDMANAGER_CAPS settingsoptions, CKListenerSettings &settings, CKBOOL set) {
    if (!m_Listener)
        return;

    if (set) {
        if (settingsoptions & CK_LISTENERSETTINGS_DISTANCE)
            m_Listener->SetDistanceFactor(settings.m_DistanceFactor, DS3D_IMMEDIATE);
        if (settingsoptions & CK_LISTENERSETTINGS_DOPPLER)
            m_Listener->SetDopplerFactor(settings.m_DopplerFactor, DS3D_IMMEDIATE);
        if (settingsoptions & CK_LISTENERSETTINGS_ROLLOFF)
            m_Listener->SetRolloffFactor(settings.m_RollOff, DS3D_IMMEDIATE);
        if (settingsoptions & CK_LISTENERSETTINGS_GAIN) {
            m_Primary->SetVolume(FloatToDb(settings.m_GlobalGain));
            g_InitialVolumeChanged = TRUE;
        }
    } else {
        if (settingsoptions & CK_LISTENERSETTINGS_DISTANCE)
            m_Listener->GetDistanceFactor(&settings.m_DistanceFactor);
        if (settingsoptions & CK_LISTENERSETTINGS_DOPPLER)
            m_Listener->GetDopplerFactor(&settings.m_DopplerFactor);
        if (settingsoptions & CK_LISTENERSETTINGS_ROLLOFF)
            m_Listener->GetRolloffFactor(&settings.m_RollOff);
        if (settingsoptions & CK_LISTENERSETTINGS_GAIN) {
            long d;
            m_Primary->GetVolume(&d);
            settings.m_GlobalGain = DbToFloat(d);
        }
    }
}

CKERROR SoundManager::OnCKInit() {
    if (m_Context->GetStartOptions() & CK_CONFIG_DISABLEDSOUND)
        return CK_OK;
    // We put all the available features in here (we disable the splash screen (coz it happens we loading the first sound...))
    CKDWORD dwFeatures = 0;

    // Create IDirectSound using the primary sound device
    HRESULT hr = DirectSoundCreate(nullptr, &m_Root, nullptr);

    if (hr) {
        if (m_Context->GetStartOptions() & CK_CONFIG_DOWARN)
            MessageBox(nullptr, "DirectX Sound Engine Initialization Failed", "Warning", MB_OK);
        return hr;
    }

    // Set cooperative level before creating a source
    hr = m_Root->SetCooperativeLevel((HWND) m_Context->GetMainWindow(), DSSCL_PRIORITY);
    if (hr) {
        if (m_Context->GetStartOptions() & CK_CONFIG_DOWARN)
            MessageBox(nullptr, "DirectX Coop level failed", "Warning", MB_OK);
        return hr;
    }

    // Primary Buffer
    DSBUFFERDESC dsbdesc;
    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;

    hr = m_Root->CreateSoundBuffer(&dsbdesc, &m_Primary, nullptr);
    if (hr) {
        if (m_Context->GetStartOptions() & CK_CONFIG_DOWARN)
            MessageBox(nullptr, "DirectX primary buffer failed", "Warning", MB_OK);
        return hr;
    }

    m_Primary->GetVolume(&g_InitialVolume);

    // Listener
    hr = m_Primary->QueryInterface(IID_IDirectSound3DListener, (VOID **) &m_Listener);
    if (hr) {
        if (m_Context->GetStartOptions() & CK_CONFIG_DOWARN)
            MessageBox(nullptr, "DirectX listener failed", "Warning", MB_OK);
        return hr;
    }

    // Set primary buffer format to 22kHz and 16-bit output.
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 22050;
    wfx.wBitsPerSample = 8;
    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    hr = m_Primary->SetFormat(&wfx);
    switch (hr) {
        case DSERR_BADFORMAT:
        case DSERR_INVALIDCALL:
        case DSERR_INVALIDPARAM:
        case DSERR_OUTOFMEMORY:
        case DSERR_PRIOLEVELNEEDED:
        case DSERR_UNSUPPORTED:
            return hr;
        default:
            break;
    }
    if (hr) {
        if (m_Context->GetStartOptions() & CK_CONFIG_DOWARN)
            MessageBox(nullptr, "DirectX set format failed", "Warning", MB_OK);
        return hr;
    }

    RegisterAttribute();

    // We have to restart all the sounds
    int soundsNumber = m_Context->GetObjectsCountByClassID(CKCID_WAVESOUND);
    if (soundsNumber) {
        // we create the ws array
        CK_ID *id = m_Context->GetObjectsListByClassID(CKCID_WAVESOUND);

        for (int i = 0; i < soundsNumber; ++i) {
            CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(id[i]);
            if (ws)
                ws->Recreate();
        }
    }

    /*	if(m_BackSound)
            m_BackSound->Release();
    */
    CKWaveFormat wf;
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 1;
    wf.nSamplesPerSec = 8000;
    wf.wBitsPerSample = 8;
    wf.nBlockAlign = wf.wBitsPerSample * wf.nChannels / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    /*
        m_BackSound = (LPDIRECTSOUNDBUFFER)CreateSource(CK_WAVESOUND_BACKGROUND,&wf,(wf.nAvgBytesPerSec/wf.nBlockAlign)*wf.nBlockAlign);
        if(m_BackSound) {
            m_BackSound->SetVolume(DSBVOLUME_MIN);
            InternalPlay(m_BackSound,TRUE);
        }
    */
    m_Primary->Play(0, 0, DSBPLAY_LOOPING);

    return CK_OK;
}

CKERROR SoundManager::OnCKEnd() {
    if (m_Context->GetStartOptions() & CK_CONFIG_DISABLEDSOUND)
        return CK_OK;
    // We have to release all the sounds
    int soundsNumber = m_Context->GetObjectsCountByClassID(CKCID_WAVESOUND);
    if (soundsNumber) {
        // we create the ws array
        CK_ID *id = m_Context->GetObjectsListByClassID(CKCID_WAVESOUND);

        for (int i = 0; i < soundsNumber; ++i) {
            CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(id[i]);
            if (ws)
                ws->Release();
        }
    }

    /*
        if(m_BackSound) {
            m_BackSound->Release();
            m_BackSound = nullptr;
        }
    */

    // We release the listener
    if (m_Listener) {
        m_Listener->Release();
        m_Listener = nullptr;
    }

    if (m_Primary) {
        m_Primary->Stop();
        m_Primary->Release();
        m_Primary = nullptr;
    }

    // We release the main interface
    if (m_Root) {
        m_Root->Release();
        m_Root = nullptr;
    }

    return CK_OK;
}


CKERROR SoundManager::OnCKReset() {
    // we clear the Dx engine
    if (m_Root && m_Listener) {
        // stop the playing sources
        for (CK_ID *itw = m_SoundsPlaying.Begin(); itw != m_SoundsPlaying.End(); itw++) {
            CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(*itw);
            // We check that the sound is not in 2D
            if (ws && ws->m_Source) {
                ws->InternalStop();
                /*
                m_Source->Stop();  // NICO problem with streamed sound doesn't right call Stop or rewind
                ws->m_Source->Rewind(); // can't call Stop or rewind cause it removes ti from the list and it crashes
                */
            }
        }

        // we clear the sounds of the resume list
        m_SoundsPlaying.Clear();

        // We release all the minions
        ReleaseMinions();

        if (g_InitialVolumeChanged)
            m_Primary->SetVolume(g_InitialVolume);

        // we commit the changes
        m_Listener->CommitDeferredSettings();
    }

    return CK_OK;
}

CKERROR SoundManager::OnCKPause() {
    //	InternalPause(m_BackSound);

    // pause the playing sources
    for (CK_ID *itw = m_SoundsPlaying.Begin(); itw != m_SoundsPlaying.End(); itw++) {
        CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(*itw);
        // We check that the sound is not in 2D
        if (ws)
            ws->Pause();
    }

    // We have to pause all the minions
    PauseMinions();

    return CK_OK;
}

CKERROR SoundManager::OnCKPlay() {
    //	InternalPlay(m_BackSound,TRUE);

    // resume the playing sources
    for (CK_ID *it = m_SoundsPlaying.Begin(); it != m_SoundsPlaying.End(); it++) {
        CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(*it);
        // We check that the sound is not in 2D
        if (ws)
            ws->Resume();
    }

    // We have to resume all the minions
    ResumeMinions();

    return CK_OK;
}

CKERROR SoundManager::PostClearAll() {
    CKSoundManager::PostClearAll();
    m_SoundsPlaying.Clear();

    ReleaseMinions();
    RegisterAttribute();
    return CK_OK;
}

void DxPositionSource(LPDIRECTSOUNDBUFFER psource, CK3dEntity *ent, VxVector &position, VxVector &direction,
                      VxVector &oldpos) {
    LPDIRECTSOUND3DBUFFER source = nullptr;
    HRESULT err = psource->QueryInterface(IID_IDirectSound3DBuffer, (VOID **) &source);
    if (err)
        return;

    // Calculate position
    VxVector pos(position);
    if (ent)
        ent->Transform(&pos, &position);

    // Calculate velocity
    VxVector vel = pos - oldpos;

    // Calculate orientation
    VxVector dir(direction);
    if (ent)
        ent->TransformVector(&dir, &direction);

    // API Call
    // We set the aureal source
    source->SetPosition(pos.x, pos.y, pos.z, DS3D_IMMEDIATE);
    source->SetVelocity(vel.x, vel.y, vel.z, DS3D_IMMEDIATE);
    source->SetConeOrientation(dir.x, dir.y, dir.z, DS3D_IMMEDIATE);

    // We update the old position
    oldpos = pos;

    source->Release();
}

CKERROR SoundManager::PostProcess() {
    // Profiling Start
    float deltaT = m_Context->GetTimeManager()->GetLastDeltaTime();
    if (m_Root && m_Listener) {
        // We clear the sound engine
        CKBOOL somethingIsPlayingIn3D = FALSE;

        // position the sources
        for (CK_ID *it = m_SoundsPlaying.Begin(); it != m_SoundsPlaying.End();) {
            CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(*it);
            // We check that the sound is currently playing
            if (ws && ws->IsPlaying()) {
                // The sound is a file streamed one
                if (ws->GetFileStreaming() && !(ws->GetState() & CK_WAVESOUND_STREAMFULLYLOADED)) {
                    // we fill the buffer, if possible
                    ws->WriteDataFromReader();
                }

                // We update the fade, if it were in fade mode
                ws->UpdateFade();

                // We update the position if it were in 3D
                if (!(ws->GetType() & CK_WAVESOUND_BACKGROUND)) {
                    somethingIsPlayingIn3D = TRUE;
                    ws->UpdatePosition(deltaT);
                }

                it++;
            } else {
                it = m_SoundsPlaying.Remove(it);
            }
        }

        // we check if minions are playing
        for (SoundMinion **itm = m_Minions.Begin(); itm != m_Minions.End(); ++itm) {
            if (IsPlaying((*itm)->m_Source)) {
                somethingIsPlayingIn3D = TRUE;
                // We have to update its position
                if ((*itm)->m_Entity) {
                    // if it is attached to an entity
                    CK3dEntity *ent = (CK3dEntity *) m_Context->GetObject((*itm)->m_Entity);
                    if (ent) { // && (ent->GetMoveableFlags()&VX_MOVEABLE_HASMOVED))
                        // We set the aureal source
                        DxPositionSource((LPDIRECTSOUNDBUFFER) (*itm)->m_Source, ent, (*itm)->m_Position,
                                         (*itm)->m_Direction, (*itm)->m_OldPosition);
                    }
                }
            }
        }

        // If something is playing in 3D, we need to update the listener position and the obstacle source
        // TODO : if a minion is playing, it doesn't work....
        if (somethingIsPlayingIn3D) {
            // temp : put a velocity in objects....
            static VxVector expos;

            // position the listener
            CK3dEntity *ent = GetListener();
            if (ent) { // && ent->GetMoveableFlags()&VX_MOVEABLE_HASMOVED)
                VxVector veloc;
                const VxMatrix &mat = ent->GetWorldMatrix();

                const VxVector4 *pos = &mat[3];
                const VxVector4 *dir = &mat[2];
                const VxVector4 *up = &mat[1];
                veloc = (*pos - expos);
                expos = *pos;
                m_Listener->SetPosition(pos->x, pos->y, pos->z, DS3D_DEFERRED);
                m_Listener->SetVelocity(veloc.x, veloc.y, veloc.z, DS3D_DEFERRED);
                m_Listener->SetOrientation(dir->x, dir->y, dir->z, up->x, up->y, up->z, DS3D_DEFERRED);
            }
        }

        // we commit the changes
        m_Listener->CommitDeferredSettings();

        // Process Minions : finished one are deleted
        ProcessMinions();
    }

    // Profiling End

    return CK_OK;
}

CKERROR SoundManager::PreLaunchScene(CKScene *OldScene, CKScene *NewScene) {
    // Pause the playing sources
    for (CK_ID *itw = m_SoundsPlaying.Begin(); itw != m_SoundsPlaying.End(); itw++) {
        CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(*itw);
        // Pause sounds that are not in the new scene
        if (!ws->IsInScene(NewScene))
            ws->Pause();
    }

    // Stop Minions if their model is not in the New Scene
    for (SoundMinion **it = m_Minions.Begin(); it != m_Minions.End();) {
        CKSceneObject *so = (CKSceneObject *) m_Context->GetObject((*it)->m_OriginalSound);
        if (!so->IsInScene(NewScene)) {
            Stop(nullptr, (*it)->m_Source);
            ReleaseSource((*it)->m_Source);
            delete *it;
            it = m_Minions.Remove(it);
        } else {
            it++;
        }
    }

    return CK_OK;
}

CKERROR SoundManager::SequenceToBeDeleted(CK_ID *objids, int count) {
    CKSoundManager::SequenceToBeDeleted(objids, count);

    // Stop and Remove the sound playing that must be deleted
    for (CK_ID *it = m_SoundsPlaying.Begin(); it != m_SoundsPlaying.End();) {
        CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject(*it);
        if (!ws || ws->IsToBeDeleted()) {
            if (ws)
                ws->Stop();
            it = m_SoundsPlaying.Remove(it);
        } else
            ++it;
    }

    // Remove Deleted Entities from 3D Wave Sound
    {
        for (SoundMinion **it = m_Minions.Begin(); it != m_Minions.End(); it++) {
            CKWaveSound *ws = (CKWaveSound *) m_Context->GetObject((*it)->m_OriginalSound);
            if (ws && ws->IsToBeDeleted())
                (*it)->m_OriginalSound = 0;

            CKObject *o = m_Context->GetObject((*it)->m_Entity);

            if (!o || o->IsToBeDeleted()) {
                (*it)->m_Entity = 0;
            }
        }
    }

    return CK_OK;
}

CKBOOL SoundManager::IsInitialized() {
    return m_Root && m_Primary;
}

SoundManager::SoundManager(CKContext *Context) : CKSoundManager(Context, "DirectX Sound Manager") {
    m_Root = nullptr;
    m_Listener = nullptr;
    m_Primary = nullptr;
    m_Context->RegisterNewManager(this);
}

void SoundManager::InternalPause(void *source) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;
    m_Source->Stop();
}

void SoundManager::InternalPlay(void *source, CKBOOL loop) {
    if (!source)
        return;
    LPDIRECTSOUNDBUFFER m_Source = (LPDIRECTSOUNDBUFFER) source;
    m_Source->Play(0, 0, loop ? DSBPLAY_LOOPING : 0);
}