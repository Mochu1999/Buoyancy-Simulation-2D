#pragma once
#include "Common.cpp"

std::vector<p> generateRandomPoints(int n) {

    float minX = 400.0;
    float maxX = 1200.0;
    float minY = 300.0;
    float maxY = 700.0;


    // Seed for random number generation
    std::random_device rd;
    std::mt19937 gen(rd());

    // Distributions for x and y coordinates
    std::uniform_real_distribution<> disX(minX, maxX);
    std::uniform_real_distribution<> disY(minY, maxY);

    std::vector<p> points;
    points.reserve(n);

    for (int i = 0; i < n; ++i) {
        float x = disX(gen);
        float y = disY(gen);
        points.push_back({ x, y });
    }

    return points;
}
