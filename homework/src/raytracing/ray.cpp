#include <raytracing/ray.h>

Ray::Ray(const glm::vec3 &o, const glm::vec3 &d):
    origin(o),
    direction(glm::normalize(d)),
    transmitted_color(1,1,1)
{}

Ray::Ray(const glm::vec4 &o, const glm::vec4 &d):
    Ray(glm::vec3(o), glm::vec3(d))
{}

Ray::Ray(const Ray &r):
    Ray(r.origin, r.direction)
{
    transmitted_color = r.transmitted_color;
}

Ray Ray::GetTransformedCopy(const glm::mat4 &T) const
{
      // w = 1 for points
    glm::vec4 origin4 = glm::vec4(this->origin, 1.0f);
      // w = 0 for pure vectors
    glm::vec4 direction4 = glm::vec4(this->direction, 0.0f);

      //transform
    glm::vec3 result_origin(T * origin4);
    glm::vec3 result_direction( T * direction4 );

      // normalization done in constructor
    return Ray(result_origin, result_direction);
}
