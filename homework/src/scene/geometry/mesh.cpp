#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>
#include <ctime>

// triangle area
inline float tri_area(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p2-p1,p3-p1)) / 2;
}


void Triangle::ComputeArea()
{
    // Since local area is not needed and not called,
    // I just calculate (and store for weighted sampling)
    // and sum them in mesh::ComputeArea()
    area = tri_area(points[0], points[1], points[2]);
}

void Mesh::ComputeArea()
{
    area = 0.f;
    auto size = this->faces.count();

    this->tri_areas.clear();
    this->tri_areas.reserve(size);

    for(int i = 0; i < size; i++)
    {
        auto tri = faces[i];
        auto p0w = glm::vec3(this->transform.T() * glm::vec4(tri->points[0],1.f));
        auto p1w = glm::vec3(this->transform.T() * glm::vec4(tri->points[1],1.f));
        auto p2w = glm::vec3(this->transform.T() * glm::vec4(tri->points[2],1.f));
        float area_tri = tri_area(p0w, p1w, p2w);

        this->tri_areas.append(area_tri);

        area += area_tri;
    }

    // generate weight distribution for triangle areas
    std::vector<float> indices;
    indices.reserve(faces.size());
    for (int i = 0; i <= faces.size(); i++) indices.push_back(i);
    this->tri_area_distribution.reset(
                new std::piecewise_constant_distribution<float>(indices.begin(), indices.end(), tri_areas.begin()));
}

Intersection Mesh::pickSampleIntersection(std::function<float ()> randomf, const glm::vec3 *target_normal)
{
    // first pick a triangle using weighted destribution
    int index = static_cast<int>((*this->tri_area_distribution)(*mersenne_generator_ptr));
    if (index >= faces.size()) index = faces.size() - 1; //although this not gonna happen
    Triangle* random_triangle = this->faces[index];

    // get the intersection from the weighted sampled triangle
    Intersection result_local = random_triangle->pickSampleIntersection(randomf, nullptr);


    glm::vec3 ipoint_world(this->transform.T() * glm::vec4(result_local.point, 1.f));
    glm::vec4 normal4_world(this->transform.invTransT() * glm::vec4(result_local.normal,0.f));

    glm::vec3 normal_world( normal4_world );
    normal_world = glm::normalize(normal_world);

    glm::vec3 tangent_world = glm::normalize(glm::vec3(this->transform.invTransT() * glm::vec4(result_local.tangent, 0)));

    glm::vec3 s_color = Material::GetImageColorInterp(result_local.object_hit->GetUVCoordinates(result_local.point), this->material->texture);

    return Intersection(ipoint_world, normal_world, tangent_world, 0.f , s_color, this);

}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }


}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    if (plane_normal.x != plane_normal.x || plane_normal.y!= plane_normal.y|| plane_normal.z != plane_normal.z)
        this->tri_valid = false;
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

float Area(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &P)
{
    float A = Area(points[0], points[1], points[2]);
    float A0 = Area(points[1], points[2], P);
    float A1 = Area(points[0], points[2], P);
    float A2 = Area(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}


glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}



glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point)
{
    if (!tri_valid)
        return glm::vec2(0,0);
    float S = tri_area(points[0], points[1], points[2]);
    float S1 = tri_area(point, points[1], points[2]);
    float S2 = tri_area(point, points[2], points[0]);
    float S3 = tri_area(point, points[0], points[1]);
    if (fequal(S,0.f))
        return glm::vec2(0,0);

    auto UV = this->uvs[0] * S1/S + this->uvs[1] * S2/S + this->uvs[2] * S3/S;
    Q_ASSERT(UV.x>= 0 && UV.y >= 0);
    return UV ;
}

BoundingBox Triangle::calculateBoundingBox()
{
    return BoundingBox(this->points[0], this->points[1], this->points[2]);
}

glm::vec3 Triangle::calculateCentroid()
{
    return (this->points[0] + this->points[1] + this->points[2])/3.f;
}

