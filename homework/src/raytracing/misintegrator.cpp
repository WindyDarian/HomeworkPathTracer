#include "misintegrator.h"
#include <ctime>
#include <raytracing/sampling.h>
#include <scene/materials/bxdfs/specularreflectionbxdf.h>

MISIntegrator::MISIntegrator():Integrator(),
    mersenne_generator(std::time(nullptr)),
    unif_distribution(0,1)
    //sample_num(1)
{

}

inline float BalanceHeuristic(int nf, float fPdf, int ng, float gPdf) {
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
    float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

glm::vec3 MISIntegrator::TraceRay(Ray r, unsigned int depth)
{
    this->tracing_list.clear();

    //float accurancy = std::numeric_limits<float>::min() * 10000;
    float accurancy = 0.0001f;
    glm::vec3 dummy(0.f);


    glm::vec3 color(0.f);
    for (auto light:this->scene->lights)
    {
        float current_pdf = 1.f;
        Ray current_ray = r;
        glm::vec3 current_factor(1.f);
        float throughput = 1.f;

        int current_depth = 0;


        if (current_depth >= min_depth)
            continue;

        Intersection intersection = this->intersection_engine->GetIntersection(current_ray);

        if (!intersection.object_hit)
        {
            break;
        }

        if (intersection.object_hit->material->is_light_source)
        {
            color =  intersection.object_hit->material->base_color * intersection.texture_color;
            break;
        }


        while(true)
        {
            if (current_depth >= min_depth)
            {
                if (throughput <= 0)
                    break;

                if (sampling::unif_random() > throughput)
                    break;
            }
            current_depth += 1;

            if (!intersection.object_hit || intersection.object_hit->material->is_light_source)
            {
                // no hit or hit other light (contribution calculated in other light loops)
                break;
            }

            float normal_fix = -glm::sign(glm::dot(current_ray.direction, intersection.normal));
            glm::vec3 out_origin_outside = intersection.point + intersection.normal * accurancy * normal_fix;
            glm::vec3 out_origin_inside = intersection.point - intersection.normal * accurancy * normal_fix;

             // direct_lighting (light sample)
            {
                glm::vec3 color_from_light(0.f);

                // SAMPLE NUM is fixed to 1
                //float r1 = unif_distribution(mersenne_generator);
                //float r2 = unif_distribution(mersenne_generator);

                std::function<float()> random_function = std::bind(&generateRandom, this);
                //Intersection light_sample = light->pickSampleIntersection(r1,r2);
                Intersection light_sample = light->pickSampleIntersection(random_function, &intersection.normal);

                glm::vec3 wiW = glm::normalize(light_sample.point - intersection.point);
                Ray out_ray = Ray(out_origin_outside, wiW);
                float lightpdf_a = light->RayPDF(light_sample, out_ray);


                if (lightpdf_a > 0 && intersection_engine->GetIntersection(out_ray).object_hit == light)
                {
                    float bsdf_pdf_a;
                    float dummy_f = 1.f;

                    auto e1 = intersection.object_hit->material->EvaluateScatteredEnergy(intersection, -current_ray.direction, wiW, bsdf_pdf_a);
                    auto e2 = light->material->EvaluateScatteredEnergy(light_sample, dummy, -wiW, dummy_f);
                    auto e3 = glm::abs(glm::dot(wiW, intersection.normal));
                    //if (bsdf_pdf == 0)
                    //    break;

                    float weight = PowerHeuristic(1, lightpdf_a, 1, bsdf_pdf_a);

                    color_from_light += current_factor * e1 * e2 * e3 * weight / lightpdf_a;
                }
                color += color_from_light;
            }



            // bsdf sample
            glm::vec3 sample_wi;
            float bsdf_pdf_b;

            auto bsdf_sample_f = intersection.object_hit->material->SampleAndEvaluateScatteredEnergy(intersection,  -current_ray.direction, sample_wi, bsdf_pdf_b);

            if (bsdf_pdf_b <= 0 || fequal(glm::length2(sample_wi), 0.f))
                break;

            current_pdf = bsdf_pdf_b;
            if (glm::dot(sample_wi, -current_ray.direction)>0)
            {
                // reflection
                current_ray = Ray(out_origin_outside, glm::normalize(sample_wi));
            }
            else {
                // transmission
                current_ray = Ray(out_origin_inside, glm::normalize(sample_wi));
            }

            Intersection intersection_bsdf = intersection_engine->GetIntersection(current_ray);

            if (!intersection_bsdf.object_hit)
            {
                break;
            }

            //direct_lighting (bsdf sample)
                if (intersection_bsdf.object_hit == light)
                {
                    float light_pdf_b = light->RayPDF(intersection_bsdf, current_ray);
                    float dummy_f = 1.f;

                    auto e1 = bsdf_sample_f;
                    auto e2 = light->material->EvaluateScatteredEnergy(intersection_bsdf, dummy, -current_ray.direction, dummy_f);
                    auto e3 = glm::abs(glm::dot(current_ray.direction, intersection.normal));

                    float weight = PowerHeuristic(1, bsdf_pdf_b, 1, light_pdf_b);
                    color += current_factor * e1 * e2 * e3 * weight / bsdf_pdf_b;

                    break;
                }


            //continue on indirect lighting
            auto abscostheta = glm::abs(glm::dot(current_ray.direction, intersection.normal));
            //auto weight = PowerHeuristic(1, bsdf_pdf_b, 1, )
            current_factor = current_factor * bsdf_sample_f * abscostheta / bsdf_pdf_b;// / bsdf_pdf_b;

//            if (intersection.object_hit->material->bxdfs.count() > 0 && current_depth <= 1 )
//            if ( auto d = dynamic_cast<SpecularReflectionBxDF*>(intersection.object_hit->material->bxdfs[0]))
//            {
//                int justdebuging = 0;
//            }

            intersection = intersection_bsdf;
            throughput *= current_pdf * glm::abs(glm::dot(intersection.normal, glm::normalize(sample_wi)));



        }
    }


    return color;

}

float MISIntegrator::generateRandom()
{
    return unif_distribution(mersenne_generator);
}

