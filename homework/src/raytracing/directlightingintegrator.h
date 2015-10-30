#pragma once
#include "integrator.h"

class DirectLightingIntegrator:public Integrator
{
public:
    DirectLightingIntegrator();
    virtual glm::vec3 TraceRay(Ray r, unsigned int depth);

    int sample_num;

protected:
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;
};
