#include <scene/materials/weightedmaterial.h>
#include <raytracing/sampling.h>

WeightedMaterial::WeightedMaterial() : Material(){}
WeightedMaterial::WeightedMaterial(const glm::vec3 &color) : Material(color){}

glm::vec3 WeightedMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, float &pdf_ret, BxDFType flags) const
{
    pdf_ret = 0.f;

    glm::vec3 result(0.f);

    if (this->bxdfs.size() == 0)
    {
        return glm::vec3(0.f);
    }

    // using weighted destribution
    int index = static_cast<int>((*this->weight_distribution)(sampling::mersenne_generator));
    if (index >= bxdfs.size()) index = bxdfs.size() - 1; //although this not gonna happen
    auto bxdf = this->bxdfs[index];

//    float total_w;
//    for (int i = 0; i < this->bxdf_weights.size(); i++)
//    {
//        total_w += bxdf_weights[i];
//    }
//    float vv = sampling::unif_random() * total_w;
//    for (int i = 0; i < this->bxdf_weights.size(); i++)
//    {
//        vv -= bxdf_weights[i];
//        if (vv <= 0)
//            bxdf = this->bxdfs[i];
//    }

    if (!(bxdf->type & flags))
        return glm::vec3(0.f);

    glm::mat3 trans = glm::transpose(glm::mat3(isx.tangent, isx.bitangent, isx.normal));
    glm::vec3 wo = trans * woW;
    glm::vec3 wi = trans * wiW;
    result += bxdf->EvaluateScatteredEnergy(wo, wi, pdf_ret);


    result = result * isx.texture_color * this->base_color;

    return result;
}

glm::vec3 WeightedMaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDFType flags) const
{


    if (this->bxdfs.count() == 0)
    {
        wiW_ret = glm::vec3(0.f);
        pdf_ret = 0.f;
        return glm::vec3(0.f);
    }

    // using weighted destribution
    int index = static_cast<int>((*this->weight_distribution)(sampling::mersenne_generator));
    if (index >= bxdfs.size()) index = bxdfs.size() - 1; //although this not gonna happen
    auto bxdf = this->bxdfs[index];

//    auto bxdf = this->bxdfs[0];

//    float total_w;
//    for (int i = 0; i < this->bxdf_weights.size(); i++)
//    {
//        total_w += bxdf_weights[i];
//    }
//    float vv = sampling::unif_random() * total_w;
//    for (int i = 0; i < this->bxdf_weights.size(); i++)
//    {
//        vv -= bxdf_weights[i];
//        if (vv <= 0)
//            bxdf = this->bxdfs[i];
//    }


    // check if type match
    if (!(bxdf->type & flags))
    {
        wiW_ret = glm::vec3(0.f);
        pdf_ret = 0.f;
        return glm::vec3(0.f);
    }

    glm::mat3 local_to_world = glm::mat3(isx.tangent, isx.bitangent, isx.normal);
    glm::mat3 world_to_local = glm::transpose(local_to_world);

    glm::vec3 wo = world_to_local * woW;
    glm::vec3 wi_ret;
    glm::vec3 result = bxdf->SampleAndEvaluateScatteredEnergy(wo, wi_ret, pdf_ret);

    wiW_ret = local_to_world * wi_ret;


    result = result * isx.texture_color * this->base_color;

    return result;
}

void WeightedMaterial::resetWeightDistribution()
{
    std::vector<int> indices;
    for (int i = 0; i <= this->bxdf_weights.count(); i++)
    {
        indices.push_back(i);
    }
    this->weight_distribution.reset(
                new std::piecewise_constant_distribution<float>(indices.begin(), indices.end(), this->bxdf_weights.begin()));


    int do_nothing;

}
