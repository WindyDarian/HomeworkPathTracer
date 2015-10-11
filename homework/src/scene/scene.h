#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <raytracing/samplers/pixelsampler.h>
#include <scene/geometry/geometry.h>
#include <scene/geometry/boundingboxframe.h>
#include <raytracing/bvhnode.h>
#include <memory>

class Geometry;
class Material;
class BVHNode;

class Scene
{
public:
    Scene();
    QList<Geometry*> objects;
    QList<Material*> materials;
    QList<Geometry*> lights;
    QList<BoundingBoxFrame*> boundingbox_objects;
    Camera camera;
    Film film;
    std::unique_ptr<PixelSampler> pixel_sampler;
    std::unique_ptr<BVHNode> bvh = nullptr;
    QList<BoundingBoxFrame*> boundingbox_bvh;


    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();

    void recomputeBVH();
};
