#pragma once

#include <la.h>
#include "integrator.h"
#include <raytracing/ray.h>
#include <list>

class MISIntegrator:public Integrator
{
public:
    MISIntegrator();
    virtual glm::vec3 TraceRay(Ray r, unsigned int depth);

    //int sample_num;
    int min_depth = 3;

protected:
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;

private:
    float generateRandom();
    std::list<std::pair<Ray, glm::vec3>> tracing_list;
};
