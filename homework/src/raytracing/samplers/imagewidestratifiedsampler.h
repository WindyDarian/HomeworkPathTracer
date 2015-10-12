#pragma once
#include <la.h>
#include <QList>
#include <random>
#include <memory>
#include <QElapsedTimer>
#include <iostream>
#include <iomanip>


#include <raytracing/samplers/pixelsampler.h>



struct PixelSamples
{
    // PixelSamples contains NxN samples in a pixel

    std::list<glm::vec2> samples;

    // these 4 lists contains only the samples closer than min_distance to corner or border
    // building in column major order from top-left of window to bottom-right
    // so when checking distance with points in other pixels,
    // only right_samples of left pixel, bottom_samples of upper pixel,
    // bottom_right_samples of upper-left pixel
    // and top_right of down-left pixel needs to be checked
    std::list<glm::vec2> right_samples;
    std::list<glm::vec2> bottom_samples;
    std::list<glm::vec2> bottom_right_samples;
    std::list<glm::vec2> top_right_samples;
};



class ImageWideStratifiedSampler: public PixelSampler
{
public:
    ImageWideStratifiedSampler();
    ImageWideStratifiedSampler(int samples);
    virtual void initialize(int width, int height);
    virtual QList<glm::vec2> GetSamples(int x, int y);

protected:
    std::mt19937 mersenne_generator;
    std::uniform_real_distribution<float> unif_distribution;

    // min distance between two nodes cross the image
    // set as  0.5 / N in constructor to make sure a pixel
    // can contain N * N samples
    float min_distance;

    // for each one Sample of the N * N, try at least MIN_TRIES times and select
    // the furthest one (or retries until further than min_distance)
    // from all previous samples in the image
    const int MIN_TRIES = 4;

    // TODO: make sure this is useless
    // max chances to generate one sample in a pixel
    const int MAX_TRIES = 100;


    std::unique_ptr<std::vector<std::vector<PixelSamples>>> cached_grid = nullptr;

    void regenerateSamples(int width, int height);
};
