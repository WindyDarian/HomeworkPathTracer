#include "sphere.h"

#include <iostream>

#include <la.h>

static const int SPH_IDX_COUNT = 2280;  // 760 tris * 3
static const int SPH_VERT_COUNT = 382;


//Add these functions to your sphere.cpp file
//Make sure UniformCodePDF is put at the top of the file; it's not a member of the Sphere class.

float UniformConePdf(float cosThetaMax)
{
    return 1.f / (2.f * PI * (1.f - cosThetaMax));
}

const float p_ellipsoid = 1.6075f;
const float p_ellipsoid_inv = 1.0f / 1.6075f;

void Sphere::ComputeArea()
{
    auto scale = this->transform.getScale();

    float xp = glm::pow(1.f * scale.x, p_ellipsoid);
    float yp = glm::pow(1.f * scale.y, p_ellipsoid);
    float zp = glm::pow(1.f * scale.z, p_ellipsoid);

    // near approxmiate of ellipsoid area
    this->area = PI * 4.f *
            glm::pow((xp * yp + xp * zp + yp * zp) / 3.f,
                     p_ellipsoid_inv);

}


glm::vec3 Sphere::pickSamplePointLocal(std::function<float ()> &randomf)
{
    float theta = randomf() * TWO_PI;
    float phi = glm::acos(2.f * randomf() - 1.f);

    glm::vec3 point_local(
                glm::sin(phi) * glm::cos(theta) * 0.5f,
                glm::sin(phi) * glm::sin(theta) * 0.5f,
                glm::cos(phi) * 0.5f);

    return point_local;
}

Intersection Sphere::pickSampleIntersection(std::function<float ()> randomf, const glm::vec3 *target_point)
{
    glm::vec3 point_world, normal_world, tangent_world;
    while (true)
    {

        glm::vec3 point_local = this->pickSamplePointLocal(randomf);

        glm::vec3 inormal(glm::normalize(point_local));

        //If make sure it samples the front face to illuminated object, the area factor in pdf also needs to be changed
        // to the front area. So I decided not to use that. So in high sample count it will be correct although there maybe
        // lots of noises forr low sample count.
//        if (target_point)
//        {
//            using rejection sampling to make sure samples are on the near side to target as req
//            auto target_local = this->transform.invT() * glm::vec4(*target_point, 1.f);
//            if (glm::dot(inormal, target_local.xyz()) < 0 || fequal(glm::length2(target_local.xyz()), 0.f))
//                continue;
//        }

        glm::vec3 itangent = glm::cross(glm::vec3(0,1,0), inormal);
        if (fequal(glm::length2(itangent), 0.f))
        {
            itangent = glm::vec3(0,0,1);
        }

        point_world = glm::vec3(this->transform.T() * glm::vec4(point_local, 1.f));
        normal_world = glm::normalize(glm::vec3(this->transform.invTransT() * glm::vec4(inormal, 0.f)));

        tangent_world = glm::normalize(glm::vec3(this->transform.invTransT() * glm::vec4(itangent, 0.f)));

        return Intersection(point_world, normal_world, tangent_world,
                            0.f, Material::GetImageColorInterp(this->GetUVCoordinates(point_local), this->material->texture),
                            this);
    }
}

glm::vec3 Sphere::ComputeNormal(const glm::vec3 &P)
{}

Intersection Sphere::GetIntersection(const Ray& r)
{
    Ray r_obj(r.getTransformedCopy(this->transform.invT()));

    float radius = 0.5f;
    glm::vec3 center(0.f);

    float A = glm::length2(r_obj.direction);
    float B = 2 * (glm::dot(r_obj.direction, r_obj.origin - center));
    float C = glm::length2(r_obj.origin - center) - glm::pow(radius, 2.f);

    float disc = glm::pow(B, 2.f) - 4 * A * C;
    if (disc<0) return Intersection(); // B^2-4AC<0 then no intersection

    float t = ( -B + glm::sqrt(disc) ) / (2 * A);
    float temp = ( -B - glm::sqrt(disc) ) / (2 * A);
    if (t >= 0)
    {
        if (temp < t && temp >= 0) t = temp;
    }
    else
        t = temp;

    if (t < 0 ) return Intersection(); // not in the front

    glm::vec3 ipoint(r_obj.origin + t * r_obj.direction);
    glm::vec3 inormal(glm::normalize(ipoint - center));
    glm::vec3 itangent = glm::cross(glm::vec3(0,1,0), inormal);
    if (fequal(glm::length2(itangent), 0.f))
    {
        itangent = glm::vec3(0,0,1);
    }


    glm::vec3 ipoint_world(this->transform.T() * glm::vec4(ipoint, 1.f));

    glm::vec4 normal4_world(this->transform.invTransT() * glm::vec4(inormal,0.f));

    glm::vec3 normal_world( normal4_world );
    normal_world = glm::normalize(normal_world);

    glm::vec3 tangent_world = glm::normalize(glm::vec3(this->transform.invTransT() * glm::vec4(normal_world, 0.f)));
    //bitangent computed in Intersection constructor

    float t_world = glm::dot(ipoint_world - r.origin, r.direction);

    glm::vec3 s_color = Material::GetImageColorInterp(this->GetUVCoordinates(ipoint), this->material->texture);

    return Intersection(ipoint_world, normal_world, tangent_world, t_world, s_color, this);

}

