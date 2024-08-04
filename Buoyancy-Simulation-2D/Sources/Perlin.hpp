#pragma once

#include "Common.hpp"

//Perlin noise is a composition of similar stacked layers called octaves.
//First we create a random unit vector at each interection of the grid

//Takes input coordinates of a grid and creates a pseudo-random gradient vector in that position.
//This vector will be use to influence the noise value at points in the grid, creating smooth transitions across the terrain
//p2 randomGradient(p2 point) {
//    const unsigned w = 8 * sizeof(unsigned);
//    const unsigned s = w / 2;
//    unsigned a = int(point.x), b = int(point.y);
//    a *= 3284157443;
//
//    b ^= a << s | a >> w - s;
//    b *= 1911520717;
//
//    a ^= b << s | b >> w - s;
//    a *= 2048419325;
//    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
//
//    // Create the vector from the angle
//    return { sin(random),cos(random) };
//}
//
//// Computes the dot product of the distance and gradient vectors.
//float dotGridGradient(p2 gridPoint, p2 point) {
//    p2 gradient = randomGradient(gridPoint);
//
//    p2 d = point - gridPoint;
//
//    return dot2(d,gradient);
//}

typedef struct {
    float x, y;
} vector2;

vector2 randomGradient(int ix, int iy, unsigned seed) {
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix + seed, b = iy + seed;
    a *= 3284157443;

    b ^= a << s | a >> (w - s);
    b *= 1911520717;

    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    vector2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

float dotGridGradient(int ix, int iy, float x, float y, unsigned seed) {
    vector2 gradient = randomGradient(ix, iy, seed);

    float dx = x - (float)ix;
    float dy = y - (float)iy;

    return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float perlin(float x, float y, unsigned seed) {
    int x0 = (int)x;
    int y0 = (int)y;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = x - (float)x0;
    float sy = y - (float)y0;

    float n0 = dotGridGradient(x0, y0, x, y, seed);
    float n1 = dotGridGradient(x1, y0, x, y, seed);
    float ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y, seed);
    n1 = dotGridGradient(x1, y1, x, y, seed);
    float ix1 = interpolate(n0, n1, sx);

    float value = interpolate(ix0, ix1, sy);

    return value;
}