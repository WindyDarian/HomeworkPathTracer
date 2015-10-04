#include "cube.h"
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

Intersection Cube::GetIntersection(Ray r)
{
    Ray r_obj(r.getTransformedCopy(this->transform.invT()));

    float bounds[3][2] = {{-0.5, 0.5}, {-0.5, 0.5}, {-0.5, 0.5}};

    float tnear = std::numeric_limits<float>::lowest();
    float tfar = std::numeric_limits<float>::max();

    for (int i=0;i<3;i++) // x slab then y then z
    {
        float t0, t1;

        if (fequal(r_obj.direction[i], 0.f))
        {
            if (r_obj.origin[i] < bounds[i][0] || r_obj.origin[i] > bounds[i][1])
                return Intersection();
            else
            {
                t0 = std::numeric_limits<float>::lowest();
                t1 = std::numeric_limits<float>::max();
            }
        }
        else
        {
            t0 = (bounds[i][0] - r_obj.origin[i]) / r_obj.direction[i];
            t1 = (bounds[i][1] - r_obj.origin[i]) / r_obj.direction[i];
        }
        tnear = glm::max(tnear, glm::min(t0, t1));
        tfar = glm::min(tfar, glm::max(t0, t1));
    }

    if (tfar < tnear) return Intersection(); // no intersection

    if (tfar < 0) return Intersection(); // behind origin of ray

    float t = (tnear >= 0)? tnear: tfar;

    glm::vec3 ipoint(r_obj.origin + t * r_obj.direction);

    glm::vec3 ipoint_world(this->transform.T() * glm::vec4(ipoint, 1.f));
    glm::vec4 normal4_world(this->transform.invTransT() * glm::vec4(this->getNormal(ipoint),0.f) );

    glm::vec3 normal_world(normal4_world);
    normal_world = glm::normalize(normal_world);

    float t_world = glm::dot(ipoint_world - r.origin, r.direction);

    glm::vec3 s_color = glm::vec3(this->material->base_color) * Material::GetImageColor(this->GetUVCoordinates(ipoint), this->material->texture);

    return Intersection(ipoint_world, normal_world, t_world, s_color, this);
}

glm::vec3 Cube::getNormal(const glm::vec3& point) const
{
    auto axis_and_sign = this->getFaceNormalAxisAndDirection(point);

    glm::vec3 normal(0.f);
    normal[axis_and_sign.first] = 1;
    normal *= axis_and_sign.second;

    return normal;
}

std::pair<int, int> Cube::getFaceNormalAxisAndDirection(const glm::vec3 &point) const
{
    float d_min = std::numeric_limits<float>::max();
    int normal_axis = 0; // axis for normal, 0 for x, 1 for y, 2 for z;
    int normal_sign = 0;

    for(int i = 0; i < 3; i++)
    {
        float d_n = glm::abs(point[i] - bounds[i][0]);
        float d_p = glm::abs(point[i] - bounds[i][1]);

        if (d_n < d_min)
        {
            d_min = d_n;
            normal_axis = i;
            normal_sign = -1;
        }
        if (d_p < d_min)
        {
            d_min = d_p;
            normal_axis = i;
            normal_sign = 1;
        }
    }
    return std::pair<int, int>(normal_axis, normal_sign);
}


//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point)
{
    glm::vec2 p1(0.f);
    glm::vec2 d, o;
    glm::vec3 shift(0.5f, 0.5f, 0.5f);

    auto axis_and_direction = this->getFaceNormalAxisAndDirection(point);

    switch (axis_and_direction.first)
    {
     case 0: //x slab
        p1.y = 0.0f;
        o = (point + shift).yz();
        if (axis_and_direction.second > 0)
        {
            p1.x = 0.75f;
            d = glm::vec2(-1.0f, 1.0f);

        }
        else
        {
            p1.x = 0.0f;
            d = glm::vec2(1.0f, 1.0f);
        }
        break;
    case 1: //y slab
        p1.x = 0.25f;
        o = (point + shift).xz();
        if (axis_and_direction.second > 0)
        {
            p1.y = 0.5f;
            d = glm::vec2(1.0f, -1.0f);
        }
        else
        {
            p1.y = 0.75f;
            d = glm::vec2(1.0f, 1.0f);
        }
        break;
    case 2: //z slab
        p1.x = 0.25f;
        o = (point + shift).xy();
        if (axis_and_direction.second > 0)
        {
            p1.y = 0.0f;
            d = glm::vec2(1.0f, 1.0f);
        }
        else
        {
            p1.y = 0.75f;
            d = glm::vec2(1.0f, -1.0f);
        }
    }

    o = glm::clamp(o, 0.0f, 1.0f);
    d *= 0.25f;
    d.x *= o.x;
    d.y *= o.y;

    return p1 + d;
}

BoundingBox Cube::calculateBoundingBox()
{
    BoundingBox b(glm::vec3(-0.5f),glm::vec3(0.5f));
    return b.getTransformedCopy(this->transform.T());
}
