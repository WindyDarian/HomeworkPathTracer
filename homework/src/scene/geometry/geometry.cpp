#include <scene/geometry/geometry.h>

Intersection Geometry::pickSampleIntersection(float random1, float random2)
{return Intersection();}

float Geometry::RayPDF(const Intersection &isx, const Ray &ray)
{
    //The isx passed in was tested ONLY against us (no other scene objects), so we test if NULL
    //rather than if != this.
    if(!isx.object_hit)
    {
        return 0;
    }

    //Add more here

    glm::vec3 d = ray.origin - isx.point;
    float r2 = glm::length2(d);
    if (r2 <= 0)
        return 0;

    float costheta = glm::dot(glm::normalize(d), isx.normal);
    if (costheta<=0)
        return 0;

    assert(area > 0);

    return r2 / (area * costheta);
}

glm::vec3 Geometry::computeLocalTangent(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2)
{
    auto dp1 = p1 - p0;
    auto dp2 = p2 - p0;
    auto p0uv = this->GetUVCoordinates(p0);
    auto duv1 = this->GetUVCoordinates(p1) - p0uv;
    auto duv2 = this->GetUVCoordinates(p2) - p0uv;

    glm::vec3 tangent = (duv2.y * dp1 - duv1.y * dp2) /
            (duv2.y * duv1.x - duv1.y * duv2.x);

    return tangent;
}
