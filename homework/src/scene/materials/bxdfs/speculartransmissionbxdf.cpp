#include "speculartransmissionbxdf.h"



glm::vec3 SpecularTransmissionBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi, float &pdf_ret) const
{
    pdf_ret = 0.f;
    return glm::vec3(0.f);
}

glm::vec3 SpecularTransmissionBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    // nope
}

glm::vec3 SpecularTransmissionBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float &pdf_ret) const
{
    bool is_entering = wo.z > 0.f;  //cos(wo,N)

    float ei,et;

    if (is_entering)
    {
        ei = etai;
        et = etat;
    }
    else
    {
        ei = etat;
        et = etai;
    }

    // Compute transmitted ray direction
    float sini2 = 1.f - wo.z * wo.z;  //sin2(wo,N)
    float eta = ei / et;
    float sint2 = eta * eta * sini2;

    // Total internal reflection
    if (sint2 >= 1.f) return glm::vec3(0.f);
    float cost = glm::sqrt(glm::max(0.f, 1.f - sint2));
    if (is_entering) cost = -cost;
    float sintOverSini = eta;
    wi_ret = glm::vec3(sintOverSini * -wo.x, sintOverSini * -wo.y, cost);
    pdf_ret = 1.f;
    glm::vec3 F(wo.z);
    return (ei*ei)/(et*et) * (glm::vec3(1.)-F) * this->transmission_color /
            glm::abs(wi_ret.z);
}

float SpecularTransmissionBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return 0.f;
}
