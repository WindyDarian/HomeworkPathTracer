#include <raytracing/samplers/uniformpixelsampler.h>

UniformPixelSampler::UniformPixelSampler():PixelSampler()
{}

UniformPixelSampler::UniformPixelSampler(int samples):PixelSampler(samples)
{}

QList<glm::vec2> UniformPixelSampler::GetSamples(int x, int y)
{

    int n = this->samples_sqrt;
    float grid_width = 1.f / static_cast<float>(n);
    // top up for window space
    float left = x + grid_width * 0.5f;
    float top = y + grid_width * 0.5f;

    QList<glm::vec2> result;


    for (int i = 0; i < n; i++)
    {
        for (int j = 0;j < n; j++)
        {
            result.append(glm::vec2(left + j * grid_width, top + i * grid_width));
        }
    }

    return result;
}
