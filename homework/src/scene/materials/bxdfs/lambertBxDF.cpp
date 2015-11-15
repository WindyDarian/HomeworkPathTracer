#include <scene/materials/bxdfs/lambertBxDF.h>
#include <raytracing/sampling.h>

glm::vec3 LambertBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi, float& pdf_ret) const
{
    pdf_ret = this->PDF(wo,wi);
    return this->diffuse_color * INV_PI ;
}

glm::vec3 LambertBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float &pdf_ret) const
{
    return BxDF::SampleAndEvaluateScatteredEnergy(wo, wi_ret, pdf_ret);
}

glm::vec3 LambertBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}

float LambertBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    return BxDF::PDF(wo,wi);
}
