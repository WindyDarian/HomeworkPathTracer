#pragma once
#include <la.h>
#include <raytracing/ray.h>

class BoundingBox
{
public:
    BoundingBox();
    BoundingBox(const glm::vec3& min, const glm::vec3& max);
    BoundingBox(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3 &v3);
    glm::vec3 min;
    glm::vec3 max;
    bool valid;

    bool isIntersected(const Ray& r);

    void expand(const glm::vec3& point);
    void merge(const BoundingBox& box);

};

