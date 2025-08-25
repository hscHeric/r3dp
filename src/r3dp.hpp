#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>

#define R3DP_VERSION_MINOR  0
#define R3DP_VERSION_MAJOR  0
#define R3DP_VERSION_PATCH  1
#define R3DP_VERSION_STRING "0.0.1"

#ifndef R3DP_CORE
  #define R3DP_CORE
  #include "core/graph.hpp"
  #include "core/rng.hpp"
#endif  // !CORE

#ifndef R3DP_GA
  #define R3DP_GA
  #include "genetic_algorithm/genetic_algorithm.hpp"
  #include "genetic_algorithm/population.hpp"
  #include "genetic_algorithm/r3d_decoder.hpp"
#endif
