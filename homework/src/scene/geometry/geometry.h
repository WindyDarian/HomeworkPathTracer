#pragma once

#include <raytracing/intersection.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>
#include <scene/materials/material.h>
#include <scene/transform.h>
#include <raytracing/boundingbox.h>
#include <memory>

class Intersection;//Forward declaration because Intersection and Geometry cross-include each other
class Material;

//Geometry is an abstract class since it contains a pure virtual function (i.e. a virtual function that is set to 0)
class Geometry : public Drawable
{
public:
    Geometry():
    name("GEOMETRY"), transform()
    {
        material = NULL;
    }

    virtual ~Geometry(){}
    virtual Intersection GetIntersection(Ray r) = 0;
    virtual void SetMaterial(Material* m){material = m;}

    virtual BoundingBox calculateBoundingBox() = 0;
    const BoundingBox& getBoundingBox()
    {
        if (bounding_box == nullptr)
            bounding_box.reset(new BoundingBox(calculateBoundingBox()));
             // reset automatically deletes old pointer

        return *bounding_box;
    }

    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point) = 0;

    QString name;//Mainly used for debugging purposes
    Transform transform;
    Material* material;


protected:
    // using smart pointer so I don't need to release it manually
    std::unique_ptr<BoundingBox> bounding_box = nullptr;

};
