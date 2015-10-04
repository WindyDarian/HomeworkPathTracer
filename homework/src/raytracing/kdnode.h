#pragma once
#include <la.h>
#include <memory>
#include <set>
#include <raytracing/boundingbox.h>
#include <raytracing/ray.h>
#include <scene/geometry/geometry.h>

class Geometry;


// K-D Tree Node Class
// I used it in both scene management and in mesh class
class KDNode
{
public:

    static KDNode* build(const std::list<Geometry*>& objs, int depth = 0);

    KDNode();

    //Calculate all possible hit targets and append to an intersection set
    void appendIntersections(std::set<Geometry*>& hitset, const Ray &r);

    //KDNode* left = nullptr;
    //KDNode* right = nullptr;
    std::unique_ptr<KDNode> left = nullptr;
    std::unique_ptr<KDNode> right = nullptr;
    BoundingBox bounding_box;
    Geometry* obj = nullptr; //the geometry contained in this node, only for leaf nodes


};


