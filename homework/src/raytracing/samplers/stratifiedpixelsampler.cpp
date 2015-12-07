#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <iostream>
#include <functional>
#include <ctime>

StratifiedPixelSampler::StratifiedPixelSampler():StratifiedPixelSampler(1, std::time(nullptr)){}

StratifiedPixelSampler::StratifiedPixelSampler(unsigned int samples):StratifiedPixelSampler(samples, std::time(nullptr)){}

StratifiedPixelSampler::StratifiedPixelSampler(unsigned int samples, unsigned int seed) : PixelSampler(samples), mersenne_generator(seed), unif_distribution(0,1)
{

}


QList<glm::vec2> StratifiedPixelSampler::GetSamples(int x, int y)
{
    int n = this->samples_sqrt;
    float grid_width = 1.f / static_cast<float>(n);
    // top up for window space
    float left = x;
    float top = y;

    QList<glm::vec2> result;


    for (int i = 0; i < n; i++)
    {
        for (int j = 0;j < n; j++)
        {
            // for each cell generate one random point
            result.append(glm::vec2(left + grid_width * (static_cast<float>(j) + unif_distribution(mersenne_generator)),
                                    top + grid_width * (static_cast<float>(i) + unif_distribution(mersenne_generator))
                                    )
                          );
        }
    }

    return result;
}

glm::vec2 StratifiedPixelSampler::getOneSample(int x, int y, int index)
{
    int n = this->samples_sqrt;
    float grid_width = 1.f / static_cast<float>(n);
    // top up for window space
    float left = x;
    float top = y;

    QList<glm::vec2> result;

    int i = index % n;
    int j = index / n;

    return glm::vec2(left + grid_width * (static_cast<float>(j) + unif_distribution(mersenne_generator)),
                                    top + grid_width * (static_cast<float>(i) + unif_distribution(mersenne_generator))
                                    );
}
