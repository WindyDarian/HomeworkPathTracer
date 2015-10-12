#include "imagewidestratifiedsampler.h"

ImageWideStratifiedSampler::ImageWideStratifiedSampler():ImageWideStratifiedSampler(1){}

ImageWideStratifiedSampler::ImageWideStratifiedSampler(int samples)
    : PixelSampler(samples),
      mersenne_generator(),
      unif_distribution(0,1),
      min_distance( .5f / samples)
{}

void ImageWideStratifiedSampler::initialize(int width, int height)
{
    this->regenerateSamples(width, height);
}

void ImageWideStratifiedSampler::regenerateSamples(int width, int height)
{
    this->min_distance = .5f/samples_sqrt;
    const float max_float = std::numeric_limits<float>::max();
    int samples_per_pixel = samples_sqrt * samples_sqrt;

    QElapsedTimer timer;
    timer.start();

    this->cached_grid.reset(new std::vector<std::vector<PixelSamples>>);
    this->cached_grid->reserve(width);

    float min_distance2 = glm::pow(min_distance, 2.f);

    std::vector<std::vector<PixelSamples>>& grid = *this->cached_grid;

    for (int x = 0; x < width; x++)
    {
        grid.push_back(std::vector<PixelSamples>(height));


        for (int y = 0; y < height; y++)
        {
            float left = x;
            float top = y;

            // for border checck
            float left_pd = left + min_distance;
            float top_pd = top + min_distance;
            float right_pd = left + 1 - min_distance;
            float bottom_pd = top + 1 - min_distance;

            for(int i = 0; i < samples_per_pixel; i++)
            {
                // generate sample

                //for each sample, try at least MIN_TRIES times and select
                // the furthest one (or retries until further than min_distance)
                // from all previous samples in the image

                glm::vec2 current_point, chosen_point;
                float current_distance2;
                float chosen_distance2 = 0;
                int ntries = 0;

                while(true)
                {
                    // generate one random point
                    current_point.x = left + unif_distribution(mersenne_generator);
                    current_point.y = top + unif_distribution(mersenne_generator);
                    current_distance2 = max_float;

                    // check distance to all possible points
                    for (auto sample: grid[x][y].samples)
                    {
                         current_distance2 = std::min(glm::distance2(current_point, sample), current_distance2);
                    }
                    if (x > 0 && current_point.x < left_pd)
                    {
                        // check left pixel
                        for (auto sample: grid[x-1][y].right_samples)
                        {
                             current_distance2 = std::min(glm::distance2(current_point, sample), current_distance2);
                        }

                        if (y > 0 && current_point.y < top_pd)
                        {
                            // check top-left pixel
                            for (auto sample: grid[x-1][y-1].bottom_right_samples)
                            {
                                 current_distance2 = std::min(glm::distance2(current_point, sample), current_distance2);
                            }
                        }
                        if (y < height-1 && current_point.y >= bottom_pd)
                        {
                            // check down-left pixel
                            for (auto sample: grid[x-1][y+1].top_right_samples)
                            {
                                 current_distance2 = std::min(glm::distance2(current_point, sample), current_distance2);
                            }
                        }
                    }
                    if (y > 0 && current_point.y < top_pd)
                    {
                        // check top grid
                        for (auto sample: grid[x][y-1].bottom_samples)
                        {
                             current_distance2 = std::min(glm::distance2(current_point, sample), current_distance2);
                        }
                    }

                    if (current_distance2 > chosen_distance2)
                    {
                        chosen_distance2 = current_distance2;
                        chosen_point = current_point;
                    }

                    ntries++;

                    if (ntries <= MIN_TRIES) continue;
                    if (chosen_distance2 >= min_distance2 || ntries > MAX_TRIES) break;
                }

                // append chosen_point to samples
                grid[x][y].samples.push_back(chosen_point);
                if (chosen_point.x >= right_pd && chosen_point.y >= bottom_pd)
                {
                    grid[x][y].bottom_right_samples.push_back(chosen_point);
                }
                if (chosen_point.x >= right_pd && chosen_point.y < top_pd)
                {
                    grid[x][y].top_right_samples.push_back(chosen_point);
                }
                if (chosen_point.x >= right_pd)
                {
                    grid[x][y].right_samples.push_back(chosen_point);
                }
                if (chosen_point.y >= bottom_pd)
                {
                    grid[x][y].bottom_samples.push_back(chosen_point);
                }

            }

        }
    }

    int elapsed_time = timer.elapsed();
    std::cout << "Sample calculation for image-wide stratified sampler completed. Total time: "
              << std::setprecision(9) << elapsed_time/1000.0
              << " seconds. (" << elapsed_time << " millseconds.)"
              << std::endl;

}

QList<glm::vec2> ImageWideStratifiedSampler::GetSamples(int x, int y)
{
    if (!this->cached_grid)
    {
        return QList<glm::vec2>();
    }

    return QList<glm::vec2>::fromStdList((*this->cached_grid)[x][y].samples);
}









