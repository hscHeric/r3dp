#pragma once

#include "../core/rng.hpp"

#include <omp.h>
#include <vector>

namespace r3dp::hho {
  template <class Decoder>
  class HHO {
  public:
    HHO( const unsigned population_size,
         const unsigned dimension,
         const double   lower_bound,
         const double   upper_bound,
         const unsigned max_threads,
         const Decoder &ref_decoder,
         core::RNG     &ref_rng );

    void                step();
    std::vector<double> get_best_solution() const;
    double              get_best_fitness() const;
    unsigned            get_iteration() const;
    void                reset();

  private:
    const unsigned dimension;
    const unsigned population_size;
    const double   lower_bound;
    const double   upper_bound;
    const unsigned max_threads;

    // Estado do algoritmo
    double                           rabbit_energy;
    std::vector<std::vector<double>> hawks;
    std::vector<double>              best_position;
    double                           best_fitness;
    unsigned                         iteration;

    // Templates:
    core::RNG     &ref_rng;
    const Decoder &ref_decoder;

    std::vector<double> levy_flight( int d );
    void                initialize_hawks();
  };
}  // namespace r3dp::hho
