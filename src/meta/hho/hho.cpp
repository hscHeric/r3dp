#pragma once

#include "hho.hpp"

#include <cmath>
#include <limits>

namespace r3dp::hho {

  template <core::problem problem_type>
  HHO<problem_type>::HHO( size_t        population_size,
                          uint32_t      max_iterations,
                          uint32_t      max_threads,
                          problem_type &problem,
                          core::RNG<>  &rng )
    : population_size( population_size )
    , max_iterations( max_iterations )
    , max_threads( max_threads )
    , problem( problem )
    , rng( rng )
    , dimension( problem.dimension() )
    , lower_bounds( problem.lower_bounds() )
    , upper_bounds( problem.upper_bounds() )
    , hawks( population_size, std::vector<double>( dimension, 0.0 ) )
    , rabbit_position( dimension, 0.0 )
    , best_fitness( std::numeric_limits<double>::infinity() )
    , iteration( 0 ) {
    initialize_hawks();
  }

  template <core::problem problem_type>
  void HHO<problem_type>::reset() {
    iteration    = 0;
    best_fitness = std::numeric_limits<double>::infinity();
    rabbit_position.assign( dimension, 0.0 );
    initialize_hawks();
  }

  template <core::problem problem_type>
  void HHO<problem_type>::initialize_hawks() {
    for ( auto &hawk : hawks ) {
      for ( size_t d = 0; d < dimension; ++d ) {
        hawk[d] = rng.random_range( lower_bounds[d], upper_bounds[d] );
      }

      double fitness = problem.evaluate( hawks );
      if ( fitness > best_fitness ) {
        best_fitness    = fitness;
        rabbit_position = hawk;
      }
    }
  }

  template <core::problem problem_type>
  std::vector<double> HHO<problem_type>::levy_flight( size_t d ) {
    constexpr double beta = 1.5;
    double           sigma_u =
      std::pow( ( tgamma( 1 + beta ) * std::sin( M_PI * beta / 2 ) ) /
                  ( tgamma( ( 1 + beta ) / 2 ) * beta * std::pow( 2, ( beta - 1 ) / 2 ) ),
                1.0 / beta );

    std::vector<double> steps( d );
    for ( size_t i = 0; i < d; ++i ) {
      double u    = rng.normal( 0, sigma_u );
      double v    = rng.normal( 0, 1.0 );
      double step = u / std::pow( std::fabs( v ), 1.0 / beta );
      steps[i]    = step;
    }
    return steps;
  }

  template <core::problem problem_type>
  void HHO<problem_type>::step() {
    // TODO: Implementar uma iteração do algoritmo
  }

  template <core::problem problem_type>
  const std::vector<double> &HHO<problem_type>::get_best_solution() const {
    return rabbit_position;
  }

  template <core::problem problem_type>
  double HHO<problem_type>::get_best_fitness() const {
    return best_fitness;
  }

  template <core::problem problem_type>
  size_t HHO<problem_type>::get_iteration() const {
    return iteration;
  }

}  // namespace r3dp::hho
