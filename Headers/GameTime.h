//
// Created by berke on 4/5/2026.
//

#ifndef TILKYENGINE_GAMETIME_H
#define TILKYENGINE_GAMETIME_H

#include <SDL3/SDL.h>

namespace GameTime {
    inline static float deltaTime = 0.0f;
    inline static float time = 0.0f;

    inline static float smoothedFPS = 0.0f;

    static void Update() {
        Uint64 now = SDL_GetPerformanceCounter();
        static Uint64 last = now;

        Uint64 diff = now - last;
        last = now;

        deltaTime = (float)(diff / (double)SDL_GetPerformanceFrequency());
        time += deltaTime;
        float instantFPS = deltaTime > .0f ? 1.0f / deltaTime : 0.0f;
        smoothedFPS = smoothedFPS * .9f + instantFPS * .1f;
    }

    static float GetFPS(){
        return smoothedFPS;
    }
}

#endif //TILKYENGINE_GAMETIME_H