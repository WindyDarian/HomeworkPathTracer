#pragma once
#include <la.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>

// I didn't make BoundingBox itself drawable because I decided to use it
// as a pure data class like Ray and use its copy constructor a lot, which
// Drawable will encounter some problems at.
// See scene/gemoetry/boundingboxframe.h

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

    BoundingBox getTransformedCopy(const glm::mat4& T) const;

};

