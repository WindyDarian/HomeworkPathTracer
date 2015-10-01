#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

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

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &position)
{
    float d0 = glm::distance(position, points[0]);
    float d1 = glm::distance(position, points[1]);
    float d2 = glm::distance(position, points[2]);
    return (normals[0] * d0 + normals[1] * d1 + normals[2] * d2)/(d0+d1+d2);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}



glm::vec2 Triangle::GetUVCoordinates(const glm::vec3 &point)
{
    float S = tri_area(points[0], points[1], points[2]);
    float S1 = tri_area(point, points[1], points[2]);
    float S2 = tri_area(point, points[2], points[0]);
    float S3 = tri_area(point, points[0], points[1]);
    return this->uvs[0] * S1/S + this->uvs[1] * S2/S + this->uvs[2] * S3/S;
}

//HAVE THEM IMPLEMENT THIS
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r)
{
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

    glm::vec3 s_color = glm::vec3(this->material->base_color) * Material::GetImageColor(this->GetUVCoordinates(ipoint), this->material->texture);

    return Intersection(ipoint,
                        this->GetNormal(ipoint),
                        t,
                        s_color,
                        this);
}

BoundingBox Triangle::getBoundingBox()
{
    return BoundingBox(this->points[0], this->points[1], this->points[2]);
}

Intersection Mesh::GetIntersection(Ray r)
{
    Ray r_obj(r.GetTransformedCopy(this->transform.invT()));

    float min_t = std::numeric_limits<float>::max();
    Intersection result;

    // Optimization: using K-D Node
    std::set<Geometry*> possible_objs;
    this->kdnode->appendIntersections(possible_objs, r_obj);

    for (auto tri: possible_objs)
    {
        Intersection intersection = tri->GetIntersection(r_obj);
        if (intersection.object_hit == NULL)
            continue;   // missed this triangle

        if (intersection.t < min_t)
        {
            min_t = intersection.t;
            result = intersection;
        }
    }


    /* //not using K-D Node
    for (auto tri : this->faces)
    {
        Intersection intersection = tri->GetIntersection(r_obj);
        if (intersection.object_hit == NULL)
            continue;   // missed this triangle

        if (intersection.t < min_t)
        {
            min_t = intersection.t;
            result = intersection;
        }
    }
    */

    if (result.object_hit == NULL || result.object_hit == nullptr)
        return Intersection();

    glm::vec3 ipoint_world(this->transform.T() * glm::vec4(result.point, 1.f));
    glm::vec4 normal4_world(this->transform.invTransT() * glm::vec4(result.normal,0.f));

    glm::vec3 normal_world( normal4_world );
    normal_world = glm::normalize(normal_world);

    float t_world = glm::dot(ipoint_world - r.origin, r.direction);

    return Intersection(ipoint_world, normal_world, t_world, result.surface_color, this);
}

void Mesh::RecomputeKDNode()
{
    std::list<Geometry*> tris;
    for (auto tri: this->faces)
    {
        tris.push_back(tri);
    }
    this->kdnode = std::unique_ptr<KDNode>(KDNode::build(tris));
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
        this->RecomputeKDNode();
        std::cout << "K-D Node for mesh computed" << std::endl;
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