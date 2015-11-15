#pragma once

#include <scene/geometry/geometry.h>

//A sphere is assumed to have a radius of 0.5 and a center of <0,0,0>.
//These attributes can be altered by applying a transformation matrix to the sphere.
class Sphere : public Geometry
{
public:
    Intersection GetIntersection(const Ray& r);
    void create();
    glm::vec2 GetUVCoordinates(const glm::vec3 &point);

    virtual glm::vec3 ComputeNormal(const glm::vec3 &P);
    virtual void ComputeArea();
    virtual float RayPDF(const Intersection &isx, const Ray &ray);
    virtual Intersection pickSampleIntersection(std::function<float()> randomf, const glm::vec3* target_normal = nullptr);

protected:
    virtual BoundingBox calculateBoundingBox();

private:
    glm::vec3 pickSamplePointLocal(std::function<float()> &randomf);
};
