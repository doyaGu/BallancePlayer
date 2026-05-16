#include "PlayerSdlShortcuts.h"

namespace player_sdl_shortcuts
{
KeyboardShortcutAction UpdateKeyboardShortcuts(const bool *keyboardState,
                                               SDL_Keymod modifiers,
                                               KeyboardShortcutState &state)
{
    const bool altDown = (modifiers & SDL_KMOD_ALT) != 0;
    const bool enterDown = keyboardState &&
                           (keyboardState[SDL_SCANCODE_RETURN] ||
                            keyboardState[SDL_SCANCODE_KP_ENTER]);
    const bool f4Down = keyboardState && keyboardState[SDL_SCANCODE_F4];

    const bool altEnterDown = altDown && enterDown;
    const bool altF4Down = altDown && f4Down;

    KeyboardShortcutAction action = KeyboardShortcutNone;
    if (altF4Down && !state.altF4Down)
        action = KeyboardShortcutQuit;
    else if (altEnterDown && !state.altEnterDown)
        action = KeyboardShortcutToggleFullscreen;

    state.altEnterDown = altEnterDown;
    state.altF4Down = altF4Down;
    return action;
}
}
