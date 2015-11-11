#pragma once
#include <scene/materials/bxdfs/bxdf.h>


class CartoonBxDF : public BxDF
{
public:
    // A cartoon BxDF!
    CartoonBxDF(const glm::vec3 &color, int levels, float threshold) : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),
        diffuse_color(color),
        levels(levels),
        threshold(glm::clamp(threshold,0.f,0.999f))
    {}
    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const;
    glm::vec3 diffuse_color;
    // divide color in to several stages
    int levels = 3;
    // min dot(wi, normal) for light
    float threshold = 0.2;

};
