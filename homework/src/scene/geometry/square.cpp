#include <scene/geometry/square.h>

Intersection SquarePlane::GetIntersection(Ray r)
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
    glm::vec3 s_color = glm::vec3(this->material->base_color) * Material::GetImageColor(this->GetUVCoordinates(ipoint), this->material->texture);

//    if (this->material->normal_map != NULL)
//    {
//        glm::normal_local = Material::GetImageColor(this->GetUVCoordinates(ipoint), this->material->normal_map) * 2 - glm::vec3(1.f);


//    }

    return Intersection(ipoint_world, normal_world, t_world, s_color, this);
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

BoundingBox SquarePlane::calculateBoundingBox()
{
    BoundingBox b(glm::vec3(-0.5f, -0.5f, 0.0f),
                  glm::vec3(0.5f, 0.5f, 0.0f));
    return b.getTransformedCopy(this->transform.T());
}
