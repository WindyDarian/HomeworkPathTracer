#include "kdnode.h"


// Just ignore this for my hw02
// Because I wanted to render the dragon.obj after hw01 is done
// it helped my Wahoo

KDNode::KDNode()
{

}

Intersection KDNode::GetIntersection(Ray r) const
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

KDNode* KDNode::build(const std::list<Geometry *> & objs, int depth)
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
    int axis = depth % 6;
    bool median_lr = axis >= 3? true: false ;
          //"true" for using right point to calcute median, "false" for using left point
    axis %= 3;
    node->split_axis = axis;

    float median = 0;

    for (auto obj: objs)
    {
        auto b = obj->getBoundingBox();
        node->bounding_box.merge(b);

        if (median_lr) median += b.max[axis]; //right
        else median += b.min[axis]; //left
    }
    median /= static_cast<float>(objs.size());



    // start dividing
    std::list<Geometry*> left_objs;
    std::list<Geometry*> right_objs;
    for (auto obj: objs)
    {
        auto b = obj->getBoundingBox();
        if (median_lr)
        {
             if (b.max[axis] > median) right_objs.push_back(obj);
             else left_objs.push_back(obj);
        }
        else
        {
            if (b.min[axis] < median) left_objs.push_back(obj);
            else right_objs.push_back(obj);
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

    node->left = std::unique_ptr<KDNode>(KDNode::build(left_objs, depth+1));
    node->right = std::unique_ptr<KDNode>(KDNode::build(right_objs, depth+1));

    if (node->left && node->right
            && node->left->bounding_box.max[axis] > node->right->bounding_box.min[axis])
        node->interlanced = true;

    return node;

}


void KDNode::appendIntersections(std::set<Geometry*>& hitset, const Ray& r)
{
    if (!this->bounding_box.isIntersected(r))
        return;

    if (this->left != nullptr)
        this->left->appendIntersections(hitset, r);

    if (this->right != nullptr)
        this->right->appendIntersections(hitset, r);

    if (this->obj != nullptr)
    {
        hitset.insert(this->obj);
    }
}

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


