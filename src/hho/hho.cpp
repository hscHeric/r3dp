#include "hho.hpp"

#include <vector>

namespace r3dp::hho {
  template <class Decoder>
  void HHO<Decoder>::initialize_hawks() {
    hawks.resize( population_size, std::vector<double>( dimension ) );

#pragma omp parallel for num_threads( max_threads )
    for ( unsigned i = 0; i < population_size; ++i ) {
      for ( unsigned j = 0; j < dimension; ++j ) {
        hawks[i][j] = ref_rng.random_range<double>( lower_bound, upper_bound );
      }
    }
  }

  template <class Decoder>
  HHO<Decoder>::HHO( const unsigned population_size,
                     const unsigned dimension,
                     const double   lower_bound,
                     const double   upper_bound,
                     const unsigned max_threads,
                     const Decoder &ref_decoder,
                     core::RNG     &ref_rng )
    : population_size( population_size )
    , dimension( dimension )
    , lower_bound( lower_bound )
    , upper_bound( upper_bound )
    , max_threads( max_threads )
    , ref_decoder( ref_decoder )
    , ref_rng( ref_rng )
    , iteration( 0 )
    , best_fitness( std::numeric_limits<double>::infinity() ) {
    initialize_hawks();  // Inicializa o a população com vetores de tamanho dimension com valores
                         // aleatorios entre lower_bound e upper_bound
  }
}  // namespace r3dp::hho
