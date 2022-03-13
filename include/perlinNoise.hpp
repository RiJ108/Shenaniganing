#pragma once PERLINNOISE_HPP

#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class PerlinNoise{
public:
    PerlinNoise();
    PerlinNoise(unsigned int seed);
    double noise(double x, double y, double z);
    double noise(vec3 position);
protected:
private:
    vector<int> p;
    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);
};