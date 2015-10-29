#include <scene/scene.h>

#include <scene/geometry/cube.h>
#include <scene/geometry/sphere.h>
#include <scene/geometry/mesh.h>
#include <scene/geometry/square.h>

#include <raytracing/samplers/uniformpixelsampler.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <scene/materials/bxdfs/lambertBxDF.h>

Scene::Scene()
{
}

void Scene::SetCamera(const Camera &c)
{
    camera = Camera(c);
    camera.create();
    film.SetDimensions(c.width, c.height);
}

void Scene::CreateTestScene()
{
  Material* diffuse1 = new Material(glm::vec3(1, 0, 0));
  Material* diffuse2 = new Material(glm::vec3(0, 1, 0));
  BxDF* lambertbxdf = new LambertBxDF(glm::vec3(1,1,1));
  diffuse1->bxdfs.append(lambertbxdf);
  diffuse2->bxdfs.append(lambertbxdf);

  Cube* c = new Cube();
  c->material = diffuse1;
  c->transform = Transform(glm::vec3(1,0,0), glm::vec3(0,0,45), glm::vec3(1,1,1));
  c->create();
  this->objects.append(c);

  Sphere* s = new Sphere();
  s->material = diffuse2;
  s->transform = Transform(glm::vec3(-1,1,0), glm::vec3(0,0,0), glm::vec3(1,2,1));
  s->create();
  this->objects.append(s);

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
    for(BxDF* b : bxdfs)
    {
        delete b;
    }
    bxdfs.clear();
    camera = Camera();
    film = Film();
    
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
