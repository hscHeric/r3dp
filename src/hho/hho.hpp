#pragma once

#include "../core/rng.hpp"

#include <omp.h>
#include <vector>

namespace r3dp::hho {
  template <class Decoder>
  class HHO {
  public:
    HHO( unsigned       population_size,
         unsigned       dimension,
         double         lower_bound,
         double         upper_bound,
         unsigned       max_iterations,
         unsigned       max_threads,
         const Decoder &ref_decoder,
         core::RNG<>   &ref_rng );

    HHO( unsigned                   population_size,
         unsigned                   dimension,
         const std::vector<double> &lower_bounds,
         const std::vector<double> &upper_bounds,
         unsigned                   max_iterations,
         unsigned                   max_threads,
         const Decoder             &ref_decoder,
         core::RNG<>               &ref_rng );

    void                                     step();
    [[nodiscard]] const std::vector<double> &get_best_solution() const;
    [[nodiscard]] double                     get_best_fitness() const;
    [[nodiscard]] const std::vector<double> &get_convergence_curve() const;
    [[nodiscard]] unsigned                   get_iteration() const;
    void                                     reset();

  private:
    const unsigned      dimension;
    const unsigned      population_size;
    std::vector<double> lower_bounds_vec;
    std::vector<double> upper_bounds_vec;
    const unsigned      max_iterations;
    const unsigned      max_threads;

    std::vector<std::vector<double>> hawks;
    std::vector<double>              best_position;
    double                           best_fitness;
    unsigned                         iteration;
    std::vector<double>              convergence_curve;

    core::RNG<>   &ref_rng;
    const Decoder &ref_decoder;

    std::vector<double> levy_flight( int d );
    void                initialize_hawks();

    // MAGIC NUMBES
    static constexpr double LEVY_BETA                        = 1.5;
    static constexpr double ESCAPE_ENERGY_BOUND_EXPLORATION  = 1.0;
    static constexpr double ESCAPE_ENERGY_BOUND_HARD_BESIEGE = 0.5;
    static constexpr double DIVE_PROBABILITY_THRESHOLD       = 0.5;
    static constexpr double JUMP_STRENGTH_FACTOR             = 2.0;
  };

}  // namespace r3dp::hho
