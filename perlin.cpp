
#include "perlin.h"
template<typename R>
std::vector<R> perlin::generateRandomAngles() {
    std::vector<R> permutation(256);
    std::mt19937 eng(GetRandomValue(1,1000));
    std::uniform_real_distribution<R> dist(0.f,2.0*M_PI);
    std::generate(permutation.begin(),permutation.end(), [&dist,&eng] () {return dist(eng);});
    return permutation;
}
template<typename T>
inline T perlin::fastcos(T x) noexcept
{
    constexpr T tp = 1./(2.*M_PI);
    x *= tp;
    x -= T(.25) + std::floor(x + T(.25));
    x *= T(16.) * (std::abs(x) - T(.5));
    return x;
}
template<typename T>
perlin::vector2<T> perlin::getVectorFromAngle(T v) {
    return vector2<T>(fastcos(v), fastcos(v - (M_PI / 2.f)));
}
template<typename T>
T perlin::fade(T t) {
    return ((6*t - 15)*t + 10)*t*t*t;
}
template<typename T>
T perlin::lerp(T t, T a1, T a2) {
    return a1 + t*(a2-a1);
}
template<typename T>
T perlin::PerlinGenerator<T>::getNoise2D(T x, T y, std::vector<T> Permutation) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    double xf = x - floor(x);
    double yf = y - floor(y);

    vector2<T> topRight = vector2(xf - 1.0, yf - 1.0);
    vector2<T> topLeft = vector2(xf, yf - 1.0);
    vector2<T> bottomRight = vector2(xf - 1.0, yf);
    vector2<T> bottomLeft = vector2(xf, yf);

    // Select a value from the permutation array for each of the 4 corners
    int valueTopRight = Permutation[Permutation[X + 1] + Y + 1];
    int valueTopLeft = Permutation[Permutation[X] + Y + 1];
    int valueBottomRight = Permutation[Permutation[X + 1] + Y];
    int valueBottomLeft = Permutation[Permutation[X] + Y];

    T dotTopRight = topRight.dot(getVectorFromAngle<T>(valueTopRight));
    T dotTopLeft = topLeft.dot(getVectorFromAngle<T>(valueTopLeft));
    T dotBottomRight = bottomRight.dot(getVectorFromAngle<T>(valueBottomRight));
    T dotBottomLeft = bottomLeft.dot(getVectorFromAngle<T>(valueBottomLeft));

    T u = fade(xf);
    T v = fade(yf);

    return lerp(u,
                lerp(v, dotBottomLeft, dotTopLeft),
                lerp(v, dotBottomRight, dotTopRight)
    );
}
template <typename T>
T perlin::PerlinGenerator<T>::getMultiOctaveNoise2D(int x, int y, int numOctaves, const std::vector<T>& Permutation) {
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