#pragma once
#include <SDL3/SDL.h>

class InputManager {
public:
    static void BeginFrame();
    static bool GetKeyDown(SDL_Scancode key);
    static bool GetKey(SDL_Scancode key);
    static bool GetKeyUp(SDL_Scancode key);

private:
    static const bool *keyboardState;
    static Uint8 prevKeyboardState[SDL_SCANCODE_COUNT];
};
