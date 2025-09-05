
#include "hho.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace r3dp::hho {
  template <class Decoder, class RNG>
  HHO<Decoder, RNG>::HHO( const unsigned population_size,
                          const unsigned dimension,
                          const double   lower_bound,
                          const double   upper_bound,
                          const unsigned max_threads,
                          const Decoder &ref_decoder,
                          RNG           &ref_rng )
    : dimension( dimension )
    , population_size( population_size )
    , lower_bounds_vec( dimension, lower_bound )
    , upper_bounds_vec( dimension, upper_bound )
    , max_threads( max_threads )
    , best_fitness( std::numeric_limits<double>::infinity() )
    , iteration( 0 )
    , ref_rng( ref_rng )
    , ref_decoder( ref_decoder ) {
    initialize_hawks();
  }

  template <class Decoder, class RNG>
  HHO<Decoder, RNG>::HHO( const unsigned             population_size,
                          const unsigned             dimension,
                          const std::vector<double> &lower_bounds,
                          const std::vector<double> &upper_bounds,
                          const unsigned             max_threads,
                          const Decoder             &ref_decoder,
                          RNG                       &ref_rng )
    : dimension( dimension )
    , population_size( population_size )
    , lower_bounds_vec( lower_bounds )
    , upper_bounds_vec( upper_bounds )
    , max_threads( max_threads )
    , best_fitness( std::numeric_limits<double>::infinity() )
    , iteration( 0 )
    , ref_rng( ref_rng )
    , ref_decoder( ref_decoder ) {
    if ( lower_bounds_vec.size() != dimension || upper_bounds_vec.size() != dimension ) {
      throw std::invalid_argument( "Os vetores de limites devem ter o mesmo tamanho da dimensão." );
    }
    initialize_hawks();
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::step() {
    double              rabbit_energy = std::numeric_limits<double>::infinity();
    std::vector<double> rabbit_location( dimension );
    std::vector<double> fitness_values( population_size );

#pragma omp parallel for num_threads( max_threads )
    for ( size_t i = 0; i < population_size; ++i ) {
      fitness_values[i] = ref_decoder.decode( hawks[i] );
    }

    for ( size_t i = 0; i < population_size; ++i ) {
      if ( rabbit_energy < fitness_values[i] ) {
        rabbit_energy   = fitness_values[i];
        rabbit_location = hawks[i];
      }
    }
  }

  template <class Decoder, class RNG>
  std::vector<double> HHO<Decoder, RNG>::get_best_solution() const {
    return best_position;
  }

  template <class Decoder, class RNG>
  double HHO<Decoder, RNG>::get_best_fitness() const {
    return best_fitness;
  }

  template <class Decoder, class RNG>
  unsigned HHO<Decoder, RNG>::get_iteration() const {
    return iteration;
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::reset() {}

  template <class Decoder, class RNG>
  std::vector<double> HHO<Decoder, RNG>::levy_flight( int d ) {}

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::initialize_hawks() {
    hawks.resize( population_size );

#pragma omp parallel for num_threads( max_threads )
    for ( unsigned i = 0; i < population_size; ++i ) {
      hawks[i].resize( dimension );
      for ( unsigned j = 0; j < dimension; ++j ) {
        hawks[i][j] = this->ref_rng.random_range( lower_bounds_vec[j], upper_bounds_vec[j] );
      }
    }
  }
}  // namespace r3dp::hho
