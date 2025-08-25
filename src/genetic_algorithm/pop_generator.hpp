#pragma once

#include "../core/rng.hpp"
#include "population.hpp"

#include <vector>

namespace r3dp::ga {

  std::vector<Chromosome> create_random_population( unsigned int population_size,
                                                    unsigned int chromosome_size,
                                                    core::RNG   &rng ) {
    std::vector<Chromosome> population;
    for ( unsigned int i = 0; i < population_size; ++i ) {
      Chromosome chromosome( chromosome_size );
      for ( unsigned int j = 0; j < chromosome_size; ++j ) {
        chromosome[j] = rng.randInt( 3 );
      }
      population.push_back( chromosome );
    }
    return population;
  }

}  // namespace r3dp::ga
