#pragma once
#include <scene/materials/bxdfs/bxdf.h>

class SpecularTransmissionBxDF: public BxDF
{
public:
//Constructors/Destructors
    SpecularTransmissionBxDF(const glm::vec3 &color, float etai, float etat) :
        BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)),
        transmission_color(color),
        etai(etai),
        etat(etat)
    {}
//Functions
    virtual glm::vec3 EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi, float & pdf_ret) const;
    virtual glm::vec3 EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const;
    virtual glm::vec3 SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float &pdf_ret) const;

    virtual float PDF(const glm::vec3 &wo, const glm::vec3 &wi) const;

//Member variables
    glm::vec3 transmission_color;
    float etai = 1.33f;
    float etat = 1.0f;
};
