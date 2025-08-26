#pragma once

#define R3DP_USE_GA

#include "core/graph.hpp"
#include "core/rng.hpp"

#ifdef R3DP_USE_GA
  #include "genetic_algorithm/genetic_algorithm.hpp"
  #include "genetic_algorithm/pop_generator.hpp"
  #include "genetic_algorithm/r3d_decoder.hpp"
#endif

namespace r3dp {
  namespace version {
    constexpr unsigned int       major  = 0;
    constexpr unsigned int       minor  = 1;
    constexpr unsigned int       patch  = 0;
    constexpr const char * const string = "0.1.0";
  }  // namespace version

}  // namespace r3dp