//HAVE THEM IMPLEMENT THIS
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(const Ray &r)
{
    if (!tri_valid)
        return Intersection();

    glm::vec3 v01(this->points[1] - this->points[0]);
    glm::vec3 v12(this->points[2] - this->points[1]);
    glm::vec3 v20(this->points[0] - this->points[2]);

    // this normal is just for getting intersection point
    glm::vec3 normal(plane_normal);
    //glm::vec3 normal(glm::cross(v01, v12));

    float direction_dot_normal = glm::dot(r.direction, normal);

    if (fequal(direction_dot_normal, 0.f))  // parallel
            return Intersection();

    float t = (glm::dot(this->points[0] - r.origin, normal))
            / direction_dot_normal;

    if (t <= 0) return Intersection();

    // intersection point of ray and plane
    glm::vec3 ipoint(r.origin + t * r.direction);

    // check if point in triangle
    glm::vec3 v0p(ipoint - this->points[0]);
    glm::vec3 v1p(ipoint - this->points[1]);
    glm::vec3 v2p(ipoint - this->points[2]);

    if (glm::dot(glm::cross(v01,v0p), normal) < 0 ||
        glm::dot(glm::cross(v12,v1p), normal) < 0 ||
        glm::dot(glm::cross(v20,v2p), normal) < 0)
    {
        // outside of triangle
        return Intersection();
    }

    glm::vec3 inormal = this->GetNormal(ipoint);
    glm::vec3 itangent = this->computeLocalTangent(this->points[0], this->points[1], this->points[2]);

    Q_ASSERT(ipoint.x == ipoint.x);

    return Intersection(ipoint,
                        inormal,
                        itangent,
                        t,
                        glm::vec3(1.f),
                        this);
}

Intersection Triangle::pickSampleIntersection(std::function<float ()> randomf, const glm::vec3 *target_normal)
{
    // sample a random point on a triangle
    glm::vec3 v1 = points[1] - points[0];
    glm::vec3 v2 = points[2] - points[0];
    float r1 = randomf();
    float r2 = randomf();
    if (r1 + r2 > 1.0f)
    {
        r1 = 1.0f - r1;
        r2 = 1.0f - r2;
    }
    glm::vec3 ipoint = points[0] + v1 * r1 + v2 * r2;

    glm::vec3 inormal = this->GetNormal(ipoint);
    glm::vec3 itangent = this->computeLocalTangent(this->points[0], this->points[1], this->points[2]);



    return Intersection(ipoint,
                        inormal,
                        itangent,
                        0.f,
                        glm::vec3(1.f),
                        this);
}

Intersection Mesh::GetIntersection(const Ray &r)
{
    Ray r_obj(r.getTransformedCopy(this->transform.invT()));

    Intersection result;

    if (BVHNode::BVHIntersectionDisabled)
    {
        float min_t = std::numeric_limits<float>::max();
        for (auto tri : this->faces)
        {
            Intersection intersection = tri->GetIntersection(r_obj);
            if (intersection.object_hit == NULL)
                continue;   // missed this triangle
            if (intersection.t < min_t && intersection.t >= 0)
            {
                min_t = intersection.t;
                result = intersection;
            }
        }
    }
    else
    {
        result = this->bvh->GetIntersection(r_obj);
    }


    if (result.object_hit == NULL || result.object_hit == nullptr || result.t < 0)
        return Intersection();

    glm::vec3 ipoint_world(this->transform.T() * glm::vec4(result.point, 1.f));
    glm::vec4 normal4_world(this->transform.invTransT() * glm::vec4(result.normal,0.f));

    glm::vec3 normal_world( normal4_world );
    normal_world = glm::normalize(normal_world);

    glm::vec3 tangent_world = glm::normalize(glm::vec3(this->transform.invTransT() * glm::vec4(result.tangent, 0)));

    float t_world = glm::dot(ipoint_world - r.origin, r.direction);

    Q_ASSERT(t_world==t_world);

    glm::vec3 s_color = Material::GetImageColorInterp(result.object_hit->GetUVCoordinates(result.point), this->material->texture);


    return Intersection(ipoint_world, normal_world, tangent_world, t_world, s_color, this);
}

