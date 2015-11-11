#pragma once

#include <raytracing/intersection.h>
#include <openGL/drawable.h>
#include <raytracing/ray.h>
#include <scene/materials/material.h>
#include <scene/transform.h>
#include <raytracing/boundingbox.h>
#include <memory>
#include <random>
#include <functional>
class Intersection;//Forward declaration because Intersection and Geometry cross-include each other
class Material;

//Geometry is an abstract class since it contains a pure virtual function (i.e. a virtual function that is set to 0)
class Geometry : public Drawable
{
public:
    Geometry();



    virtual ~Geometry(){}
    virtual Intersection pickSampleIntersection(std::function<float()> randomf, const glm::vec3* target_point = nullptr);
    virtual Intersection GetIntersection(const Ray& r) = 0;
    virtual void SetMaterial(Material* m){material = m;}

    virtual glm::vec3 getCentroid()
    {
        if (!centroid_valid)
        {
            this->centroid = calculateCentroid();
            centroid_valid = true;
        }
        return centroid;
    }

    const BoundingBox& getBoundingBox()
    {
        if (bounding_box_ptr == nullptr)
        {
            bounding_box_ptr.reset(new BoundingBox(calculateBoundingBox()));
             // reset automatically deletes old pointer
        }
        return *bounding_box_ptr;
    }

    virtual glm::vec2 GetUVCoordinates(const glm::vec3 &point) = 0;
    virtual glm::vec3 ComputeNormal(const glm::vec3 &P) = 0;

    //Returns the solid-angle weighted probability density function given a point we're trying to illuminate and
    //a ray going towards the Geometry
    virtual float RayPDF(const Intersection &isx, const Ray &ray);

    //This is called by the XML Reader after it's populated the scene's list of geometry
    //Computes the surface area of the Geometry in world space
    //Remember that a Geometry's Transform's scale is applied before its rotation and translation,
    //so you'll never have a skewed shape
    virtual void ComputeArea() = 0;

    QString name;//Mainly used for debugging purposes
    Transform transform;
    Material* material;
    std::unique_ptr<BoundingBox> bounding_box_ptr = nullptr;

protected:
    virtual BoundingBox calculateBoundingBox() = 0;
    virtual glm::vec3 calculateCentroid(){return glm::vec3(this->transform.T() * glm::vec4(glm::vec3(0.f), 1.f));}

    bool centroid_valid = false;
    glm::vec3 centroid;

    float area=0;

    glm::vec3 computeLocalTangent(const glm::vec3& p0, const glm::vec3 &p1, const glm::vec3 &p2);
    static std::unique_ptr<std::mt19937> mersenne_generator_ptr;

};
