#include <scene/geometry/disc.h>

void Disc::ComputeArea()
{
    auto scale = transform.getScale();
    area = PI * 0.25f * scale.x * scale.y;
}

Intersection Disc::pickSampleIntersection(float random1, float random2)
{
    // sqrt to make sample evenly distributed
    float d = glm::sqrt(random1);
    float angle = random2 * TWO_PI;

    auto point_obj = glm::vec3(d * glm::cos(angle), d * glm::sin(angle), 0.f) * 0.5f;

    auto point = glm::vec3(this->transform.T() *
                     glm::vec4(point_obj, 1.f));
    auto normal = glm::vec3(this->transform.invTransT() *
                            glm::vec4(0,0,1.f,0.f));
    auto tangent = glm::vec3(this->transform.invTransT() *
                            glm::vec4(1.f,0,0,0.f));
    auto color = glm::vec3(this->material->base_color) * Material::GetImageColor(this->GetUVCoordinates(point_obj), this->material->texture);

    return Intersection(point, normal, tangent, 0.f, color, this);

}

BoundingBox Disc::calculateBoundingBox()
{
    BoundingBox b(glm::vec3(-0.5f, -0.5f, 0.0f),
                  glm::vec3(0.5f, 0.5f, 0.0f));
    return b.getTransformedCopy(this->transform.T());

}

Intersection Disc::GetIntersection(const Ray &r)
{
    //Transform the ray
    Ray r_loc = r.getTransformedCopy(transform.invT());
    Intersection result;

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    glm::vec4 P = glm::vec4(t * r_loc.direction + r_loc.origin, 1);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 0.25f)
    {
        result.point = glm::vec3(transform.T() * P);
        result.normal = glm::normalize(glm::vec3(transform.invTransT() * glm::vec4(ComputeNormal(glm::vec3(P)), 0)));
        result.object_hit = this;
        result.t = glm::distance(result.point, r.origin);
        result.color = Material::GetImageColorInterp(GetUVCoordinates(glm::vec3(P)), material->texture);

        result.tangent = glm::normalize(glm::vec3(this->transform.invTransT() * glm::vec4(1,0,0,0)));
        result.bitangent = glm::cross(result.normal, result.tangent);

        return result;
    }
    return result;
}

glm::vec2 Disc::GetUVCoordinates(const glm::vec3 &point)
{
    return glm::vec2(point.x + 0.5f, point.y + 0.5f);
}

glm::vec3 Disc::ComputeNormal(const glm::vec3 &P)
{
    return glm::vec3(0,0,1);
}

void Disc::create()
{
    GLuint idx[54];
    //18 tris, 54 indices
    glm::vec3 vert_pos[20];
    glm::vec3 vert_nor[20];
    glm::vec3 vert_col[20];

    //Fill the positions, normals, and colors
    glm::vec4 pt(0.5f, 0, 0, 1);
    float angle = 18.0f * DEG2RAD;
    glm::vec3 axis(0,0,1);
    for(int i = 0; i < 20; i++)
    {
        //Position
        glm::vec3 new_pt = glm::vec3(glm::rotate(glm::mat4(1.0f), angle * i, axis) * pt);
        vert_pos[i] = new_pt;
        //Normal
        vert_nor[i] = glm::vec3(0,0,1);
        //Color
        vert_col[i] = material->base_color;
    }

    //Fill the indices.
    int index = 0;
    for(int i = 0; i < 18; i++)
    {
        idx[index++] = 0;
        idx[index++] = i + 1;
        idx[index++] = i + 2;
    }

    count = 54;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(idx, 54 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(vert_pos, 20 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(vert_nor, 20 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(vert_col, 20 * sizeof(glm::vec3));
}
