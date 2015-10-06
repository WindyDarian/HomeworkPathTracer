-Extra Credits



-Bounding Box:

Use View menu to toggle between Object bounding box and scene BVH bounding box

I didn't make BoundingBox itself a Drawable because I decided to use it
as a pure data class like Ray and use its copy constructor a lot, which
Drawable will encounter some problems at. See 

    raytracing/boundingbox.h
    scene/gemoetry/boundingboxframe.h


-BVH

My BVH tree structure is in raytracing/KDTree.h
