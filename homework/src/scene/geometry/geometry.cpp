#include <scene/geometry/geometry.h>
#include <ctime>
#include <random>

std::unique_ptr<std::mt19937> Geometry::mersenne_generator_ptr(new std::mt19937(std::time(nullptr)));

Geometry::Geometry():
    name("GEOMETRY"), transform(),
      bounding_box_ptr(nullptr)
{
        material = NULL;
}

Intersection Geometry::pickSampleIntersection(std::function<float ()> randomf, const glm::vec3 *target_normal)
{
    return Intersection();
}

float Geometry::RayPDF(const Intersection &isx, const Ray &ray)
{
    //The isx passed in was tested ONLY against us (no other scene objects), so we test if NULL
    //rather than if != this.
    if(!isx.object_hit)
    {
        return 0;
    }

    glm::vec3 d = ray.origin - isx.point;
    float r2 = glm::length2(d);
    if (r2 <= 0)
        return 0;

    float costheta = glm::dot(glm::normalize(d), isx.normal);
    if (costheta<=0)
        return 0;

    Q_ASSERT(area > 0);

    return r2 / (area * costheta);
}

glm::vec3 Geometry::computeLocalTangent(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2)
{
    auto dp1 = p1 - p0;
    auto dp2 = p2 - p0;
    auto p0uv = this->GetUVCoordinates(p0);
    auto duv1 = this->GetUVCoordinates(p1) - p0uv;
    auto duv2 = this->GetUVCoordinates(p2) - p0uv;

    glm::vec3 f1 = (duv2.y * dp1 - duv1.y * dp2);
    float f2 = (duv2.y * duv1.x - duv1.y * duv2.x);
    glm::vec3 tangent;

    if (fequal(glm::length2(f1), 0.f) || fequal(f2, 0.f))
    {
       tangent = glm::normalize(dp1 + dp2);
    }
    else
    {
        tangent= glm::normalize( f1 / f2 );
    }


    return tangent;
}


