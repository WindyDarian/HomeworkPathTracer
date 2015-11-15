#pragma once
#include <scene/geometry/geometry.h>

//A square is aligned with the XY plane with a normal aligned with the positive Z axis. Its side length is 1, and it is centered at the origin.
//These attributes can be altered by applying a transformation matrix to the square.
class SquarePlane : public Geometry
{
    Intersection GetIntersection(const Ray& r);
    void create();
    glm::vec2 GetUVCoordinates(const glm::vec3 &point);

    virtual Intersection pickSampleIntersection(std::function<float()> randomf, const glm::vec3* target_normal = nullptr);
    virtual glm::vec3 ComputeNormal(const glm::vec3 &P);
    virtual void ComputeArea();
protected:
    virtual BoundingBox calculateBoundingBox();
};
