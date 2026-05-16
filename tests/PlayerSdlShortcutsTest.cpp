#include "PlayerSdlShortcuts.h"

#include <SDL3/SDL_scancode.h>

#include <array>
#include <cstdio>

namespace
{
bool ExpectEqual(player_sdl_shortcuts::KeyboardShortcutAction actual,
                 player_sdl_shortcuts::KeyboardShortcutAction expected,
                 const char *message)
{
    if (actual == expected)
        return true;

    std::fprintf(stderr, "%s: expected %d, got %d\n", message, expected, actual);
    return false;
}
}

int main()
{
    using namespace player_sdl_shortcuts;

    std::array<bool, SDL_SCANCODE_COUNT> keys = {};
    KeyboardShortcutState state = {};

    bool ok = true;
    keys[SDL_SCANCODE_RETURN] = 1;
    ok &= ExpectEqual(UpdateKeyboardShortcuts(keys.data(), SDL_KMOD_NONE, state),
                      KeyboardShortcutNone,
                      "Return without Alt must not trigger a player shortcut");

    ok &= ExpectEqual(UpdateKeyboardShortcuts(keys.data(), SDL_KMOD_LALT, state),
                      KeyboardShortcutToggleFullscreen,
                      "Alt+Return should toggle fullscreen on the first frame");
    ok &= ExpectEqual(UpdateKeyboardShortcuts(keys.data(), SDL_KMOD_LALT, state),
                      KeyboardShortcutNone,
                      "Held Alt+Return should not toggle fullscreen repeatedly");

    keys[SDL_SCANCODE_RETURN] = 0;
    ok &= ExpectEqual(UpdateKeyboardShortcuts(keys.data(), SDL_KMOD_NONE, state),
                      KeyboardShortcutNone,
                      "Releasing Alt+Return should only reset shortcut state");

    keys[SDL_SCANCODE_KP_ENTER] = 1;
    ok &= ExpectEqual(UpdateKeyboardShortcuts(keys.data(), SDL_KMOD_RALT, state),
                      KeyboardShortcutToggleFullscreen,
                      "Alt+keypad Enter should toggle fullscreen");

    keys[SDL_SCANCODE_KP_ENTER] = 0;
    keys[SDL_SCANCODE_F4] = 1;
    ok &= ExpectEqual(UpdateKeyboardShortcuts(keys.data(), SDL_KMOD_LALT, state),
                      KeyboardShortcutQuit,
                      "Alt+F4 should request quit");

    return ok ? 0 : 1;
}
