#ifndef PLAYER_INPUTMANAGER_H
#define PLAYER_INPUTMANAGER_H

#include "CKInputManager.h"

#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>

#define KEYBOARD_BUFFER_SIZE 256
#define MOUSE_BUFFER_SIZE 256

#define DX8_INPUTMANAGER_GUID CKGUID(0xF787C904, 0)

void CKInitializeParameterTypes(CKContext *context);
void CKInitializeOperationTypes(CKContext *context);
void CKInitializeOperationFunctions(CKContext *context);
void CKUnInitializeParameterTypes(CKContext *context);
void CKUnInitializeOperationTypes(CKContext *context);

typedef enum CK_INPUT_DEVICE {
    CK_INPUT_DEVICE_NONE = 0,
    CK_INPUT_DEVICE_KEYBOARD = 1,
    CK_INPUT_DEVICE_MOUSE = 2,
    CK_INPUT_DEVICE_JOYSTICK = 4,
    CK_INPUT_DEVICE_ALL = CK_INPUT_DEVICE_KEYBOARD | CK_INPUT_DEVICE_MOUSE | CK_INPUT_DEVICE_JOYSTICK,
} CK_INPUT_DEVICE;

class InputManager : public CKInputManager {
public:
    class CKMouse {
        friend InputManager;
    public:
        CKMouse() = default;

        void Init(HWND hWnd);
        void Release();

        void Clear();

        void Poll(CKBOOL pause);

    private:
        LPDIRECTINPUTDEVICE8 m_Device = nullptr;
        Vx2DVector m_Position;
        DIMOUSESTATE m_State = {};
        CKBYTE m_LastButtons[4] = {};
        DIDEVICEOBJECTDATA m_Buffer[MOUSE_BUFFER_SIZE] = {};
        int m_NumberOfBuffer = 0;
    };

    class CKJoystick {
        friend InputManager;
    public:
        CKJoystick() = default;

        void Init(HWND hWnd);
        void Release();

        void Poll();

        void GetInfo();

        CKBOOL IsAttached();

    private:
        LPDIRECTINPUTDEVICE2 m_Device = nullptr;
        CKDWORD m_JoyID = -1;
        CKBOOL m_Polled = FALSE;
        VxVector m_Position;
        VxVector m_Rotation;
        Vx2DVector m_Sliders;
        CKDWORD m_PointOfViewAngle = 0;
        CKDWORD m_Buttons = 0;
        CKDWORD m_Xmin = 0; // Minimum X-coordinate
        CKDWORD m_Xmax = 0; // Maximum X-coordinate
        CKDWORD m_Ymin = 0; // Minimum Y-coordinate
        CKDWORD m_Ymax = 0; // Maximum Y-coordinate
        CKDWORD m_Zmin = 0; // Minimum Z-coordinate
        CKDWORD m_Zmax = 0; // Maximum Z-coordinate
        CKDWORD m_XRmin = 0; // Minimum X-rotation
        CKDWORD m_XRmax = 0; // Maximum X-rotation
        CKDWORD m_YRmin = 0; // Minimum Y-rotation
        CKDWORD m_YRmax = 0; // Maximum Y-rotation
        CKDWORD m_ZRmin = 0; // Minimum Z-rotation
        CKDWORD m_ZRmax = 0; // Maximum Z-rotation
        CKDWORD m_Umin = 0; // Minimum u-coordinate (fifth axis)
        CKDWORD m_Vmin = 0; // Minimum v-coordinate (sixth axis)
        CKDWORD m_Umax = 0; // Maximum u-coordinate (fifth axis)
        CKDWORD m_Vmax = 0; // Maximum v-coordinate (sixth axis)
    };

    void EnableKeyboardRepetition(CKBOOL iEnable = TRUE) override;
    CKBOOL IsKeyboardRepetitionEnabled() override;

    CKBOOL IsKeyDown(CKDWORD iKey, CKDWORD *oStamp = NULL) override;
    CKBOOL IsKeyUp(CKDWORD iKey) override;
    CKBOOL IsKeyToggled(CKDWORD iKey, CKDWORD *oStamp = NULL) override;

