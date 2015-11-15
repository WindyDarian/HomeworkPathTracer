#pragma once
#include <scene/geometry/geometry.h>
#include <scene/geometry/boundingboxframe.h>
#include <openGL/drawable.h>
#include <QList>
#include <set>
#include <raytracing/bvhnode.h>


class Triangle : public Geometry
{
public:
    Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3);
    Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3);
    Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3);
    Intersection GetIntersection(const Ray& r);
     virtual Intersection pickSampleIntersection(std::function<float()> randomf, const glm::vec3* target_normal = nullptr);


    glm::vec3 points[3];
    glm::vec3 normals[3];
    glm::vec2 uvs[3];
    glm::vec3 plane_normal;

    void create();//This does nothing because individual triangles are not rendered with OpenGL;
                            //they are rendered all together in their Mesh.

    virtual glm::vec3 ComputeNormal(const glm::vec3 &P);
    virtual void ComputeArea();

    glm::vec3 GetNormal(const glm::vec3 &position);//Returns the interpolation of the triangle's three normals
                                                    //based on the point inside the triangle that is given.
    glm::vec4 GetNormal(const glm::vec4 &position);

    glm::vec2 GetUVCoordinates(const glm::vec3 &point);
protected:
    virtual BoundingBox calculateBoundingBox();
    virtual glm::vec3 calculateCentroid();


};

//A mesh just holds a collection of triangles against which one can test intersections.
//Its primary purpose is to store VBOs for rendering the triangles in OpenGL.
class Mesh : public Geometry
{
public:

    Intersection GetIntersection(const Ray& r);

    Mesh();
    virtual ~Mesh();
    void SetMaterial(Material *m);
    void create();
    void LoadOBJ(const QStringRef &filename, const QStringRef &local_path);
    void recomputeBVH();
    void recomputeBVH(BVHNode::SplitMethod split_method);

    virtual glm::vec3 ComputeNormal(const glm::vec3 &P);
    virtual void ComputeArea();
    virtual Intersection pickSampleIntersection(std::function<float()> randomf, const glm::vec3* target_normal = nullptr);

    void createVisibleBoundingBoxes();
    std::list<BoundingBoxFrame*> &getVisibleBoundingBoxesBVH();
    std::list<BoundingBoxFrame*> &getVisibleBoundingBoxesTriangle();

    glm::vec2 GetUVCoordinates(const glm::vec3 &point);
protected:
    virtual BoundingBox calculateBoundingBox();
    virtual glm::vec3 calculateCentroid();

private:
    QList<Triangle*> faces;


    std::unique_ptr<BVHNode> bvh;  // K-D Node for optimization
    std::unique_ptr<std::list<BoundingBoxFrame*>> visible_bounding_boxes_bvh = nullptr;
    std::unique_ptr<std::list<BoundingBoxFrame*>> visible_bounding_boxes_triangle = nullptr;

    void createVisibleBoundingBoxes_bvh();
    void createVisibleBoundingBoxes_triangle();

    QList<float> tri_areas;
    std::unique_ptr<std::piecewise_constant_distribution<float>> tri_area_distribution;

};
