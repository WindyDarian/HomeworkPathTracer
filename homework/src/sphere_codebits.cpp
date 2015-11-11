//Add these functions to your sphere.cpp file
//Make sure UniformCodePDF is put at the top of the file; it's not a member of the Sphere class.

float UniformConePdf(float cosThetaMax)
{
    return 1.f / (2.f * PI * (1.f - cosThetaMax));
}

float Sphere::RayPDF(const Intersection &isx, const Ray &ray) {
    glm::vec3 Pcenter = transform.position();
    float radius = 0.5f*(transform.getScale().x + transform.getScale().y + transform.getScale().z)/3.0f;
    // Return uniform weight if point inside sphere
    if (glm::distance2(isx.point, Pcenter) - radius*radius < 1e-4f)
        return Geometry::RayPDF(isx, ray);

    // Compute general sphere weight
    float sinThetaMax2 = radius*radius / glm::distance2(isx.point, Pcenter);
    float cosThetaMax = glm::sqrt(max(0.f, 1.f - sinThetaMax2));
    return UniformConePdf(cosThetaMax);
}

void Sphere::ComputeArea()
{
    glm::vec3 scale = transform.getScale();
    float a = scale.x*0.5f; float b = scale.y*0.5f; float c = scale.z*0.5f;
    area = 4*PI*glm::pow((glm::pow(a*b, 1.6f) + glm::pow(a*c, 1.6f) + glm::pow(b*c, 1.6f))/3.0f, 1/1.6f);
}

Intersection Sphere::GetSurfaceSample(float u1, float u2, const glm::vec3 &isx_normal)
{
    float z = 1.f - 2.f * u1;
    float r = glm::sqrt(glm::max(0.f, 1.f - z*z));
    float phi = 2.f * PI * u2;
    float x = r * glm::cos(phi);
    float y = r * glm::sin(phi);
    glm::vec3 normal3 = glm::normalize(glm::vec3(x,y,z));
    if(glm::dot(isx_normal, normal3) > 0)
    {
    	normal3 = -normal3;
    }
    glm::vec4 pointL(x/2, y/2, z/2, 1);
    glm::vec4 normalL(normal3,0);
    glm::vec2 uv = this->GetUVCoordinates(glm::vec3(pointL));
    glm::vec3 color = Material::GetImageColor(uv, this->material->texture);
    glm::vec3 T = glm::normalize(glm::cross(glm::vec3(0,1,0), normal3));
    glm::vec3 B = glm::cross(normal3, T);

    Intersection result;
    result.point = glm::vec3(transform.T() * pointL);
    result.normal = glm::normalize(glm::vec3(transform.invTransT() * normalL));
    result.texture_color = color;
    result.tangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(T, 0)));
    result.bitangent = glm::normalize(glm::vec3(transform.T() * glm::vec4(B, 0)));
    result.object_hit = this;
    return result;
}