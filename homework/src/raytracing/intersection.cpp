#include <raytracing/intersection.h>

Intersection::Intersection():
    point(glm::vec3(0)),
    normal(glm::vec3(0)),
    t(-1)
{
    object_hit = NULL;
}

Intersection::Intersection(glm::vec3 point, glm::vec3 normal, float t , Geometry* object_hit):
    point(point),
    normal(normal),
    t(t),
    object_hit(object_hit)
{}

IntersectionEngine::IntersectionEngine()
{
    scene = NULL;
}

Intersection IntersectionEngine::GetIntersection(Ray r)
{
    float near_clip = scene->camera.near_clip;
    float far_clip =  scene->camera.far_clip;
    glm::vec3 camera_look = scene->camera.look;
    float min_t = std::numeric_limits<float>::max();

    Intersection result;

    for (auto obj : scene->objects)
    {
        Intersection intersection = obj->GetIntersection(r);
        if (intersection.object_hit == NULL)
            continue;   // missed this object

          // calculate intersection depth in camera frustum
        float d = glm::dot((r.direction * intersection.t), camera_look);
        if (d  < near_clip || d > far_clip)
            continue;   // not in clip range

        if (intersection.t < min_t)
        {
            min_t = intersection.t;
            result = intersection;
        }
    }

    return result;
}
