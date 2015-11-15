#include <scene/materials/material.h>
#include <QColor>
#include <math.h>
#include <raytracing/sampling.h>

Material::Material() :
    Material(glm::vec3(0.5f, 0.5f, 0.5f))
{}

Material::Material(const glm::vec3 &color):
    name("MATERIAL"),
    bxdfs(),
    is_light_source(false),
    base_color(color),
    intensity(0)
{
    texture = NULL;
    normal_map = NULL;
}




glm::vec3 Material::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, float &pdf_ret, BxDFType flags) const
{
    pdf_ret = 0.f;

    glm::vec3 result(0.f);

    if (this->bxdfs.count() == 0)
    {
        pdf_ret = 0.f;
        return glm::vec3(0.f);
    }

    int i = sampling::rand_int(bxdfs.count());

    auto bxdf = bxdfs[i];

    // check if type match // guess this may cause darker if an object has different type of BxDFs
    // but pick random one until match may cause endless loop if there is just..none match
    if (!(bxdf->type & flags))
    {
        pdf_ret = 0.f;
        return glm::vec3(0.f);
    }

    glm::mat3 trans = glm::transpose(glm::mat3(isx.tangent, isx.bitangent, isx.normal));
    glm::vec3 wo = trans * woW;
    glm::vec3 wi = trans * wiW;
    result += bxdf->EvaluateScatteredEnergy(wo, wi, pdf_ret);


    result = result * isx.texture_color * this->base_color;

    return result;
}

glm::vec3 Material::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDFType flags) const
{

    if (this->bxdfs.count() == 0)
    {
        wiW_ret = glm::vec3(0.f);
        pdf_ret = 0.f;
        return glm::vec3(0.f);
    }

    int i = sampling::rand_int(bxdfs.count());

    auto bxdf = bxdfs[i];

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


//    glm::mat3 local_to_world(isx.tangent, isx.bitangent, isx.normal);

//    // cosine weighted hemisphere sampling
//    glm::vec3 wiO = sampling::cosine_hemisphere();
//    wiW_ret = local_to_world * wiO;

//    float costheta = glm::dot(local_to_world * glm::vec3(0.f, 0.f, 1.f), woW);
//    pdf_ret = costheta * INV_PI;

//    return EvaluateScatteredEnergy(isx, woW, wiW_ret, flags);
}

glm::vec3 Material::EvaluateHemisphereScatteredEnergy(const Intersection &isx, const glm::vec3 &wo, int num_samples, BxDFType flags) const
{
    //TODO
    return glm::vec3(0);
}



glm::vec3 Material::GetImageColor(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == NULL || uv_coord.x < 0 || uv_coord.y < 0 || uv_coord.x >= 1.0f || uv_coord.y >= 1.0f)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue())/255.0f;
    }
}

glm::vec3 Material::GetImageColorInterp(const glm::vec2 &uv_coord, const QImage* const& image)
{
    if(image == NULL || uv_coord.x < 0 || uv_coord.y < 0 || uv_coord.x >= 1.0f || uv_coord.y >= 1.0f)
    {
        return glm::vec3(1,1,1);
    }
    else
    {
        //Use bilinear interp.

        float X = image->width() * uv_coord.x;
        float Y = image->height() * (1.0f - uv_coord.y);

        glm::vec2 floors = glm::vec2(floor(X), floor(Y));
        glm::vec2 ceils = glm::vec2(ceil(X), ceil(Y));
        ceils = glm::min(ceils, glm::vec2(image->width()-1, image->height()-1));
        QColor qll = image->pixel(floors.x, floors.y); glm::vec3 ll(qll.red(), qll.green(), qll.blue());
        QColor qlr = image->pixel(ceils.x, floors.y); glm::vec3 lr(qlr.red(), qlr.green(), qlr.blue());
        QColor qul = image->pixel(floors.x, ceils.y); glm::vec3 ul(qul.red(), qul.green(), qul.blue());
        QColor qur = image->pixel(ceils.x, ceils.y); glm::vec3 ur(qur.red(), qur.green(), qur.blue());

        float distX = (X - floors.x);
        glm::vec3 color_low = ll * (1-distX) + lr * distX;
        glm::vec3 color_high = ul * (1-distX) + ur * distX;

        float distY = (Y - floors.y);

        glm::vec3 result = (color_low * (1 - distY) + color_high * distY)/255.0f;

        return result;


        glm::vec2 XY(X,Y);
        //Want floor and ceil of both X and Y
        //Do square interp of <X,Y>

        float dist_ll = glm::distance(XY, floors);
        float dist_lr = glm::distance(XY, glm::vec2(ceils.x, floors.y));
        float dist_ul = glm::distance(XY, glm::vec2(floors.x, ceils.y));
        float dist_ur = glm::distance(XY, ceils);
        float sum = dist_ll + dist_lr + dist_ul + dist_ur;

        float llc = (1 - dist_ll/sum);
        float lrc = (1 - dist_lr/sum);
        float ulc = (1 - dist_ul/sum);
        float urc = (1 - dist_ur/sum);

        float checksum = llc + lrc + ulc + urc;

        glm::vec3 final_color = llc * ll + lrc * lr + ulc * ul + urc * ur;
        return final_color/255.0f;
    }
}
