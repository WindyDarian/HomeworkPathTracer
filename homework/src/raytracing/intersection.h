#pragma once
#include <la.h>
#include <scene/geometry/geometry.h>
#include <raytracing/ray.h>
#include <scene/scene.h>
#include <set>

class Material;
class Geometry;
class Scene;
class BVHNode;


class Intersection
{
public:
    Intersection();
    Intersection(glm::vec3 point, glm::vec3 normal, glm::vec3 tangent, float t, glm::vec3 color, Geometry* object_hit);

    glm::vec3 point;      //The place at which the intersection occurred
    glm::vec3 normal;     //The surface normal at the point of intersection
    glm::vec3 tangent;    //The surface tangent at the POI
    glm::vec3 bitangent;  //The surface bitangent at the POI    float t;              //The parameterization for the ray (in world space) that generated this intersection.
                          //t is equal to the distance from the point of intersection to the ray's origin if the ray's direction is normalized.
    float t;              //The parameterization for the ray (in world space) that generated this intersection.
                          //t is equal to the distance from the point of intersection to the ray's origin if the ray's direction is normalized.


    glm::vec3 color;
    Geometry* object_hit; //The object that the ray intersected, or nullptr if the ray hit nothing.

};

class IntersectionEngine
{
public:
    IntersectionEngine();
    Scene *scene;

    Intersection GetIntersection(Ray r);
    QList<Intersection> GetAllIntersections(Ray r);
};