// These are functions that are only defined in this cpp file. They're used for organizational purposes
// when filling the arrays used to hold the vertex and index data.
void createSphereVertexPositions(glm::vec3 (&sph_vert_pos)[SPH_VERT_COUNT])
{
    // Create rings of vertices for the non-pole vertices
    // These will fill indices 1 - 380. Indices 0 and 381 will be filled by the two pole vertices.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 0.5f, 0, 1);
            sph_vert_pos[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole vertices
    sph_vert_pos[0] = glm::vec3(0, 0.5f, 0);
    sph_vert_pos[381] = glm::vec3(0, -0.5f, 0);  // 361 - 380 are the vertices for the bottom cap
}


void createSphereVertexNormals(glm::vec3 (&sph_vert_nor)[SPH_VERT_COUNT])
{
    // Unlike a cylinder, a sphere only needs to be one normal per vertex
    // because a sphere does not have sharp edges.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.0f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 1.0f, 0, 0);
            sph_vert_nor[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole normals
    sph_vert_nor[0] = glm::vec3(0, 1.0f, 0);
    sph_vert_nor[381] = glm::vec3(0, -1.0f, 0);
}


void createSphereIndices(GLuint (&sph_idx)[SPH_IDX_COUNT])
{
    int index = 0;
    // Build indices for the top cap (20 tris, indices 0 - 60, up to vertex 20)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 0;
        sph_idx[index + 1] = i + 1;
        sph_idx[index + 2] = i + 2;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[57] = 0;
    sph_idx[58] = 20;
    sph_idx[59] = 1;
    index += 3;

    // Build indices for the body vertices
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            sph_idx[index] = (i - 1) * 20 + j + 1;
            sph_idx[index + 1] = (i - 1) * 20 +  j + 2;
            sph_idx[index + 2] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 3] = (i - 1) * 20 +  j + 1;
            sph_idx[index + 4] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 5] = (i - 1) * 20 +  j + 21;
            index += 6;
        }
    }

    // Build indices for the bottom cap (20 tris, indices 2220 - 2279)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 381;
        sph_idx[index + 1] = i + 361;
        sph_idx[index + 2] = i + 362;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[2277] = 381;
    sph_idx[2278] = 380;
    sph_idx[2279] = 361;
    index += 3;
}

void Sphere::create()
{
    GLuint sph_idx[SPH_IDX_COUNT];
    glm::vec3 sph_vert_pos[SPH_VERT_COUNT];
    glm::vec3 sph_vert_nor[SPH_VERT_COUNT];
    glm::vec3 sph_vert_col[SPH_VERT_COUNT];

    createSphereVertexPositions(sph_vert_pos);
    createSphereVertexNormals(sph_vert_nor);
    createSphereIndices(sph_idx);
    for (int i = 0; i < SPH_VERT_COUNT; i++) {
        sph_vert_col[i] = material->base_color;
    }

    count = SPH_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(sph_idx, SPH_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(sph_vert_pos, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(sph_vert_col, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(sph_vert_nor, SPH_VERT_COUNT * sizeof(glm::vec3));
}

glm::vec2 Sphere::GetUVCoordinates(const glm::vec3 &point)
{
   float phi = std::atan2(point.z, point.x);
   if (phi < 0) phi += TWO_PI;
   float theta = glm::acos(point.y * 2); //map y to (-1, 1)

   glm::vec2 uv( 1 - phi / TWO_PI, 1 - theta / PI);

   return uv;
}



BoundingBox Sphere::calculateBoundingBox()
{
    // I know this is not accurate when rotating a sphere
    // but due to irregular scale like default scene exists
    // I'm just using a shape to cover the whole sphere for priorty
    BoundingBox b(glm::vec3(-0.5f),glm::vec3(0.5f));
    return b.getTransformedCopy(this->transform.T());
}



float Sphere::RayPDF(const Intersection &isx, const Ray &ray) {
    glm::vec3 Pcenter = transform.position();
    float radius = 0.5f*(transform.getScale().x + transform.getScale().y + transform.getScale().z)/3.0f;
    // Return uniform weight if point inside sphere
    if (glm::distance2(isx.point, Pcenter) - radius*radius < 1e-4f)
        return Geometry::RayPDF(isx, ray);

    // Compute general sphere weight
    float sinThetaMax2 = radius*radius / glm::distance2(isx.point, Pcenter);
    float cosThetaMax = glm::sqrt(glm::max(0.f, 1.f - sinThetaMax2));
    return UniformConePdf(cosThetaMax);
}
