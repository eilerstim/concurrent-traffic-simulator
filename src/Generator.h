#ifndef GENERATOR_H
#define GENERATOR_H
#include <random>

// global random number generating engine
namespace RNG {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<std::mt19937::result_type> dist(4000,6000); // distribution in range [4000, 6000]
}

#endif