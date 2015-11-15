#include <scene/materials/lightmaterial.h>

glm::vec3 LightMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, float& pdf_ret, BxDFType flags) const
{
    pdf_ret = 0.f; // use geometry.raypdf() instead!
    return glm::dot(wiW, isx.normal) > 0.0f ? (this->base_color * isx.texture_color * this->intensity) : glm::vec3(0.0f);
}
