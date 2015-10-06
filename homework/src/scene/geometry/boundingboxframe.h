#pragma once
#include <la.h>
#include <openGL/drawable.h>
#include <raytracing/boundingbox.h>

// I didn't make BoundingBox itself drawable because I decided to use it
// as a pure data class like Ray and use its copy constructor a lot, which
// Drawable will encounter some problems at.
// See raytracing/boundingbox.h

class BoundingBoxFrame: public Drawable
{
public:
    BoundingBoxFrame(const BoundingBox& bounding_box);
    BoundingBoxFrame(const BoundingBox& bounding_box, const glm::vec3& color);

    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 color;

    virtual void create();
    virtual GLenum drawMode();
};

