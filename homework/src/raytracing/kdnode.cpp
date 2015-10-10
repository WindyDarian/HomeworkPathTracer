#include "kdnode.h"

#include <algorithm>
#include <memory>

// Just ignore this for my hw02
// Because I wanted to render the dragon.obj after hw01 is done
// it helped my Wahoo

KDNode::KDNode()
{

}

Intersection KDNode::GetIntersection(const Ray &r) const
{

    if ((!this->left) && this->right)
        return this->right->GetIntersection(r);
    if ((!this->right) && this->left)
        return this->left->GetIntersection(r);

    if (!this->bounding_box.isIntersected(r))
        return Intersection();
    if (this->obj)
    {
        return obj->GetIntersection(r);
    }

    if (!interlanced)
    {
        // If left and right child don't overlap
        if (r.origin[split_axis] <= left->bounding_box.max[split_axis])
        {
            Intersection result = left->GetIntersection(r);
            if (result.object_hit && result.t >= 0) return result;
            return right->GetIntersection(r);
        }
        else
        {
            Intersection result = right->GetIntersection(r);
            if (result.object_hit && result.t >= 0) return result;
            return left->GetIntersection(r);
        }
    }
    else
    {
        // In case bounding box of left and right child overlap
        Intersection il = left->GetIntersection(r);
        Intersection ir = right->GetIntersection(r);

        if (!il.object_hit || il.t < 0)
            return ir;
        else if (!ir.object_hit || ir.t < 0)
            return il;
        else
        {
             return (il.t > ir.t)? ir : il;
        }
    }
}

KDNode* KDNode::build(const std::list<Geometry *> & objs, SplitMethod split_method)
{
    if (objs.size() == 0)
    {
        return nullptr;
    }


    std::vector<Geometry *> objs_vec;
    objs_vec.reserve(objs.size());
    for (auto obj : objs)
    {
        objs_vec.push_back(obj);
    }

    if (split_method == SPLIT_EQUAL_COUNTS )
    {

        return KDNode::generateNode_equal_counts(objs_vec, 0, objs_vec.size(), 0);
    }
}


//void KDNode::appendIntersections(std::set<Geometry*>& hitset, const Ray& r)
//{
//    if (!this->bounding_box.isIntersected(r))
//        return;

//    if (this->left != nullptr)
//        this->left->appendIntersections(hitset, r);

//    if (this->right != nullptr)
//        this->right->appendIntersections(hitset, r);

//    if (this->obj != nullptr)
//    {
//        hitset.insert(this->obj);
//    }
//}

void KDNode::appendBoundingBoxFrame(std::list<BoundingBoxFrame *>& list, glm::vec3 color, float color_decay)
{
    glm::vec3 newcolor = color * color_decay;

    if (this->left != nullptr)
    {
        this->left->appendBoundingBoxFrame(list, newcolor, color_decay);
    }

    if (this->right != nullptr)
    {

        this->right->appendBoundingBoxFrame(list, newcolor, color_decay);
    }

    list.push_back(new BoundingBoxFrame(this->bounding_box, color));
}


struct CenterComparasion
{
    int axis;

    CenterComparasion(int axis)
    {
        this->axis = axis;
    }
    bool operator()(Geometry* a,
                    Geometry*b) const
    {
        return a->getBoundingBox().getCenter()[axis] <
                b->getBoundingBox().getCenter()[axis];
    }
};

KDNode *KDNode::generateNode_equal_counts(std::vector<Geometry *> &objs, int start, int end, int depth)
{
    int size = end - start;

    //  empty
    if (size <= 0) return nullptr;

    KDNode* node = new KDNode();

    //  contains unique node
    if (size == 1)
    {
        node->obj = objs[start];
        node->bounding_box = objs[start]->getBoundingBox();
        return node;
    }


    // use x,y,z,x,y,z as dividing axis
    int axis = depth % 3;

    node->split_axis = axis;

    for (int i = start; i <  end; i++)
    {
        auto b = objs[i]->getBoundingBox();
        node->bounding_box.merge(b);
    }
    node->bounding_box.cacheCenter();



    int median_index = (size - 1) / 2 + start;
    std::nth_element(objs.begin() + start,
                     objs.begin() + median_index,
                     objs.begin() + end,
                     CenterComparasion(axis)
                     );

    node->left = std::unique_ptr<KDNode>(KDNode::generateNode_equal_counts(objs, start, median_index + 1, depth + 1));
    node->right = std::unique_ptr<KDNode>(KDNode::generateNode_equal_counts(objs, median_index + 1, end, depth + 1));

    if (node->left && node->right
            && node->left->bounding_box.max[axis] > node->right->bounding_box.min[axis])
        node->interlanced = true;

    return node;

}

KDNode *KDNode::generateNode_sah(std::vector<Geometry *> &objs, int start, int end, int depth)
{
    int size = end - start;

    //  empty
    if (size <= 0) return nullptr;

    KDNode* node = new KDNode();

    //  contains unique node
    if (size == 1)
    {
        node->obj = objs[start];
        node->bounding_box = objs[start]->getBoundingBox();
        return node;
    }



    // TODO


    if (node->left && node->right
            && node->left->bounding_box.max[axis] > node->right->bounding_box.min[axis])
        node->interlanced = true;

    return node;
}
