#pragma once
#include <la.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>


// I didn't make BoundingBox itself drawable because
// I was encountering some problem with copy constructor of a drawable
// as it sometimes resets the create() state when a copied instance dies.

// the drawable frame at scene/gemoetry/boundingboxframe.h

class BoundingBox
{
public:
    BoundingBox();
    BoundingBox(const glm::vec3& min, const glm::vec3& max);
    BoundingBox(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3 &v3);
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 getCenter() const;
    bool valid;

    void cacheCenter();

    bool isIntersected(const Ray& r) const;

    void expand(const glm::vec3& point);
    void merge(const BoundingBox& box);

    BoundingBox getTransformedCopy(const glm::mat4& T) const;

private:
    bool center_valid = false;
    glm::vec3 center;

};

