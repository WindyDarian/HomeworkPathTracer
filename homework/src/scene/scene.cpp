#include <scene/scene.h>

#include <scene/geometry/cube.h>
#include <scene/geometry/sphere.h>
#include <scene/geometry/mesh.h>
#include <scene/geometry/square.h>

#include <raytracing/samplers/uniformpixelsampler.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <scene/materials/lambertmaterial.h>
#include <scene/materials/phongmaterial.h>

Scene::Scene()
{
    pixel_sampler.reset(new UniformPixelSampler());
}

void Scene::SetCamera(const Camera &c)
{
    camera = Camera(c);
    camera.create();
    film.SetDimensions(c.width, c.height);
}

void Scene::CreateTestScene()
{
    Material* lambert1 = new LambertMaterial(glm::vec3(1, 0, 0));
    Material* lambert2 = new LambertMaterial(glm::vec3(0, 1, 0));

    Cube* c = new Cube();
    c->material = lambert1;
    c->transform = Transform(glm::vec3(1,0,0), glm::vec3(0,0,45), glm::vec3(1,1,1));
    c->create();
    this->objects.append(c);
    auto b = new BoundingBoxFrame(c->getBoundingBox());
    b->create();
    this->boundingbox_objects.append(b);

    Sphere* s = new Sphere();
    s->material = lambert2;
    s->transform = Transform(glm::vec3(-1,1,0), glm::vec3(0,0,0), glm::vec3(1,2,1));
    s->create();
    this->objects.append(s);
    b = new BoundingBoxFrame(s->getBoundingBox());
    b->create();
    this->boundingbox_objects.append(b);

    camera = Camera(400, 400);
    camera.near_clip = 0.1f;
    camera.far_clip = 100.0f;
    camera.create();
    film = Film(400, 400);

    this->recomputeBVH();

}

void Scene::Clear()
{
    for(Geometry *g : objects)
    {
        delete g;
    }
    objects.clear();
    lights.clear();
    for(Material *m : materials)
    {
        delete m;
    }
    materials.clear();
    camera = Camera();
    film = Film();
    pixel_sampler.reset(new UniformPixelSampler());
    this->bvh.reset();
    for(auto b : this->boundingbox_objects)
    {
        delete b;
    }
    this->boundingbox_objects.clear();
    for(auto b : this->boundingbox_bvh)
    {
        delete b;
    }
    this->boundingbox_bvh.clear();
}

void Scene::recomputeBVH()
{


    // reset() automatically deletes old object and stores new one

    this->bvh.reset(BVHNode::build(this->objects.toStdList(), BVHNode::CurrentSplitMethodSettings));

    std::cout << "BVH node for scene constructed: " << BVHNode::GetCurrentSplitMethodText() << std::endl;


    // update k-d node visualization
    for(auto b : this->boundingbox_bvh)
    {
        delete b;
    }
    this->boundingbox_bvh.clear();
    std::list<BoundingBoxFrame*> b_frame;
    this->bvh->appendBoundingBoxFrame(b_frame, glm::vec3(1.f,1.f,0.5f), 0.95f);
    for (auto b : b_frame)
    {
        b->create();
        this->boundingbox_bvh.push_back(b);
    }
}
