//
// Created by mkowa on 02.04.2024.
//

#include "perlin.h"

float perlin::PerlinGenerator::interpolate(float a0, float a1, float w) {
    if (0.0 > w) return a0;
    if (1.0 < w) return a1;
    return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

perlin::vector2 perlin::PerlinGenerator::randomGradient(int ix, int iy) {
    m_generator.seed(m_seed + ix ^ 2 * iy + 2 * iy ^ 2 * ix);
    vector2 v(m_dist(m_generator),
    sqrt(1 - (v.x * v.x)));
    if (m_uniformDistribution(m_generator) != 0) v.y = -v.y;
    return v;
}

double perlin::PerlinGenerator::dotGridGradient(int ix, int iy, float x, float y) {
    vector2 gradient = randomGradient(ix, iy);
    float dx = x - (float) ix;
    float dy = y - (float) iy;
    return (dx * gradient.x + dy * gradient.y);
}
[[nodiscard]] double perlin::PerlinGenerator::perlin(double x, double y) {      int x0 = (int) floor(x);
    int x1 = x0 + 1;
    int y0 = (int) floor(y);
    int y1 = y0 + 1;

    float sx = x - (float) x0;
    float sy = y - (float) y0;

    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value * 0.5 + 0.5; }
std::vector<double> perlin::generateRandomAngles() {
    std::vector<double> permutation;
    std::mt19937 eng(GetRandomValue(1,1000));
    std::uniform_real_distribution<double> dist(0.f,2.0*M_PI);
    for(int i = 0; i < 256; i++) {

        permutation.push_back(dist(eng));
    }
    return permutation;
}
template<typename T>
inline T perlin::fastcos(T x) noexcept
{
    constexpr T tp = 1./(2.*M_PI);
    x *= tp;
    x -= T(.25) + std::floor(x + T(.25));
    x *= T(16.) * (std::abs(x) - T(.5));
#if EXTRA_PRECISION
    x += T(.225) * x * (std::abs(x) - T(1.));
#endif
    return x;
}
perlin::vector2 perlin::getVectorFromAngle(double v) {
    return vector2(fastcos(v), fastcos(v - (M_PI / 2.f)));
}

double perlin::fade(double t) {
    return ((6*t - 15)*t + 10)*t*t*t;
}

double perlin::lerp(double t, double a1, double a2) {
    return a1 + t*(a2-a1);
}

double perlin::PerlinGenerator::getNoise2D(double x, double y, std::vector<double> Permutation) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    double xf = x - floor(x);
    double yf = y - floor(y);

    vector2 topRight = vector2(xf - 1.0, yf - 1.0);
    vector2 topLeft = vector2(xf, yf - 1.0);
    vector2 bottomRight = vector2(xf - 1.0, yf);
    vector2 bottomLeft = vector2(xf, yf);

    // Select a value from the permutation array for each of the 4 corners
    int valueTopRight = Permutation[Permutation[X + 1] + Y + 1];
    int valueTopLeft = Permutation[Permutation[X] + Y + 1];
    int valueBottomRight = Permutation[Permutation[X + 1] + Y];
    int valueBottomLeft = Permutation[Permutation[X] + Y];

    double dotTopRight = topRight.dot(getVectorFromAngle(valueTopRight));
    double dotTopLeft = topLeft.dot(getVectorFromAngle(valueTopLeft));
    double dotBottomRight = bottomRight.dot(getVectorFromAngle(valueBottomRight));
    double dotBottomLeft = bottomLeft.dot(getVectorFromAngle(valueBottomLeft));

    double u = fade(xf);
    double v = fade(yf);

    return lerp(u,
                lerp(v, dotBottomLeft, dotTopLeft),
                lerp(v, dotBottomRight, dotTopRight)
    );
}

double perlin::PerlinGenerator::getMultiOctaveNoise2D(int x, int y, int numOctaves, const std::vector<double>& Permutation) {
    double result = 0.0;
    double amplitude = 1.0;
    double frequency = 0.005;

    for (int octave = 0; octave < numOctaves; octave++) {
        double n = amplitude * getNoise2D(x * frequency, y * frequency, Permutation);
        result += n;

        amplitude *= 0.5;
        frequency *= 2.0;
    }
    if(result > 1.0) result = 1.0;
    if(result < -1.0) result = -1.0;
    result =  ((result + 1)/(2));
    return result;
}