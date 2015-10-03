#pragma once
#include <la.h>
#include <memory>
#include <set>
#include <raytracing/boundingbox.h>
#include <raytracing/ray.h>
#include <scene/geometry/geometry.h>

// Just ignore this for my hw02
// Because I wanted to render the dragon.obj after hw01 is done
// it helped my Wahoo

// K-D Tree Node Class for optimization
class KDNode
{
public:

    static KDNode* build(const std::list<Geometry*>& objs, int depth = 0, int max_depth = 5000);

    KDNode();

    void appendIntersections(std::set<Geometry*>& hitset, const Ray &r); //Calculate all possible hit targets and append to an intersection set


    //KDNode* left = nullptr;
    //KDNode* right = nullptr;
    std::unique_ptr<KDNode> left;
    std::unique_ptr<KDNode> right;
    BoundingBox bounding_box;
    std::list<Geometry*> obj; //the geometry contained in this node, only for leaf nodes
};


