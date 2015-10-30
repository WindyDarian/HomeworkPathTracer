#include "directlightingintegrator.h"
#include <ctime>


DirectLightingIntegrator::DirectLightingIntegrator():Integrator(),
    mersenne_generator(std::time(nullptr)),
    unif_distribution(0,1)
{

}

glm::vec3 DirectLightingIntegrator::TraceRay(Ray r, unsigned int depth)
{
    //float accurancy = std::numeric_limits<float>::min() * 10000;
    float accurancy = 0.0001f;
    glm::vec3 dummy(0.f);


    //if (depth >= this->max_depth)
    //    return glm::vec3(0,0,0);

    auto intersection = this->intersection_engine->GetIntersection(r);

    if (!intersection.object_hit)
        return glm::vec3(0,0,0);

    glm::vec3 out_origin_outside = intersection.point + intersection.normal * accurancy;
    //glm::vec3 out_origin_inside = intersection.point - intersection.normal * accurancy;

    glm::vec3 color(0.f);

    if (intersection.object_hit->material->is_light_source)
        color = intersection.object_hit->material->EvaluateScatteredEnergy(intersection, dummy, -r.direction) ;

    for (auto light:this->scene->lights)
    {

        if (light == intersection.object_hit)
            continue;

        glm::vec3 scolor(0.f);
        for(int i = 0; i < sample_num; i++)
        {

            //float r1 = unif_distribution(mersenne_generator);
            //float r2 = unif_distribution(mersenne_generator);

            std::function<float()> random_function = std::bind(&generateRandom, this);
            //Intersection light_sample = light->pickSampleIntersection(r1,r2);
            Intersection light_sample = light->pickSampleIntersection(random_function, &intersection.point);

            glm::vec3 wiW = glm::normalize(light_sample.point - intersection.point);
            Ray out_ray = Ray(out_origin_outside, wiW);
            float pdf = light->RayPDF(light_sample, out_ray);
            if (pdf<=0) continue;

            if (pdf && intersection_engine->GetIntersection(out_ray).object_hit == light)
            {

               auto e1 = intersection.object_hit->material->EvaluateScatteredEnergy(intersection, -r.direction, wiW);
               auto e2 = light->material->EvaluateScatteredEnergy(light_sample, dummy, -wiW);
               auto e3 = glm::abs(glm::dot(wiW, intersection.normal));

               scolor += e1 * e2 * e3 / pdf;
            }
        }
        scolor /= sample_num;
        color += scolor;
    }

    //glm::vec3 color = (lightnum > 0)? color_sum / static_cast<float>(lightnum): glm::vec3(0.f);

    return color;

}

float DirectLightingIntegrator::generateRandom()
{
    return unif_distribution(mersenne_generator);
}

