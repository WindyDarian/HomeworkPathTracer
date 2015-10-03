#include <scene/materials/phongmaterial.h>

PhongMaterial::PhongMaterial():
    PhongMaterial(glm::vec3(0.5f, 0.5f, 0.5f))
{}

PhongMaterial::PhongMaterial(const glm::vec3 &color):
    Material(color),
    specular_power(10)
{}

glm::vec3 PhongMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray)
{
    float ka = 0.05f;
    float kd = 0.4f;
    float ks = 0.55f;

    glm::vec3 A(1.f, 1.f, 1.f);
    glm::vec3 D = glm::clamp(glm::dot(isx.normal, -incoming_ray),0.f,1.f) * isx.color;
    glm::vec3 H = (outgoing_ray - incoming_ray) / 2.f;
    glm::vec3 S = glm::max(glm::pow(glm::dot(H, isx.normal), this->specular_power), 0.f) * glm::vec3(1.f);

    return ka * A + kd * D + ks * S;
}
