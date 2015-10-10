-Extra Credits



-Bounding Box:

Use View menu to toggle Object bounding box and scene BVH bounding box

 I didn't make BoundingBox itself drawable because
 I was encountering some problem with copy constructor of a drawable
 as it sometimes resets the create() state when a copied instance dies.

    raytracing/boundingbox.h
    scene/gemoetry/boundingboxframe.h


-BVH

My BVH tree structure is in raytracing/KDTree.h
