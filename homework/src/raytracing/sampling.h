#pragma once
#include <la.h>
#include <random>

namespace sampling
{
    extern std::mt19937 mersenne_generator;
    extern std::uniform_real_distribution<float> unif_distribution;

    inline float unif_random()
    {
        return unif_distribution(mersenne_generator);
    }

    inline glm::vec2 concentric_sample_disc() {
//        glm::vec2 u(unif_random(), unif_random());
//        glm::vec2 uOffset = 2.f * u - glm::vec2(1.f, 1.f);

//        // Handle degeneracy at the origin
//        if (fequal(uOffset.x, 0.f) && fequal(uOffset.y, 0.f)) return glm::vec2(0.f);

//        // Apply concentric mapping to point
//        float theta, r;
//        if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
//            r = uOffset.x;
//            theta = PI_OVER_4 * (uOffset.y / uOffset.x);
//        } else {
//            r = uOffset.y;
//            theta = PI_OVER_2 - PI_OVER_4 * (uOffset.x / uOffset.y);
//        }
//        return r * glm::vec2(glm::cos(theta), glm::sin(theta));

        float u1 = unif_random();
        float u2 = unif_random();

        float sx = 2 * u1 - 1.0f;
        float sy = 2 * u2 - 1.0f;
        float r, theta;

        if (sx == 0.0 && sy == 0.0)
        {
            return glm::vec2(0.f);
        }
        if (sx >= -sy)
        {
            if (sx > sy)
            {
                // Handle first region of disk
                r = sx;
                if (sy > 0.0) theta = sy/r;
                else          theta = 8.0f + sy/r;
            }
            else
            {
                // Handle second region of disk
                r = sy;
                theta = 2.0f - sx/r;
            }
        }
        else
        {
            if (sx <= sy)
            {
                // Handle third region of disk
                r = -sx;
                theta = 4.0f - sy/r;
            }
            else
            {
                // Handle fourth region of disk
                r = -sy;
                theta = 6.0f + sx/r;
            }
        }
        theta *= PI / 4.f;
        float dx = r * cosf(theta);
        float dy = r * sinf(theta);
        return glm::vec2(dx, dy);
    }

    inline glm::vec3 cosine_hemisphere() {
        glm::vec2 d = concentric_sample_disc();
        float z = glm::sqrt(glm::max(0.f, 1.f - d.x * d.x - d.y * d.y));
        return glm::vec3(d.x, d.y, z);
    }

    inline int rand_int(int max_exclusive)
    {
        int a = static_cast<int>(unif_distribution(mersenne_generator) * max_exclusive);
        if (a < max_exclusive)
            return a;
        else return a - 1;
    }
}
