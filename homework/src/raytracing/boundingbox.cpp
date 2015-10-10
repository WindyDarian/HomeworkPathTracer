#include "boundingbox.h"

#include <la.h>

BoundingBox::BoundingBox():valid(false), center_valid(false)
{
}

BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max):
    min(glm::min(min,max)),
    max(glm::max(min,max)),
    valid(true),
    center_valid(false)
{
}

BoundingBox::BoundingBox(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3):
    min(glm::min(glm::min(v1,v2),v3)),
    max(glm::max(glm::max(v1,v2),v3)),
    valid(true),
    center_valid(false)
{
}

glm::vec3 BoundingBox::getCenter() const
{
    if (!this->center_valid){
       return (min + max) * 0.5f;
    }
    return center;
}

void BoundingBox::cacheCenter()
{
    this->center = (min + max) * 0.5f;
    this->center_valid = true;
}

void BoundingBox::expand(const glm::vec3& point)
{
    this->center_valid = false;
    if (!this->valid)
    {
        this->min = point;
        this->max = point;
        this->valid = true;
        return;
    }
     for (int i = 0; i < 3; i++)
     {
         this->min[i] = glm::min(this->min[i], point[i]);
         this->max[i] = glm::max(this->max[i], point[i]);
     }
}

void BoundingBox::merge(const BoundingBox& box)
{
    if (!box.valid)
        return;

    this->center_valid = false;
    if (!this->valid)
    {
        this->min = box.min;
        this->max = box.max;
        this->valid = true;
        return;
    }
     for (int i = 0; i < 3; i++)
     {
         this->min[i] = glm::min(this->min[i], box.min[i]);
         this->max[i] = glm::max(this->max[i], box.max[i]);
     }
}

BoundingBox BoundingBox::getTransformedCopy(const glm::mat4 &T) const
{
    BoundingBox b;

    std::vector<glm::vec4> vertices;
    vertices.reserve(8);
    vertices.push_back(glm::vec4(min.x, min.y, min.z, 1.f));
    vertices.push_back(glm::vec4(max.x, min.y, min.z, 1.f));
    vertices.push_back(glm::vec4(min.x, max.y, min.z, 1.f));
    vertices.push_back(glm::vec4(min.x, min.y, max.z, 1.f));
    vertices.push_back(glm::vec4(max.x, min.y, max.z, 1.f));
    vertices.push_back(glm::vec4(max.x, max.y, min.z, 1.f));
    vertices.push_back(glm::vec4(min.x, max.y, max.z, 1.f));
    vertices.push_back(glm::vec4(max.x, max.y, max.z, 1.f));


    for (auto v: vertices)
    {
        b.expand(glm::vec3(T * v));
    }

    b.cacheCenter();
    return b;
}

bool BoundingBox::isIntersected(const Ray& r) const
{
    if (!this->valid) return false;

    float tnear = std::numeric_limits<float>::lowest();
    float tfar = std::numeric_limits<float>::max();

    for (int i=0;i<3;i++) // x slab then y then z
    {
        float t0, t1;

        if (fequal(r.direction[i], 0.f))
        {
            if (r.origin[i] < this->min[i] || r.origin[i] > this->max[i])
                return false;
            else
            {
                t0 = std::numeric_limits<float>::lowest();
                t1 = std::numeric_limits<float>::max();
            }
        }
        else
        {
            t0 = (this->min[i] - r.origin[i]) / r.direction[i];
            t1 = (this->max[i] - r.origin[i]) / r.direction[i];
        }

        tnear = glm::max(tnear, glm::min(t0, t1));
        tfar = glm::min(tfar, glm::max(t0, t1));
    }

    if (tfar < tnear) return false; // no intersection

    if (tfar < 0) return false; // behind origin of ray

    return true;

}
