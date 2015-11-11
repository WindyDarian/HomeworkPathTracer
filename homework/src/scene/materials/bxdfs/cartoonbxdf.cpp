#include "cartoonbxdf.h"



glm::vec3 CartoonBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    glm::vec3 color_base = this->diffuse_color * INV_PI ;
    glm::vec3 normal(0.f, 0.f, 1.f);
    float v = glm::dot(wi, normal);
    if (v <= threshold)
       return glm::vec3(0.f);
    int current_level = int( (1.f - v) * levels/ (1.f - threshold) ) + 1;

        return color_base/(v * current_level);
}
