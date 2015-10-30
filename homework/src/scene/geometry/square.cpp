#include <scene/geometry/square.h>

void SquarePlane::ComputeArea()
{
    // in this program we only have initial transform.scale that can change size so
    float area_local = 1.f;
    auto scale = transform.getScale();
    area = area_local * scale.x * scale.y;
}

glm::vec3 SquarePlane::ComputeNormal(const glm::vec3 &P)
{
        return glm::vec3(0,0,1);
}


Intersection SquarePlane::GetIntersection(const Ray& r)
{
    Ray r_obj(r.getTransformedCopy(this->transform.invT()));

    glm::vec3 center(0.f, 0.f, 0.f);
    glm::vec3 normal(0.f, 0.f, 1.f);
    float halfside = 1 * 0.5f;

    if (fequal(r_obj.direction.z, 0.f))
        return Intersection();

    // since it's a fixed XY plane, this is a simplified ray-plane intersection
    float t = (center.z - r_obj.origin.z) / r_obj.direction.z;

    if (t < 0) return Intersection();

    glm::vec3 ipoint(r_obj.origin + t * r_obj.direction);

    if (ipoint.x > center.x + halfside || ipoint.x < center.x - halfside)
        return Intersection();
    if (ipoint.y > center.y + halfside || ipoint.y < center.y - halfside)
        return Intersection();

    glm::vec3 ipoint_world(this->transform.T() * glm::vec4(ipoint, 1.f));
    glm::vec4 normal4_world(this->transform.invTransT() * glm::vec4(normal,0.f));

    glm::vec3 normal_world(normal4_world);
    normal_world = glm::normalize(normal_world);

    float t_world = glm::dot(ipoint_world - r.origin, r.direction);
    glm::vec3 s_color = Material::GetImageColorInterp(this->GetUVCoordinates(ipoint), this->material->texture);


    glm::vec4 tangent_o(1,0,0,0);
    glm::vec3 tangent_world = glm::normalize(glm::vec3(this->transform.invTransT() * tangent_o));
    //bitangent computed in Intersection constructor

    return Intersection(ipoint_world, normal_world, tangent_world, t_world, s_color, this);
}

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}

glm::vec2 SquarePlane::GetUVCoordinates(const glm::vec3 &point)
{
    glm::vec2 uv(point.x + 0.5f, point.y + 0.5f);

    return uv;
}

Intersection SquarePlane::pickSampleIntersection(std::function<float ()> randomf, const glm::vec3 *target_point)
{
     float random1 = randomf();
     float random2 = randomf();
     auto point_obj = glm::vec3(random1 - 0.5f, random2 - 0.5f, 0.f);
     auto point = glm::vec3(this->transform.T() *
                      glm::vec4(point_obj, 1.f));
     auto normal = glm::normalize(glm::vec3(this->transform.invTransT() *
                             glm::vec4(0,0,1.f,0.f)));
     auto tangent = glm::normalize(glm::vec3(this->transform.invTransT() *
                             glm::vec4(1.f,0,0,0.f)));
     auto color = Material::GetImageColorInterp(this->GetUVCoordinates(point_obj), this->material->texture);

     return Intersection(point, normal, tangent, 0.f, color, this);
}

BoundingBox SquarePlane::calculateBoundingBox()
{
    BoundingBox b(glm::vec3(-0.5f, -0.5f, 0.0f),
                  glm::vec3(0.5f, 0.5f, 0.0f));
    return b.getTransformedCopy(this->transform.T());
}
