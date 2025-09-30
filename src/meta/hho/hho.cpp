#include "hho.hpp"

#include <cstdlib>
#include <stdexcept>

namespace r3dp::hho {

  // Construtor com seed opcional
  template <hho_problem problem_type>
  HHO<problem_type>::HHO( size_t              population_size,
                          size_t              max_iterations,
                          uint32_t            max_threads,
                          const problem_type &ref_problem,
                          uint32_t            seed )
    : population_size( population_size )
    , max_iterations( max_iterations )
    , max_threads( max_threads )
    , ref_problem( ref_problem )
    , rng( seed == 0 ? std::random_device{}() : seed ) {
    // Verificações de limites
    if ( population_size == 0 ) {
      throw std::invalid_argument( "population_size deve ser maior que 0" );
    }

    if ( max_iterations == 0 ) {
      throw std::invalid_argument( "max_iterations deve ser maior que 0" );
    }

    if ( max_threads == 0 ) {
      throw std::invalid_argument( "max_threads deve ser maior que 0" );
    }

    hawks.resize( population_size, std::vector<double>( ref_problem.get_dimensions(), 0.0 ) );
    rabbit_position.resize( ref_problem.get_dimensions(), 0.0 );

    rabbit_fitness = std::numeric_limits<double>::infinity();  // Fitness como infinito, pois o
                                                               // problema é de minimização

    initialize_hawks();
  }

  template <hho_problem problem_type>
  HHO<problem_type>::HHO( size_t              population_size,
                          size_t              max_iterations,
                          uint32_t            max_threads,
                          const problem_type &ref_problem )
    : HHO( population_size, max_iterations, max_threads, ref_problem, 0 ) {}

  template <hho_problem problem_type>
  void HHO<problem_type>::initialize_hawks() {
    const auto &lb = ref_problem.get_lower_bounds();
    const auto &ub = ref_problem.get_upper_bounds();

    for ( size_t i = 0; i < population_size; ++i ) {
      for ( size_t j = 0; j < ref_problem.get_dimension(); ++j ) {
        std::uniform_real_distribution<double> dist( lb[j], ub[j] );
        hawks[i][j] = dist( rng );  // Preenche cada variavel com valors lb =< x >= ub
      }
    }
  }

  template <hho_problem problem_type>
  void HHO<problem_type>::step() {
    /**
     * Vou implementar duas funções, uma em que vou calcular tudo exceto o evaluate de forma
     * sequencial, e vou implementar uma precomputando tudo que usa rng para manter o determinismo e
     * paralelizar o loop principal com omp, e verificar qual é mais rapido, se pré computar todos
     * os números paralelamente e paralelizar o loop principal for melhor, vou manter essa
     * estratégia
     * */
    if ( iteration >= max_iterations ) {
      return;
    }

    const auto dimension        = ref_problem.get_dimension();
    const auto lower_bounds_vec = ref_problem.get_lower_bounds();
    const auto upper_bounds_vec = ref_problem.get_upper_bounds();

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
      fitness_values[i] = ref_problem.evaluate( hawks[i] );
    }

    double current_best_fitness = std::numeric_limits<double>::infinity();
    size_t current_best_index   = 0;
    for ( size_t i = 0; i < population_size; ++i ) {
      if ( fitness_values[i] < current_best_fitness ) {
        current_best_fitness = fitness_values[i];
        current_best_index   = i;
      }
    }
    if ( current_best_fitness < rabbit_fitness ) {
      rabbit_fitness  = current_best_fitness;
      rabbit_position = hawks[current_best_index];
    }

    auto E1 =
      2.0 * ( 1.0 - ( static_cast<double>( iteration ) / static_cast<double>( max_iterations ) ) );
    std::uniform_real_distribution<double> dist( -1.0, 1.0 );
    std::mt19937_64                        rng_local( rng() );

    for ( size_t i = 0; i < population_size; ++i ) {
      auto   E0              = dist( rng_local );
      double escaping_energy = E1 * E0;

      if ( std::abs( escaping_energy ) >= 1 ) {
        // TODO: Fase de exploração
      } else {
        // TODO: Fase de explotação
      }
    }
  }

  template <hho_problem problem_type>
  void HHO<problem_type>::step_parallel() {
    // Lógica do passo paralelizado da otimização (a ser implementada)
  }

  template <hho_problem problem_type>
  const std::vector<double> &HHO<problem_type>::get_best_solution() const {
    return rabbit_position;
  }

  template <hho_problem problem_type>
  double HHO<problem_type>::get_best_fitness() const {
    return rabbit_fitness;
  }

  template <hho_problem problem_type>
  size_t HHO<problem_type>::get_iteration() const {
    return iteration;
  }

  template <hho_problem problem_type>
  void HHO<problem_type>::reset() {
    iteration = 0;
    hawks.clear();
    rabbit_position.clear();
    rabbit_fitness = std::numeric_limits<double>::infinity();
    initialize_hawks();
  }
}  // namespace r3dp::hho
