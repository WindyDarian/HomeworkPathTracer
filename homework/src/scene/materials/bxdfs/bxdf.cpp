#include <scene/materials/bxdfs/bxdf.h>
#include <raytracing/sampling.h>

glm::vec3 BxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float &pdf_ret) const
{
        // cosine weighted hemisphere sampling
        wi_ret = sampling::cosine_hemisphere();
        //pdf_ret = PDF(wo, wi_ret);

        return EvaluateScatteredEnergy(wo, wi_ret, pdf_ret);
}

glm::vec3 BxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2* samples) const
{
    //TODO
    return glm::vec3(0);
}

float BxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    if (wo.z * wi.z > 0)
        return wi.z * INV_PI;//costheta
    else return 0.f;
}
