#include "randompixelsampler.h"
#include <iostream>
#include <functional>

RandomPixelSampler::RandomPixelSampler():RandomPixelSampler(1){}

RandomPixelSampler::RandomPixelSampler(int samples) : PixelSampler(samples), mersenne_generator(), unif_distribution(0,1){}

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
