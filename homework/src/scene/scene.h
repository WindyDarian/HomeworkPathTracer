#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <raytracing/samplers/pixelsampler.h>
#include <scene/geometry/geometry.h>
#include <scene/geometry/boundingboxframe.h>
#include <raytracing/kdnode.h>

class Geometry;
class Material;
class KDNode;

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
    PixelSampler* pixel_sampler;
    std::unique_ptr<KDNode> kdnode_objects = nullptr;
    QList<BoundingBoxFrame*> boundingbox_kdnode;


    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();

    void recomputeKDNode();
};
