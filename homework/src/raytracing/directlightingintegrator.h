#pragma once
#include "integrator.h"
#include <random>

class DirectLightingIntegrator:public Integrator
{
public:
    DirectLightingIntegrator();
    virtual glm::vec3 TraceRay(Ray r, unsigned int depth);

    int sample_num = 2;

protected:
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;

private:
    float generateRandom();
};
