#include "kdnode.h"

// Just ignore this for my hw02
// Because I wanted to render the dragon.obj after hw01 is done
// it helped my Wahoo

KDNode::KDNode()
{

}

KDNode* KDNode::build(const std::list<Geometry *> & objs, int depth, int max_depth)
{   
    //  empty
    if (objs.size() == 0) return nullptr;

    KDNode* node = new KDNode();

    //  contains unique node
    if (objs.size() == 1)
    {
        node->obj.push_back(objs.front());
        node->bounding_box = objs.front()->getBoundingBox();
        return node;
    }


    // use x,y,z,x,y,z as dividing axis
    int axis = depth % 6;
    bool median_lr = axis >= 3? true: false ;
          //"true" for using right point to calcute median, "false" for using left point
    axis %= 3;


    float median = 0;

    for (auto obj: objs)
    {
        BoundingBox b = obj->getBoundingBox();
        node->bounding_box.merge(b);

        if (median_lr) median += b.max[axis]; //right
        else median += b.min[axis]; //left
    }
    median /= static_cast<float>(objs.size());

    if (depth == max_depth)
    {
        node->obj = objs;
        return node;
    }

    // start dividing
    std::list<Geometry*> left_objs;
    std::list<Geometry*> right_objs;
    for (auto obj: objs)
    {
        BoundingBox b = obj->getBoundingBox();
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

    if (this->obj.size()!= 0)
    {
        for (auto o : this->obj){
            hitset.insert(o);
        }
    }
}
