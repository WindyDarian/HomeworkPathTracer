#include <renderthread.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>

QMutex RenderThread::mutex;

RenderThread::RenderThread(unsigned int xstart, unsigned int xend, unsigned int ystart, unsigned int yend, unsigned int samplesSqrt, unsigned int depth, Film *f, Camera *c, Integrator *i)
    : x_start(xstart), x_end(xend), y_start(ystart), y_end(yend), samples_sqrt(samplesSqrt), max_depth(depth), film(f), camera(c), integrator(i)
{}

void RenderThread::run()
{
    unsigned int seed = (((x_start << 16 | x_end) ^ x_start) * ((y_start << 16 | y_end) ^ y_start));
    StratifiedPixelSampler pixel_sampler(samples_sqrt, seed);

    //depth first (original)
//    for(unsigned int Y = y_start; Y < y_end; Y++)
//    {
//        for(unsigned int X = x_start; X < x_end; X++)
//        {
//            glm::vec3 pixel_color;
//            QList<glm::vec2> samples = pixel_sampler.GetSamples(X, Y);
//            for(int i = 0; i < samples.size(); i++)
//            {
//                Ray ray = camera->Raycast(samples[i]);
//                pixel_color += integrator->TraceRay(ray, 0);
//            }
//            pixel_color /= samples.size();
//            film->pixels[X][Y] = pixel_color;

//            if (preview_image)
//            {
//                mutex.lock();
//                preview_image->setPixel(X,Y,qRgba((int)(glm::clamp(pixel_color.x,0.f,1.0f) * 255),
//                                                  (int)(glm::clamp(pixel_color.y,0.f,1.0f) * 255),
//                                                  (int)(glm::clamp(pixel_color.z,0.f,1.0f) * 255),
//                                                  255));
//                mutex.unlock();
//            }
//        }
//    }

    int sample_size = samples_sqrt * samples_sqrt;

    //Breadth First
    for(int i = 0; i < sample_size; i++)
    {
        for(unsigned int Y = y_start; Y < y_end; Y++)
        {
            for(unsigned int X = x_start; X < x_end; X++)
            {
                glm::vec3 pixel_color;
                QList<glm::vec2> samples = pixel_sampler.GetSamples(X, Y);

                Ray ray = camera->Raycast(samples[i]);
                pixel_color = (integrator->TraceRay(ray, 0) + film->pixels[X][Y] * (float)i) / (float)(i + 1);

                //pixel_color /= samples.size();
                film->pixels[X][Y] = pixel_color;

                if (preview_image)
                {
                    mutex.lock();
                    preview_image->setPixel(X,Y,qRgba((int)(glm::clamp(pixel_color.x,0.f,1.0f) * 255),
                                                      (int)(glm::clamp(pixel_color.y,0.f,1.0f) * 255),
                                                      (int)(glm::clamp(pixel_color.z,0.f,1.0f) * 255),
                                                      255));
                    mutex.unlock();
                }
            }
        }
    }

}
