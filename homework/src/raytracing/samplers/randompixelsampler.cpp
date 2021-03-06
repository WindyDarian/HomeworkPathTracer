#include "randompixelsampler.h"
#include <iostream>
#include <functional>
#include <ctime>

RandomPixelSampler::RandomPixelSampler():RandomPixelSampler(1, std::time(nullptr)){}

RandomPixelSampler::RandomPixelSampler(unsigned int samples):RandomPixelSampler(samples, std::time(nullptr)){}

RandomPixelSampler::RandomPixelSampler(unsigned int samples, unsigned int seed) : PixelSampler(samples), mersenne_generator(seed), unif_distribution(0,1){}

QList<glm::vec2> RandomPixelSampler::GetSamples(int x, int y)
{
    int n = this->samples_sqrt;
    int total_samples = n * n;
    // top up for window space
    float left = x;
    float top = y;
    //float grid_width = 1.f;

    QList<glm::vec2> result;


    for (int i = 0; i < total_samples; i++)
    {

            // generate one random point
            result.append(glm::vec2(left + unif_distribution(mersenne_generator),
                                    top + unif_distribution(mersenne_generator)
                                    )
                          );
    }

    return result;
}
