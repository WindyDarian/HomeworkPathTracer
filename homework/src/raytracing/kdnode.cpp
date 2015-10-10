#include "kdnode.h"

#include <algorithm>
#include <memory>

// Just ignore this for my hw02
// Because I wanted to render the dragon.obj after hw01 is done
// it helped my Wahoo

KDNode::KDNode()
{

}

//struct CenterComparasion
//{
//    int axis;

//    CenterComparasion(int axis)
//    {
//        this->axis = axis;
//    }
//    bool operator()(Geometry* a,
//                    Geometry*b) const
//    {
//        return a->getBoundingBox().getCenter()[axis] <
//                b->getBoundingBox().getCenter()[axis];
//    }
//};


Intersection KDNode::GetIntersection(const Ray &r) const
{
//    auto p = KDNode::calculateIntersection(r);
//    if (p && p->t >= 0)
//    {
//        return Intersection(*p);
//    }
//    else
//    {
//        return Intersection();
//    }

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
        //in case bounding box of left and right child overlapped
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
    if (split_method == SPLIT_EQUAL_COUNTS )
    {
        std::list<Geometry *> copied_list(objs);
        return KDNode::generateNode_equal_counts(copied_list, 0);
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

KDNode *KDNode::generateNode_equal_counts(std::list<Geometry *> &objs, int depth)
{
    //  empty
    if (objs.size() == 0) return nullptr;

    KDNode* node = new KDNode();

    //  contains unique node
    if (objs.size() == 1)
    {
        node->obj = objs.front();
        node->bounding_box = objs.front()->getBoundingBox();
        return node;
    }


    // use x,y,z,x,y,z as dividing axis
    int axis = depth % 3;

    node->split_axis = axis;

    float split_plane = 0;

    std::vector<float> centers;
    centers.reserve(objs.size());

    for (auto obj: objs)
    {
        auto b = obj->getBoundingBox();
        node->bounding_box.merge(b);
        centers.push_back(b.getCenter()[axis]);
    }
    node->bounding_box.cacheCenter();



    int median_index = objs.size() / 2;
    std::nth_element(centers.begin(),
                     centers.begin() + median_index,
                     centers.end()
                     );
    split_plane = centers[median_index];



    // start dividing
    std::list<Geometry*> left_objs;
    std::list<Geometry*> right_objs;
    for (auto obj: objs)
    {
        auto b = obj->getBoundingBox();


        if (b.getCenter()[axis] < split_plane)
        {
            left_objs.push_back(obj);
        }
        else
        {
            right_objs.push_back(obj);
        }

    }

    if(left_objs.size()==0 && right_objs.size()>1)
    {
        Geometry* swapping_obj = nullptr;
        for(auto o: right_objs)
        {
            if (o->getBoundingBox().getCenter()[axis] <= split_plane)
            {
                swapping_obj = o;
                break;
            }
        }
        if (swapping_obj)
        {
            left_objs.push_back(swapping_obj);
            right_objs.remove(swapping_obj);
        }
    }
    else if(right_objs.size()==0 && left_objs.size()>1)
    {
        Geometry* swapping_obj = nullptr;
        for(auto o: left_objs)
        {
            if (o->getBoundingBox().getCenter()[axis] >= split_plane)
            {
                swapping_obj = o;
                break;
            }
        }
        if (swapping_obj)
        {
            right_objs.push_back(swapping_obj);
            left_objs.remove(swapping_obj);
        }
    }

    node->left = std::unique_ptr<KDNode>(KDNode::generateNode_equal_counts(left_objs, depth+1));
    node->right = std::unique_ptr<KDNode>(KDNode::generateNode_equal_counts(right_objs, depth+1));

    if (node->left && node->right
            && node->left->bounding_box.max[axis] > node->right->bounding_box.min[axis])
        node->interlanced = true;

    return node;

}

//Intersection* KDNode::calculateIntersection(const Ray &r) const
//{
//    if ((!this->left) && this->right)
//        return this->right->calculateIntersection(r);
//    if ((!this->right) && this->left)
//        return this->left->calculateIntersection(r);

//    if (!this->bounding_box.isIntersected(r))
//        return nullptr;

//    if (this->obj)
//    {
//        return new Intersection(obj->GetIntersection(r));
//    }

//    if (!interlanced)
//    {
//        if (r.origin[split_axis] <= left->bounding_box.max[split_axis])
//        {
//            auto result = left->calculateIntersection(r);
//            if (result && result->t >= 0) return result;
//            return right->calculateIntersection(r);
//        }
//        else
//        {
//            auto result = right->calculateIntersection(r);
//            if (result && result->t >= 0) return result;
//            return left->calculateIntersection(r);
//        }
//    }
//    else
//    {
//        //in case bounding box of left and right child overlapped
//        auto il = left->calculateIntersection(r);
//        auto ir = right->calculateIntersection(r);

//        if (!il || il->t < 0)
//            return ir;
//        else if (!ir || ir->t < 0)
//            return il;
//        else
//        {
//             return (il->t > ir->t)? ir : il;
//        }
//    }
//}


