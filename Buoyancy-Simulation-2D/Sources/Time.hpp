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


//high_resolution_clock::time_point currentTime;
//high_resolution_clock::time_point lastTime;
//float elapsedTime;
//for (size_t i = 0; i < static_cast<size_t>(1); i++)
//{
//	wettedSurface.calculatePositions();
//}
//currentTime = high_resolution_clock::now();
//for (size_t i = 0; i < static_cast<size_t>(1); i++)
//{
//	wettedSurface.createWettedPositions();
//}
//lastTime = high_resolution_clock::now();
//elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
//cout << "old format, time: " << elapsedTime << endl;
//currentTime = high_resolution_clock::now();
//for (size_t i = 0; i < static_cast<size_t>(1); i++)
//{
//	wettedSurface.calculatePositions();
//}
//lastTime = high_resolution_clock::now();
//elapsedTime = duration_cast<duration<float>>(lastTime - currentTime).count();
//cout << "new format, time: " << elapsedTime << endl;