    int GetKeyName(CKDWORD iKey, CKSTRING oKeyName) override;
    CKDWORD GetKeyFromName(CKSTRING iKeyName) override;
    unsigned char *GetKeyboardState() override;

    CKBOOL IsKeyboardAttached() override;

    int GetNumberOfKeyInBuffer() override;
    int GetKeyFromBuffer(int i, CKDWORD &oKey, CKDWORD *oTimeStamp = NULL) override;

    CKBOOL IsMouseButtonDown(CK_MOUSEBUTTON iButton) override;
    CKBOOL IsMouseClicked(CK_MOUSEBUTTON iButton) override;
    CKBOOL IsMouseToggled(CK_MOUSEBUTTON iButton) override;

    void GetMouseButtonsState(CKBYTE oStates[4]) override;
    void GetMousePosition(Vx2DVector &oPosition, CKBOOL iAbsolute = TRUE) override;
    void GetMouseRelativePosition(VxVector &oPosition) override;

    CKBOOL IsMouseAttached() override;

    CKBOOL IsJoystickAttached(int iJoystick) override;
    void GetJoystickPosition(int iJoystick, VxVector *oPosition) override;
    void GetJoystickRotation(int iJoystick, VxVector *oRotation) override;
    void GetJoystickSliders(int iJoystick, Vx2DVector *oPosition) override;
    void GetJoystickPointOfViewAngle(int iJoystick, float *oAngle) override;
    CKDWORD GetJoystickButtonsState(int iJoystick) override;
    CKBOOL IsJoystickButtonDown(int iJoystick, int iButton) override;

    void Pause(CKBOOL pause) override;

    void ShowCursor(CKBOOL iShow) override;
    CKBOOL GetCursorVisibility() override;

    VXCURSOR_POINTER GetSystemCursor() override;
    void SetSystemCursor(VXCURSOR_POINTER cursor) override;

    virtual int GetJoystickCount();
    virtual IDirectInputDevice2 *GetJoystickDxInterface(int iJoystick);

    virtual CKBOOL IsDeviceEnabled(CK_INPUT_DEVICE device);
    virtual void EnableDevice(CK_INPUT_DEVICE device);
    virtual void DisableDevice(CK_INPUT_DEVICE device);

    // Internal functions

    CKERROR OnCKInit() override;
    CKERROR OnCKEnd() override;
    CKERROR OnCKReset() override;
    CKERROR OnCKPause() override;
    CKERROR OnCKPlay() override;
    CKERROR PreProcess() override;
    CKERROR PostProcess() override;

    CKDWORD GetValidFunctionsMask() override {
        return CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKReset |
               CKMANAGER_FUNC_OnCKPause |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_PreProcess |
               CKMANAGER_FUNC_PostProcess;
    }

    ~InputManager() override;

    explicit InputManager(CKContext *context);

    void Initialize(HWND hWnd);
    void Uninitialize();

    void ClearBuffers(CK_INPUT_DEVICE device = CK_INPUT_DEVICE_ALL);

    static BOOL CALLBACK JoystickEnum(const DIDEVICEINSTANCE *pdidInstance, void *pContext);

protected:
    LPDIRECTINPUT8 m_DirectInput = nullptr;
    LPDIRECTINPUTDEVICE8 m_Keyboard = nullptr;
    VXCURSOR_POINTER m_Cursor = VXCURSOR_NORMALSELECT;
    CKMouse m_Mouse;
    CKJoystick m_Joysticks[4];
    int m_JoystickCount = 0;
    CKBYTE m_KeyboardState[KEYBOARD_BUFFER_SIZE] = {};
    int m_KeyboardStamps[KEYBOARD_BUFFER_SIZE] = {};
    DIDEVICEOBJECTDATA m_KeyInBuffer[KEYBOARD_BUFFER_SIZE] = {};
    int m_NumberOfKeyInBuffer;
    CKBOOL m_EnabledDevice;
    CKBOOL m_EnableKeyboardRepetition;
    CKDWORD m_KeyboardRepeatDelay;
    CKDWORD m_KeyboardRepeatInterval;
    CKBOOL m_ShowCursor;

private:
    void EnsureCursorVisible(CKBOOL iShow);
};

#endif // PLAYER_INPUTMANAGER_H
