#include "hho.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
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

    best_fitness = std::numeric_limits<double>::infinity();  // o r3dp é de minimização
    iteration    = 0;
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

  template <class Decoder, class RNG>
  const std::vector<double> &HHO<Decoder, RNG>::get_best_solution() const {
    return best_position;
  }

  template <class Decoder, class RNG>
  double HHO<Decoder, RNG>::get_best_fitness() const {
    return best_fitness;
  }

  template <class Decoder, class RNG>
  const std::vector<double> &HHO<Decoder, RNG>::get_convergence_curve() const {
    return convergence_curve;
  }

  template <class Decoder, class RNG>
  unsigned HHO<Decoder, RNG>::get_iteration() const {
    return iteration;
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::reset() {
    hawks.clear();
    best_position.clear();
    best_fitness = std::numeric_limits<double>::infinity();
    iteration    = 0;
    convergence_curve.clear();
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::initialize_hawks() {
    hawks.resize( population_size );

    // clang-format off
    #pragma omp parallel for num_threads( max_threads ) default(none)  // clang-format on
    for ( size_t i = 0; i < population_size; ++i ) {
      hawks[i].resize( dimension );
      for ( unsigned j = 0; j < dimension; ++j ) {
        hawks[i][j] = this->ref_rng.random_range( lower_bounds_vec[j], upper_bounds_vec[j] );
      }
    }
  }

  template <class Decoder, class RNG>
  void HHO<Decoder, RNG>::step() {
    // Verifica o criterio de parada baseado no número máximo de iterações
    // Aqui o algoritmo utiliza o número máximo de iterações como um parâmetro para ele
    // logo acredito que não é possivel definir todo o algoritmo apenas definindo um outro criterio
    // de parada como tempo limite (lembrar de testar depois colocando o max_iterations = tempo
    // máximo em segundos) assim removendo a necessidade de definir um max_iterations para os testes
    if ( iteration >= max_iterations ) {
      return;  // não fazer nada pois já atingiu o limite;
    }

    // clang-format off
    #pragma omp parallel for num_threads( max_threads ) default(none)  // clang-format on
    for ( size_t i = 0; i < population_size; ++i ) {
      for ( unsigned j = 0; j < dimension; ++j ) {
        hawks[i][j] = std::clamp( hawks[i][j], lower_bounds_vec[j], upper_bounds_vec[j] );
      }
    }

    std::vector<double> fitness_values( population_size, std::numeric_limits<double>::infinity() );
    // clang-format off
    #pragma omp parallel for num_threads( max_threads )  // clang-format on
    for ( size_t i = 0; i < population_size; ++i ) {
      fitness_values[i] = this->ref_decoder.decode( hawks[i] );
    }

    double current_best_fitness = std::numeric_limits<double>::infinity();
    size_t current_best_index   = 0;
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

    auto E1 = JUMP_STRENGTH_FACTOR * ( ESCAPE_ENERGY_BOUND_EXPLORATION -
                                       static_cast<double>( iteration ) / max_iterations );

    // clang-format off
    #pragma omp parallel for num_threads(max_threads)  // clang-format on
    for ( size_t i = 0; i < population_size; ++i ) {
      // energia de escape do coelho
      double E0 = ( JUMP_STRENGTH_FACTOR * ref_rng.random_range( 0.0, 1.0 ) ) -
                  ESCAPE_ENERGY_BOUND_EXPLORATION;
      double escaping_energy = E1 * E0;
    }
  }
}  // namespace r3dp::hho
