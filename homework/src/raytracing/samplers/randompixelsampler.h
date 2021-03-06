#pragma once
#include <raytracing/samplers/pixelsampler.h>
#include <random>

class RandomPixelSampler:public PixelSampler
{
public:
    RandomPixelSampler();
    RandomPixelSampler(unsigned int samples);
    RandomPixelSampler(unsigned int samples, unsigned int seed);
    virtual QList<glm::vec2> GetSamples(int x, int y);
    virtual void initialize(int width, int height) {}

protected:
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;
};

