#include "hho.hpp"

#include <limits>
#include <omp.h>
#include <vector>

namespace r3dp::hho {

  template <class Decoder, class RNG>
  HHO<Decoder, RNG>::HHO( unsigned       population_size,
                          unsigned       dimension,
                          double         lower_bound,
                          double         upper_bound,
                          unsigned       max_iterations,
                          unsigned       max_threads,
                          const Decoder &ref_decoder,
                          RNG           &ref_rng )
    : dimension( dimension )
    , population_size( population_size )
    , lower_bounds_vec( dimension, lower_bound )
    , upper_bounds_vec( dimension, upper_bound )
    , max_iterations( max_iterations )
    , max_threads( max_threads )
    , rabbit_energy( 0.0 )
    , best_fitness( std::numeric_limits<double>::infinity() )
    , iteration( 0 )
    , ref_rng( ref_rng )
    , ref_decoder( ref_decoder ) {
    initialize_hawks();
  }

  template <class Decoder, class RNG>
  HHO<Decoder, RNG>::HHO( unsigned                   population_size,
                          unsigned                   dimension,
                          const std::vector<double> &lower_bounds,
                          const std::vector<double> &upper_bounds,
                          unsigned                   max_iterations,
                          unsigned                   max_threads,
                          const Decoder             &ref_decoder,
                          RNG                       &ref_rng )
    : dimension( dimension )
    , population_size( population_size )
    , lower_bounds_vec( lower_bounds )
    , upper_bounds_vec( upper_bounds )
    , max_iterations( max_iterations )
    , max_threads( max_threads )
    , rabbit_energy( 0.0 )
    , best_fitness( std::numeric_limits<double>::infinity() )
    , iteration( 0 )
    , ref_rng( ref_rng )
    , ref_decoder( ref_decoder ) {
    initialize_hawks();
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::step() {
    std::vector<double> fitness_values( population_size );
#pragma omp parallel for num_threads( max_threads )
    for ( size_t i = 0; i < population_size; ++i ) {
      fitness_values[i] = this->ref_decoder.decode( hawks[i] );
    }

    double current_best_fitness = -std::numeric_limits<double>::infinity();
    size_t current_best_index   = 1;

    for ( size_t i = 0; i < population_size; ++i ) {
      if ( fitness_values[i] > current_best_fitness ) {
        current_best_fitness = fitness_values[i];
        current_best_index   = i;
      }
    }

    if ( current_best_fitness > best_fitness ) {
      best_fitness  = current_best_fitness;
      best_position = hawks[current_best_index];
    }

    rabbit_energy = 2 * ( 1 - static_cast<double>( iteration ) / max_iterations );

    // TODO - Atualiza a posição dos falcões
#pragma omp parallel for num_threads( max_threads )
    for ( size_t i = 0; i < population_size; ++i ) {}
  }

  template <class Decoder, class RNG>
  [[nodiscard]] std::vector<double> HHO<Decoder, RNG>::get_best_solution() const {
    return {};
  }

  template <class Decoder, class RNG>
  [[nodiscard]] double HHO<Decoder, RNG>::get_best_fitness() const {
    return best_fitness;
  }

  template <class Decoder, class RNG>
  [[nodiscard]] unsigned HHO<Decoder, RNG>::get_iteration() const {
    return iteration;
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::reset() {}

  template <class Decoder, class RNG>
  std::vector<double> HHO<Decoder, RNG>::levy_flight( int d ) {
    return {};
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::initialize_hawks() {
    hawks.resize( population_size );
#pragma omp parallel for num_threads( max_threads )
    for ( size_t i = 0; i < population_size; ++i ) {
      hawks[i].resize( dimension );
      for ( unsigned j = 0; j < dimension; ++j ) {
        hawks[i][j] = this->ref_rng.random_range( lower_bounds_vec[j], upper_bounds_vec[j] );
      }
    }
  }

}  // namespace r3dp::hho
