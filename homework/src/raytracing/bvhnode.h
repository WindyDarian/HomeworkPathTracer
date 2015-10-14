#pragma once
#include <la.h>
#include <memory>
#include <set>
#include <raytracing/boundingbox.h>
#include <raytracing/ray.h>
#include <raytracing/intersection.h>

#include <scene/geometry/geometry.h>
#include <scene/geometry/boundingboxframe.h>


class Geometry;
class Intersection;


// K-D Tree Node Class
// I used it in both scene management and in mesh class
class BVHNode
{
public:

    enum SplitMethod { SPLIT_EQUAL_COUNTS, SPLIT_SAH};

    static SplitMethod CurrentSplitMethodSettings;
    static std::string GetCurrentSplitMethodText()
    { if (CurrentSplitMethodSettings == SPLIT_EQUAL_COUNTS) return "SPLIT_EQUAL_COUNTS";
        else return "SPLIT_SAH";}
    static bool BVHIntersectionDisabled;

    static BVHNode* build(const std::list<Geometry *> &objs, SplitMethod split_method);

    BVHNode();

    Intersection GetIntersection(const Ray& r) const; // TODO: make this and geometry const Ray&


    //Calculate all possible hit targets and append to an intersection set
    //void appendIntersections(std::set<Geometry*>& hitset, const Ray &r);


    void appendBoundingBoxFrame(std::list<BoundingBoxFrame*>& list,
                                  glm::vec3 color,
                                  float color_decay = 0.5f,
                                int maxdepth = 50,
                                int depth = 0);

    //KDNode* left = nullptr;
    //KDNode* right = nullptr;
    std::unique_ptr<BVHNode> left = nullptr;
    std::unique_ptr<BVHNode> right = nullptr;
    BoundingBox bounding_box;
    Geometry* obj = nullptr; //the geometry contained in this node, only for leaf nodes



    bool interlanced = false;
    int split_axis = 0;

private:
    static BVHNode* generateNode_equal_counts(std::vector<Geometry *> &objs, int start, int end, int depth = 0);
    static BVHNode* generateNode_sah(const std::list<Geometry *> &objs, int depth = 0);

    // to prevent many intersections being created thus enhance the performance
    Intersection *calculateIntersection(const Ray& r) const;

};


