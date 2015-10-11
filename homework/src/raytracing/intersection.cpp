#include <raytracing/intersection.h>

Intersection::Intersection():
    point(glm::vec3(0)),
    normal(glm::vec3(0)),
    t(-1),
    color(1.f)
{
    object_hit = nullptr;
}

Intersection::Intersection(glm::vec3 point, glm::vec3 normal, float t , glm::vec3 surface_color, Geometry* object_hit):
    point(point),
    normal(normal),
    t(t),
    object_hit(object_hit),
    color(surface_color)
{}

IntersectionEngine::IntersectionEngine()
{
    scene = NULL;
}

Intersection IntersectionEngine::GetIntersection(Ray r)
{

    float min_t = std::numeric_limits<float>::max();

    Intersection result;

    /* // not using BVH
    for (auto obj : scene->objects)
    {
        Intersection intersection = obj->GetIntersection(r);
        if (intersection.object_hit == nullptr)
            continue;   // missed this object

        if (intersection.t < min_t && intersection.t > 0)
        {
            min_t = intersection.t;
            result = intersection;
        }
    }*/

    /*
    // using BVH (old)
    std::set<Geometry*> possible_objs;
    this->scene->kdnode_objects->appendIntersections(possible_objs, r);
    for (auto obj : possible_objs)
    {
        Intersection intersection = obj->GetIntersection(r);
        if (intersection.object_hit == nullptr)
            continue;   // missed this object

        if (intersection.t < min_t && intersection.t > 0)
        {
            min_t = intersection.t;
            result = intersection;
        }
    }*/

    result = this->scene->bvh->GetIntersection(r);
    if (result.t < 0)
        result = Intersection();

    return result;
}
