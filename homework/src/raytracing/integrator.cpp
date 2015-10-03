#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{
    //float accurancy = std::numeric_limits<float>::min() * 10000;
    float accurancy = 0.0001f;


    if (depth >= this->max_depth)
        return glm::vec3(0,0,0);

    auto intersection = this->intersection_engine->GetIntersection(r);
    auto o_hit = intersection.object_hit;

    if (intersection.object_hit == nullptr)
        return  glm::vec3(0,0,0);

    if (intersection.object_hit->material->emissive)
        return intersection.color;

    // out ray origins to prevent float problem
    glm::vec3 out_origin_shadowtest = intersection.point + intersection.normal * accurancy;
    glm::vec3 out_origin_outside = intersection.point + intersection.normal * accurancy;
    glm::vec3 out_origin_inside = intersection.point - intersection.normal * accurancy;


    glm::vec3 reflected_color;
    glm::vec3 local_illumination(0.f);


    bool transparent_flag;
    if (o_hit->material->refract_idx_in > 0 && o_hit->material->refract_idx_out >0)
    {
        // transparent
        transparent_flag = true;

        float enter_or_exit =  glm::dot(r.direction, intersection.normal);

        if (enter_or_exit > 0)
        {
            // feeler ray exit (or light in)
            Ray reflect_ray = Ray(out_origin_inside, glm::reflect(r.direction, -intersection.normal));
            reflected_color = TraceRay(reflect_ray, depth + 1) * intersection.color;


            glm::vec3 refraction = glm::refract(r.direction, -intersection.normal, o_hit->material->refract_idx_in / o_hit->material->refract_idx_out);
            if (glm::length2(refraction) < accurancy)
            {
                //total internal reflection
                local_illumination = reflected_color;
            }
            else
            {
                Ray refraction_ray = Ray(out_origin_outside, refraction);
                local_illumination = TraceRay(refraction_ray, depth + 1) * intersection.color;
            }
        }
        else
        {
            // feeler ray in (or light out)

            Ray reflect_ray = Ray(out_origin_outside, glm::reflect(r.direction, intersection.normal));
            reflected_color = TraceRay(reflect_ray, depth + 1) * intersection.color;


            glm::vec3 refraction = glm::refract(r.direction, intersection.normal, o_hit->material->refract_idx_out / o_hit->material->refract_idx_in);
            Ray refraction_ray = Ray(out_origin_inside, refraction);

            //local_illumination = glm::vec3(0,0,0);
            local_illumination = TraceRay(refraction_ray, depth + 1) * intersection.color;
        }


    }
    else
    {
        // opaque
        transparent_flag = false;
        Ray out_ray = Ray(out_origin_outside, glm::reflect(r.direction, intersection.normal));
        reflected_color = TraceRay(out_ray, depth + 1) * intersection.color;

        local_illumination = intersection.color;  //later: * EvaluateReflectedEnergy * shadow afterwards
    }

    float kr = intersection.object_hit->material->reflectivity;


    glm::vec3 color_sum(0.f);
    for (auto light:this->scene->lights)
    {
        if (!transparent_flag)
        {

            color_sum += kr * reflected_color +
                    (1.f - kr) * local_illumination
                    * ShadowTest(out_origin_shadowtest, light)
                    * o_hit->material->EvaluateReflectedEnergy(intersection,
                                                               -r.direction,
                                                               glm::normalize(intersection.point - light->transform.position())
                                                               );
        }
        else
        {
            color_sum += kr * reflected_color + (1.f - kr) * local_illumination;
        }
    }
    int lightnum = this->scene->lights.size();
    glm::vec3 color = (lightnum > 0)? color_sum / static_cast<float>(lightnum): glm::vec3(0.f);

    return color;

}

glm::vec3 Integrator::ShadowTest(const glm::vec3 &point, Geometry* light) const
{
    Ray lightfeeler(point, light->transform.position() - point);
    Intersection i_lightfeeler = this->intersection_engine->GetIntersection(lightfeeler);
    if (i_lightfeeler.object_hit == light) //|| i_lightfeeler.object_hit == nullptr||i_lightfeeler.object_hit == NULL)
    {
        return i_lightfeeler.color;
    }
    else if (i_lightfeeler.object_hit == nullptr||i_lightfeeler.object_hit == NULL)
    {
        return glm::vec3(0.f);
    }
    else if (i_lightfeeler.object_hit->material->refract_idx_in > 0 &&
             i_lightfeeler.object_hit->material->refract_idx_out > 0)
    {
        return i_lightfeeler.color;
    }
    else
    {
        return glm::vec3(0.f);
    }

}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
