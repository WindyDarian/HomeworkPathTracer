#pragma once

#include <scene/geometry/geometry.h>

//A cube is assumed to have side lengths of 1 and a center of <0,0,0>. This means all vertices are of the form <+/-0.5, +/-0.5, +/-0.5>
//These attributes can be altered by applying a transformation matrix to the cube.
class Cube : public Geometry
{
public:


    Intersection GetIntersection(const Ray& r);
    void create();
    glm::vec2 GetUVCoordinates(const glm::vec3 &point);

protected:
    virtual BoundingBox calculateBoundingBox();
private:
    glm::vec3 getNormal(const glm::vec3& point) const; // Get normal from point
    std::pair<int, int> getFaceNormalAxisAndDirection(const glm::vec3& point) const;

    float bounds[3][2] = {{-0.5, 0.5}, {-0.5, 0.5}, {-0.5, 0.5}};
    std::vector<glm::vec3> vertexes;

};
