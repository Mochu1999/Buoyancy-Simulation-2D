#pragma once

#include "Common.cpp"


float deltaTime = 0.0f;

struct Time {
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    std::chrono::high_resolution_clock::time_point startElapsedTime;

    
    float fps = 0.0f;
    float frameCount = 0.0f;
    float timeAccumulator = 0.0f;

    Time() {
        lastFrameTime = std::chrono::high_resolution_clock::now();
        startElapsedTime = lastFrameTime;
    }

    void update() {
        auto currentFrameTime = std::chrono::high_resolution_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;

        frameCount++;
        timeAccumulator += deltaTime;

        if (timeAccumulator >= 1.0f) {
            fps = frameCount / timeAccumulator;
            frameCount = 0;
            timeAccumulator -= 1.0f;
        }
    }

    float getDeltaTime() const {
        return deltaTime;
    }

    float getFPS() const {
        return fps;
    }
};
