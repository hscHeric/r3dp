#pragma once

#include <vector>

namespace r3dp::rkhho {
  template <class Decoder, class RNG>
  class HHO_RK {
  public:
    HHO_RK( const unsigned population_size,
            const unsigned dimension,
            const double   lower_bound,
            const double   upper_bound,
            const Decoder &ref_decoder,
            RNG           &ref_rng );

    void                step();
    std::vector<double> get_best_solution() const;
    double              get_best_fitness() const;
    unsigned            get_iteration() const;
    void                reset();  // reseta para usar em multiplas trials

  private:
    const unsigned dimension;
    const unsigned population_size;
    const double   lower_bound;
    const double   upper_bound;

    // Estado do algoritmo
    double                           rabbit_energy;
    std::vector<std::vector<double>> hawks;
    std::vector<double>              best_position;
    double                           best_fitness;
    unsigned                         iteration;

    // Templates:
    RNG           &ref_rng;
    const Decoder &ref_decoder;

    std::vector<double> levy_flight( int d );
  };
}  // namespace r3dp::rkhho
