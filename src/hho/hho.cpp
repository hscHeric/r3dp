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
    if ( iteration >= max_iterations ) {
      return;  // Criterio de parada do algoritmo
    }

    // Restringe as variaveis de toda a população de soluções aos limitantes superiores e inferiores
    // passados

    // clang-format off
    #pragma omp parallel for num_threads( max_threads ) default( none )  // clang-format on
    for ( size_t i = 0; i < population_size; ++i ) {
      for ( unsigned j = 0; j < dimension; ++j ) {
        hawks[i][j] = std::clamp( hawks[i][j], lower_bounds_vec[j], upper_bounds_vec[j] );
      }
    }

    std::vector<double> fitness_values( population_size, std::numeric_limits<double>::infinity() );
    // clang-format off
    #pragma omp parallel for num_threads( max_threads ) default( none )  // clang-format on
    for ( size_t i = 0; i < population_size; ++i ) {
      fitness_values[i] = this->ref_decoder.decode(
        hawks[i] );  // aqui estou paralelizando partindo do pré-suporto que o decoder é
                     // thread-safe, acho que vou implementar o decoder de forma que ele não
                     // modifique as solução inválidas, mas na verdade incremente o fitness como se
                     // fosse de uma solução válide. por exemplo, suponha um vértice v com f[v] = 1,
                     // v é invalido pela definição do r3dp, se falta um valor n de rotulos na
                     // vizinhança de v, então o vértice v vai aumentar o fitness em n ao invés de
                     // aumentar em 1, assim não vou modificar os falcoes e vou ter um fitness
                     // relativo a solução válida
    }

    // pegando o melhor fitness atual das soluções
    double current_best_fitness = std::numeric_limits<double>::infinity();
    size_t current_best_index   = 0;
    for ( size_t i = 0; i < population_size; ++i ) {
      if ( fitness_values[i] < current_best_fitness ) {
        current_best_fitness = fitness_values[i];
        current_best_index   = i;
      }
    }
    if ( current_best_fitness < best_fitness ) {
      best_fitness  = current_best_fitness;
      best_position = hawks[current_best_index];
    }

    const double E1 = JUMP_STRENGTH_FACTOR *
                      ( ESCAPE_ENERGY_BOUND_EXPLORATION -
                        static_cast<double>( iteration ) / static_cast<double>( max_iterations ) );

    std::vector<double> mean_pos( dimension, 0.0 );
    for ( unsigned j = 0; j < dimension; ++j ) {
      double acc = 0.0;
      for ( size_t i = 0; i < population_size; ++i ) {
        acc += hawks[i][j];
      }
      mean_pos[j] = acc / static_cast<double>( population_size );
    }

    const auto hawks_prev = hawks;

    // geração de todos os números aleatorios usados
    std::vector<double> E0_vec( population_size );
    std::vector<double> q_vec( population_size );
    std::vector<double> r1_vec( population_size );
    std::vector<double> r2_vec( population_size );
    std::vector<double> J_vec( population_size );
    std::vector<size_t> rand_idx( population_size );

    for ( size_t i = 0; i < population_size; ++i ) {
      const double u0 = this->ref_rng.random_range( 0.0, 1.0 );
      E0_vec[i]       = ( JUMP_STRENGTH_FACTOR * u0 ) - ESCAPE_ENERGY_BOUND_EXPLORATION;

      q_vec[i]  = this->ref_rng.random_range( 0.0, 1.0 );
      r1_vec[i] = this->ref_rng.random_range( 0.0, 1.0 );
      r2_vec[i] = this->ref_rng.random_range( 0.0, 1.0 );

      J_vec[i] = JUMP_STRENGTH_FACTOR * ( 1.0 - this->ref_rng.random_range( 0.0, 1.0 ) );

      if ( population_size > 1 ) {
        auto r = static_cast<size_t>( this->ref_rng.random_range( 0.0, 1.0 ) * population_size );
        if ( r == i ) {
          r = ( r + 1 ) % population_size;
        }
        rand_idx[i] = r;
      } else {
        rand_idx[i] = i;
      }
    }
    /**
     * Loop principal, aqui o loop é divido em três fases
     *
     * A fase de exploração vai acontecer quando a energia de escape do coelho é >= que 1 nesse caso
     * vão se ter duas estrategias de modificação do falção escolhidas a partir da geração de um
     * número aleatorio q, caso a energia de escape do coelho tenha acabado, então se entra na fase
     * de transição
     */
    // clang-format off
    #pragma omp parallel for num_threads( max_threads ) default( none )  // clang-format on
    for ( size_t i = 0; i < population_size; ++i ) {
      // TODO: Atualiza todas as posições
      const double escaping_energy = E1 * E0_vec[i];

      std::vector<double> next( dimension );

      if ( std::abs( escaping_energy ) >= 1 ) {
        // TODO: Fase de exploração
      } else {
        // TODO: Fases de transição e de exploitation
      }
    }
  }

}  // namespace r3dp::hho
