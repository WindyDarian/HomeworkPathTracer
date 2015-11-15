#include <scene/materials/bxdfs/specularreflectionBxDF.h>

glm::vec3 SpecularReflectionBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi, float &pdf_ret) const
{
    pdf_ret = 0.f;
    // chance is too small, directly calculated in SampleAndEvaluateScatteredEnergy
    return glm::vec3(0.f);
//    if (fequal(wo.z, wi.z) && fequal(wo.x, -wi.x) && fequal(wo.y, -wi.y))
//        return glm::vec3(1.f);
//    else return glm::vec3(0.f);
}

glm::vec3 SpecularReflectionBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO(fake)
    return glm::vec3(0);
}

glm::vec3 SpecularReflectionBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float &pdf_ret) const
{
    wi_ret = glm::vec3(-wo.x, -wo.y, wo.z);
    pdf_ret = 1.f ;
    return reflection_color;
}



float SpecularReflectionBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    if (fequal(wo.z, wi.z) && fequal(wo.x, -wi.x) && fequal(wo.y, -wi.y))
        return 1.f;
    else return 0.f;
}
