#include "sampling.h"
#include <ctime>

std::mt19937 sampling::mersenne_generator(std::time(nullptr));
std::uniform_real_distribution<float> sampling::unif_distribution;
