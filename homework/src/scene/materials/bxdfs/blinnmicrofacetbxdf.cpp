#include <scene/materials/bxdfs/blinnmicrofacetbxdf.h>
#include <raytracing/sampling.h>

glm::vec3 BlinnMicrofacetBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi, float &pdf_ret) const
{
    glm::vec3 R =  reflection_color;
    float cosThetaO = glm::abs(glm::dot(wo, glm::vec3(0,0,1)));
    float cosThetaI = glm::abs(glm::dot(wi, glm::vec3(0,0,1)));
    if (cosThetaI == 0.f || cosThetaO == 0.f) return glm::vec3(0.f);
    glm::vec3 wh = wi + wo;
    if (wh.x == 0. && wh.y == 0. && wh.z == 0.) return glm::vec3(0.f);
    wh = glm::normalize(wh);
    float wi_dot_wh = glm::dot(wi, wh);
    glm::vec3 F = wi_dot_wh * glm::vec3(1.f);

    float cosThetaH = glm::abs(glm::dot(wh, glm::vec3(0,0,1)));
    float D = (exponent+2) * INV_PI * 0.5f * powf(cosThetaH, exponent);


    float wo_dot_wh = glm::dot(wo, wh);
    float G = glm::min(1.f, glm::min((2.f * cosThetaH * cosThetaO / wo_dot_wh),
                        (2.f * cosThetaH * cosThetaI / wo_dot_wh)));

    pdf_ret = this->PDF(wo,wi);
    return R * D * G * F /
               (4.f * cosThetaI * cosThetaO);
}

glm::vec3 BlinnMicrofacetBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float &pdf_ret) const
{

    float r1 = sampling::unif_random();
    float r2 = sampling::unif_random();

    float costheta = glm::pow(r1, 1.f / (this->exponent+1));
    float sintheta = glm::sqrt(glm::max(0.f, 1.f - costheta*costheta));
    float phi = r2 * 2.0f * PI;
    glm::vec3 wh = glm::vec3(sintheta * glm::cos(phi),
                             sintheta * glm::sin(phi),
                             costheta);


    if (wh.z * wo.z < 0) wh = -wh;

    // Compute incident direction by reflecting about $\wh$
    wi_ret = -wo + 2.f * glm::dot(wo, wh) * wh;

    // Compute PDF
    float blinn_pdf = ((exponent + 1.f) * glm::pow(costheta, exponent)) /
                      (2.f * PI * 4.f * glm::dot(wo, wh));
    if (glm::dot(wo, wh) <= 0.f) blinn_pdf = 0.f;
    pdf_ret = blinn_pdf;

    float dummy = 1.0f;
    return this->EvaluateScatteredEnergy(wo,wi_ret,dummy);
}


glm::vec3 BlinnMicrofacetBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    return glm::vec3(0);
}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 wh = glm::normalize(wo + wi);
    float costheta = glm::abs(glm::dot(wh, glm::vec3(0,0,1.f)));
    float blinn_pdf = ((exponent + 1.f) * glm::pow(costheta, exponent)) /
                      (2.f * PI * 4.f * glm::dot(wo, wh));
    if (glm::dot(wo, wh) <= 0.f) blinn_pdf = 0.f;
    return blinn_pdf;
    //return::BxDF::PDF(wo, wi);
}

