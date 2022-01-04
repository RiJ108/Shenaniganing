#pragma once PERLINNOISE_HPP

#include <vector>
#include <numeric>
#include <random>
#include <algorithm>

using namespace std;

class PerlinNoise{
public:
    PerlinNoise();
    PerlinNoise(unsigned int seed);
    double noise(double x, double y, double z);
protected:
private:
    vector<int> p;
    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);
};