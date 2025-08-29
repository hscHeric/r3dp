

#include "population.hpp"

#include <algorithm>
#include <stdexcept>

namespace r3dp::brkga {
  Population::Population( const Population &pop )
    : population( pop.population ), fitness( pop.fitness ) {}

  Population::Population( const unsigned n, const unsigned p )
    : population( p, std::vector<double>( n, 0.0 ) ), fitness( p ) {
    if ( p == 0 ) {
      throw std::range_error( "Population size p cannot be zero." );
    }
    if ( n == 0 ) {
      throw std::range_error( "Chromosome size n cannot be zero." );
    }
  }

  Population::~Population() {}

  unsigned Population::getN() const {
    return population[0].size();
  }

  unsigned Population::getP() const {
    return population.size();
  }

  double Population::getBestFitness() const {
    return getFitness( 0 );
  }

  double Population::getFitness( unsigned i ) const {
#ifdef RANGECHECK
    if ( i >= getP() ) {
      throw std::range_error( "Invalid individual identifier." );
    }
#endif
    return fitness[i].first;
  }

  const std::vector<double> &Population::getChromosome( unsigned i ) const {
#ifdef RANGECHECK
    if ( i >= getP() ) {
      throw std::range_error( "Invalid individual identifier." );
    }
#endif

    return population[fitness[i].second];
  }

  std::vector<double> &Population::getChromosome( unsigned i ) {
#ifdef RANGECHECK
    if ( i >= getP() ) {
      throw std::range_error( "Invalid individual identifier." );
    }
#endif

    return population[fitness[i].second];
  }

  void Population::setFitness( unsigned i, double f ) {
    fitness[i].first  = f;
    fitness[i].second = i;
  }

  void Population::sortFitness() {
    sort( fitness.begin(), fitness.end() );
  }

  double &Population::operator()( unsigned chromosome, unsigned allele ) {
    return population[chromosome][allele];
  }

  std::vector<double> &Population::operator()( unsigned chromosome ) {
    return population[chromosome];
  }
}  // namespace r3dp::brkga