Mesh::Mesh():Geometry()
{

}

Mesh::~Mesh()
{
    if(this->visible_bounding_boxes_bvh)
    {
        for (auto p: *this->visible_bounding_boxes_bvh)
        {
            delete p;
        }
        this->visible_bounding_boxes_bvh->clear();
        this->visible_bounding_boxes_bvh.reset();
    }
    if(this->visible_bounding_boxes_triangle)
    {
        for (auto p: *this->visible_bounding_boxes_triangle)
        {
            delete p;
        }
        this->visible_bounding_boxes_triangle->clear();
        this->visible_bounding_boxes_triangle.reset();
    }
}

void Mesh::recomputeBVH()
{
    this->recomputeBVH(BVHNode::CurrentSplitMethodSettings);

    std::cout << "BVH node for mesh constructed:" << BVHNode::GetCurrentSplitMethodText() << std::endl;
}

void Mesh::recomputeBVH(BVHNode::SplitMethod split_method)
{
    std::list<Geometry*> tris;
    for (auto tri: this->faces)
    {
        tris.push_back(tri);
    }
    this->bvh = std::unique_ptr<BVHNode>(BVHNode::build(tris, split_method));
}

glm::vec3 Mesh::ComputeNormal(const glm::vec3 &P)
{

}

std::list<BoundingBoxFrame*> &Mesh::getVisibleBoundingBoxesBVH()
{
    return *this->visible_bounding_boxes_bvh;
}

std::list<BoundingBoxFrame *> &Mesh::getVisibleBoundingBoxesTriangle()
{
    if (!this->visible_bounding_boxes_triangle)
    {
        createVisibleBoundingBoxes();
    }
    return *this->visible_bounding_boxes_triangle;
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}


void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
        this->recomputeBVH();
        this->createVisibleBoundingBoxes_bvh();
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

glm::vec2 Mesh::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2();
}

BoundingBox Mesh::calculateBoundingBox()
{
    BoundingBox b;

    //getting bounding box in world space
    for (auto t: this->faces)
    {
        for (int i=0;i<3;i++)
            b.expand(glm::vec3(this->transform.T() * glm::vec4(t->points[i], 1.f)));
    }

    b.cacheInfomation();
    return b;
}

glm::vec3 Mesh::calculateCentroid()
{
    glm::vec3 result(0.f);
    //getting bounding box in world space
    for (auto t: this->faces)
    {
        result += t->getCentroid();
    }
    result/= this->faces.size();

    return glm::vec3(this->transform.T() * glm::vec4(result, 1.f));

}

void Mesh::createVisibleBoundingBoxes_bvh()
{

    if (!this->visible_bounding_boxes_bvh)
    {
        this->visible_bounding_boxes_bvh.reset(new std::list<BoundingBoxFrame*>());
        this->bvh->appendBoundingBoxFrame(*this->visible_bounding_boxes_bvh, glm::vec3(0.5f,1.0f,1.0f), 0.95f, 5);
        for (auto b: *this->visible_bounding_boxes_bvh)
        {
            b->create();
        }
    }
}

void Mesh::createVisibleBoundingBoxes_triangle()
{
    if(!this->visible_bounding_boxes_triangle)
    {
        this->visible_bounding_boxes_triangle.reset(new std::list<BoundingBoxFrame*>());
        for (auto t: this->faces)
        {
            auto b = new BoundingBoxFrame(*t->bounding_box_ptr);
            b->create();
            this->visible_bounding_boxes_triangle->push_back(b);
        }
    }
}

void Mesh::createVisibleBoundingBoxes()
{
    this->createVisibleBoundingBoxes_bvh();
    this->createVisibleBoundingBoxes_triangle();
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}

glm::vec3 Triangle::ComputeNormal(const glm::vec3 &P)
{
}
