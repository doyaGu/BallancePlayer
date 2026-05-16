#ifndef PLAYER_SDL_SHORTCUTS_H
#define PLAYER_SDL_SHORTCUTS_H

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_scancode.h>

namespace player_sdl_shortcuts
{
enum KeyboardShortcutAction
{
    KeyboardShortcutNone = 0,
    KeyboardShortcutToggleFullscreen,
    KeyboardShortcutQuit,
};

struct KeyboardShortcutState
{
    bool altEnterDown;
    bool altF4Down;
};

KeyboardShortcutAction UpdateKeyboardShortcuts(const bool *keyboardState,
                                               SDL_Keymod modifiers,
                                               KeyboardShortcutState &state);
}

#endif /* PLAYER_SDL_SHORTCUTS_H */
