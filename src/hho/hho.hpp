#pragma once

#include <omp.h>
#include <vector>

namespace r3dp::hho {

  template <class Decoder, class RNG>
  class HHO {
  public:
    HHO( unsigned       population_size,
         unsigned       dimension,
         double         lower_bound,
         double         upper_bound,
         unsigned       max_iterations,
         unsigned       max_threads,
         const Decoder &ref_decoder,
         RNG           &ref_rng );

    HHO( unsigned                   population_size,
         unsigned                   dimension,
         const std::vector<double> &lower_bounds,
         const std::vector<double> &upper_bounds,
         unsigned                   max_iterations,
         unsigned                   max_threads,
         const Decoder             &ref_decoder,
         RNG                       &ref_rng );

    void                              step();
    [[nodiscard]] std::vector<double> get_best_solution() const;
    [[nodiscard]] double              get_best_fitness() const;
    [[nodiscard]] unsigned            get_iteration() const;
    void                              reset();

  private:
    const unsigned      dimension;
    const unsigned      population_size;
    std::vector<double> lower_bounds_vec;
    std::vector<double> upper_bounds_vec;
    const unsigned      max_iterations;
    const unsigned      max_threads;

    double                           rabbit_energy;
    std::vector<std::vector<double>> hawks;
    std::vector<double>              best_position;
    double                           best_fitness;
    unsigned                         iteration;

    RNG           &ref_rng;
    const Decoder &ref_decoder;

    std::vector<double> levy_flight( int d );
    void                initialize_hawks();
  };

}  // namespace r3dp::hho
