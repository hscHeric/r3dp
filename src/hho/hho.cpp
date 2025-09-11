#include "hho.hpp"

#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <vector>

namespace r3dp::hho {

  template <class Decoder>
  HHO<Decoder>::HHO( unsigned       population_size,
                     unsigned       dimension,
                     double         lower_bound,
                     double         upper_bound,
                     unsigned       max_iterations,
                     unsigned       max_threads,
                     const Decoder &ref_decoder,
                     core::RNG<>   &ref_rng )
    : dimension( dimension )
    , population_size( population_size )
    , max_iterations( max_iterations )
    , max_threads( max_threads )
    , ref_rng( ref_rng )
    , ref_decoder( ref_decoder ) {
    if ( population_size == 0 ) {
      throw std::invalid_argument( "Population size must be greater than 0." );
    }
    if ( dimension == 0 ) {
      throw std::invalid_argument( "Dimension must be greater than 0." );
    }
    if ( lower_bound > upper_bound ) {
      throw std::invalid_argument( "Lower bound cannot be greater than upper bound." );
    }
    if ( max_iterations == 0 ) {
      throw std::invalid_argument( "Max iterations must be greater than 0." );
    }

    // Inicialização dos vetores de limites
    lower_bounds_vec.assign( dimension, lower_bound );
    upper_bounds_vec.assign( dimension, upper_bound );

    best_fitness = std::numeric_limits<double>::infinity();
    iteration    = 0;
  }

  template <class Decoder>
  HHO<Decoder>::HHO( unsigned                   population_size,
                     unsigned                   dimension,
                     const std::vector<double> &lower_bounds,
                     const std::vector<double> &upper_bounds,
                     unsigned                   max_iterations,
                     unsigned                   max_threads,
                     const Decoder             &ref_decoder,
                     core::RNG<>               &ref_rng )
    : dimension( dimension )
    , population_size( population_size )
    , lower_bounds_vec( lower_bounds )
    , upper_bounds_vec( upper_bounds )
    , max_iterations( max_iterations )
    , max_threads( max_threads )
    , ref_rng( ref_rng )
    , ref_decoder( ref_decoder ) {
    if ( population_size == 0 ) {
      throw std::invalid_argument( "Population size must be greater than 0." );
    }
    if ( dimension == 0 ) {
      throw std::invalid_argument( "Dimension must be greater than 0." );
    }
    if ( lower_bounds.size() != dimension || upper_bounds.size() != dimension ) {
      throw std::invalid_argument( "Bounds vectors must match the specified dimension." );
    }
    for ( size_t i = 0; i < dimension; ++i ) {
      if ( lower_bounds[i] > upper_bounds[i] ) {
        throw std::invalid_argument(
          "Lower bound cannot be greater than upper bound for dimension " + std::to_string( i ) +
          "." );
      }
    }
    if ( max_iterations == 0 ) {
      throw std::invalid_argument( "Max iterations must be greater than 0." );
    }

    best_fitness = std::numeric_limits<double>::infinity();
    iteration    = 0;
  }

  template <class Decoder>
  const std::vector<double> &HHO<Decoder>::get_best_solution() const {
    return best_position;
  }

  template <class Decoder>
  double HHO<Decoder>::get_best_fitness() const {
    return best_fitness;
  }

  template <class Decoder>
  const std::vector<double> &HHO<Decoder>::get_convergence_curve() const {
    return convergence_curve;
  }

  template <class Decoder>
  unsigned HHO<Decoder>::get_iteration() const {
    return iteration;
  }

  template <class Decoder>
  void HHO<Decoder>::reset() {
    hawks.clear();
    best_position.clear();
    best_fitness = std::numeric_limits<double>::infinity();
    iteration    = 0;
    convergence_curve.clear();
    initialize_hawks();
  }

  template <class Decoder>
  void HHO<Decoder>::initialize_hawks() {
    hawks.assign( population_size, std::vector<double>( dimension, 0.0 ) );
    for ( size_t i = 0; i < population_size; ++i ) {
      for ( size_t j = 0; j < dimension; ++j ) {
        hawks[i][j] = this->ref_rng.random_range( lower_bounds_vec[j], upper_bounds_vec[j] );
      }
    }
  }

  template <class Decoder>
  void HHO<Decoder>::step() {
    // TODO: Onde usar rng tem que ser sequencial
  }

}  // namespace r3dp::hho
