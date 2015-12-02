#include "bvhnode.h"

#include <algorithm>
#include <memory>

BVHNode::SplitMethod BVHNode::CurrentSplitMethodSettings = SPLIT_SAH;
bool BVHNode::BVHIntersectionDisabled = false;

BVHNode::BVHNode()
{

}

Intersection BVHNode::GetIntersection(const Ray &r) const
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

BVHNode* BVHNode::build(const std::list<Geometry *> & objs, SplitMethod split_method)
{
    if (objs.size() == 0)
    {
        return nullptr;
    }


    if (split_method == SPLIT_EQUAL_COUNTS )
    {

        std::vector<Geometry *> objs_vec;
        objs_vec.reserve(objs.size());
        for (auto obj : objs)
        {
            objs_vec.push_back(obj);
        }

        return BVHNode::generateNode_equal_counts(objs_vec, 0, objs_vec.size(), 0);
    }
    else if ( split_method == SPLIT_SAH)
    {
        return BVHNode::generateNode_sah(objs, 0);
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

void BVHNode::appendBoundingBoxFrame(std::list<BoundingBoxFrame *> &list, glm::vec3 color,
                                     float color_decay, int maxdepth, int depth)
{
    if (depth >= maxdepth) return;
    glm::vec3 newcolor = color * color_decay;

    if (this->left != nullptr)
    {
        this->left->appendBoundingBoxFrame(list, newcolor, color_decay, maxdepth, depth + 1);
    }

    if (this->right != nullptr)
    {

        this->right->appendBoundingBoxFrame(list, newcolor, color_decay, maxdepth, depth + 1);
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

BVHNode *BVHNode::generateNode_equal_counts(std::vector<Geometry *> &objs, int start, int end, int depth)
{
    int size = end - start;

    //  empty
    if (size <= 0) return nullptr;

    BVHNode* node = new BVHNode();

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
    node->bounding_box.cacheInfomation();



    int median_index = (size - 1) / 2 + start;

    // find the median and make left elements all smaller or equal than median (selection algorithm)
    std::nth_element(objs.begin() + start,
                     objs.begin() + median_index,
                     objs.begin() + end,
                     CenterComparasion(axis)
                     );

    node->left.reset(BVHNode::generateNode_equal_counts(objs, start, median_index + 1, depth + 1));
    node->right.reset(BVHNode::generateNode_equal_counts(objs, median_index + 1, end, depth + 1));

    if (node->left && node->right
            && node->left->bounding_box.max[axis] > node->right->bounding_box.min[axis])
        node->interlanced = true;

    return node;

}


struct SAHBucket
{
    BoundingBox bbox;
    std::list<Geometry *> items;
};


BVHNode *BVHNode::generateNode_sah(const std::list<Geometry *> &objs, int depth)
{
    // reference: p217,  physically based rendering 2nd edition


    int size = objs.size();

    //  empty
    if (size <= 0) return nullptr;


    // object size too small to make the cost of sah useful
    if (size <= 4)
    {
        std::vector<Geometry *> objs_copy;
        objs_copy.reserve(size);
        for(auto obj: objs)
        {
            objs_copy.push_back(obj);
        }
        BVHNode* node = BVHNode::generateNode_equal_counts(objs_copy, 0, size, depth);
        return node;
    }


    BVHNode* node = new BVHNode();

    BoundingBox bbox_centers;
    // calculate bounding box for the node
    for (auto obj : objs)
    {
        auto b = obj->getBoundingBox();
        node->bounding_box.merge(b);
        bbox_centers.expand(obj->getCentroid());
    }
    node->bounding_box.cacheInfomation();
    bbox_centers.cacheInfomation();



    // divide the node into n buckets and calculate surface for each bucket
    const int buckets_count = 12;
    auto buckets = std::vector<SAHBucket>(buckets_count);


    // use x,y,z,x,y,z as dividing axis
    int axis = depth % 3;

    node->split_axis = axis;

    // throw every object into each bucket
    float width = bbox_centers.max[axis] - bbox_centers.min[axis];

    //Q_ASSERT(width > 0);
    for (auto obj : objs)
    {
        int pos;
        if (width > 0)
        {
            pos = (obj->getCentroid()[axis] - bbox_centers.min[axis])
                / width * buckets_count;
        }
        else
        {
            pos = 0;
        }
        if (pos >= buckets_count) pos = buckets_count - 1;

        buckets[pos].items.push_back(obj);
        buckets[pos].bbox.merge(obj->getBoundingBox());
    }

    // calculate the relative arbitary intersection cost for each split point
    std::vector<float> costs(buckets_count);
    for (int i = 0; i < buckets_count - 1; i++)
    {
        BoundingBox bbox_l, bbox_r;
        int count_l = 0;
        int count_r = 0;
        for (int j = 0; j <= i; ++j)
        {
            bbox_l.merge(buckets[j].bbox);
            count_l += buckets[j].items.size();
        }
        for (int j = i+1; j < buckets_count; j++)
        {
            bbox_r .merge(buckets[j].bbox);
            count_r += buckets[j].items.size();
        }


        costs[i] = (count_l * bbox_l.getSurfaceArea() + count_r * bbox_r.getSurfaceArea()) /
                node->bounding_box.getSurfaceArea();
    }

    int min_pos = 0;
    float cost_min = costs[0];
    for (int i = 1; i < buckets_count - 1; i++)
    {
        if (costs[i] <  cost_min)
        {
            cost_min = costs[i];
            min_pos = i;
        }
    }

    std::list<Geometry*> left_objs;
    std::list<Geometry*> right_objs;

    for (int i = 0; i < buckets_count; i++ )
    {
        if (i <= min_pos)
        {
            left_objs.splice(left_objs.end(), buckets[i].items);
        }
        else
        {
            right_objs.splice(right_objs.end(), buckets[i].items);
        }
    }

    if(left_objs.size()==0 && right_objs.size()>1)
    {
        left_objs.push_back(right_objs.front());
        right_objs.pop_front();
    }
    else if(right_objs.size()==0 && left_objs.size()>1)
    {
        right_objs.push_front(left_objs.back());
        left_objs.pop_back();
    }


    node->left.reset(BVHNode::generateNode_sah(left_objs, depth + 1));
    node->right.reset(BVHNode::generateNode_sah(right_objs, depth + 1));


    if (node->left && node->right
            && node->left->bounding_box.max[axis] > node->right->bounding_box.min[axis])
        node->interlanced = true;

    return node;
}
