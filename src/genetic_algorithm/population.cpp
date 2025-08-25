#include "population.hpp"

#include <algorithm>
#include <stdexcept>

namespace r3dp::ga {

  Population::Population( const std::vector<std::vector<uint8_t>> &initialPopulation )
    : population( initialPopulation ), fitness( initialPopulation.size() ) {
    if ( population.empty() ) {
      throw std::range_error( "A população inicial não pode ser vázia" );
    }
    if ( population[0].empty() ) {
      throw std::range_error( "Chromossomos não podem ser vázios" );
    }
  }

  Population::Population( const Population &pop )
    : population( pop.population ), fitness( pop.fitness ) {}

  unsigned Population::chromosome_size() const {
    return population[0].size();
  }

  unsigned Population::size() const {
    return population.size();
  }

  double Population::get_best_fitness() const {
    return get_fitness( 0 );
  }

  double Population::get_fitness( unsigned i ) const {
    if ( i >= size() ) {
      throw std::range_error( "[get_fitness] individuo fora dos limites" );
    }
    return fitness[i].first;
  }

  const std::vector<uint8_t> &Population::get_chromosome( unsigned i ) const {
    if ( i >= size() ) {
      throw std::range_error( "[get_chromosome] individuo fora dos limites" );
    }
    return population[fitness[i].second];
  }

  std::vector<uint8_t> &Population::get_chromosome_at( unsigned i ) {
    if ( i >= size() ) {
      throw std::range_error( "[get_chromosome_at] individuo fora dos limites" );
    }
    return population[i];
  }

  void Population::set_fitness( unsigned i, double f ) {
    fitness[i].first  = f;
    fitness[i].second = i;
  }

  void Population::sort_fitness() {
    std::sort( fitness.begin(), fitness.end() );
  }
}  // namespace r3dp::ga
