#pragma once
#include <la.h>
#include <openGL/drawable.h>
#include <raytracing/boundingbox.h>

// I didn't make BoundingBox itself drawable because
// I was encountering some problem with copy constructor of a drawable
// as it sometimes resets the create() state when a copied instance dies.
// for bounding box itself see raytracing/boundingbox.h

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

