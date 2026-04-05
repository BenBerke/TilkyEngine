#include "../Headers/InputManager.h"
#include <cstring>

const bool *InputManager::keyboardState = nullptr;
Uint8 InputManager::prevKeyboardState[SDL_SCANCODE_COUNT] = {0};

void InputManager::BeginFrame()
{
    if (!keyboardState)
    {
        SDL_PumpEvents();
        keyboardState = SDL_GetKeyboardState(nullptr);
    }

    std::memcpy(prevKeyboardState, keyboardState, SDL_SCANCODE_COUNT * sizeof(Uint8));

    SDL_PumpEvents();
    keyboardState = SDL_GetKeyboardState(nullptr);
}

bool InputManager::GetKeyDown(SDL_Scancode key)
{
    return keyboardState[key] && !prevKeyboardState[key];
}

bool InputManager::GetKey(SDL_Scancode key)
{
    return keyboardState[key];
}

bool InputManager::GetKeyUp(SDL_Scancode key)
{
    return !keyboardState[key] && prevKeyboardState[key];
}
