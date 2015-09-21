#include <scene/materials/material.h>

Material::Material() :
    Material(glm::vec3(0.5f, 0.5f, 0.5f))
{}

Material::Material(const glm::vec3 &color):
    name("MATERIAL"),
    base_color(color),
    emissive(false),
    reflectivity(0),
    refract_idx_in(0),
    refract_idx_out(0)
{
    texture = NULL;
}

glm::vec4 Material::GetTextureColor(const glm::vec2 &uv_coord)
{
    if(texture == NULL)
    {
        return glm::vec4(1,1,1,1);
    }
    else
    {
        int X = texture->TellWidth() * uv_coord.x;
        int Y = texture->TellHeight() * uv_coord.y;
        RGBApixel color = texture->GetPixel(X, Y);
        return glm::vec4(color.Red, color.Green, color.Blue, color.Alpha);
    }
}
