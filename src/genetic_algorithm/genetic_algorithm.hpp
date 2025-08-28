#pragma once

#include "population.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace r3dp::ga {
  template <typename RNG, typename Decoder>
  class GAEngine {
  public:
    GAEngine( const std::vector<Chromosome> &initial_population,
              double                         mutation_rate,
              double                         crossover_rate,
              double                         elite_rate,
              unsigned int                   tournament_size,
              unsigned int                   max_threads,
              RNG                           &rng,
              Decoder                       &decoder );

    std::pair<Chromosome, double> evolve();

    const Chromosome &get_best_chromosome() const;

    double best_fitness() const;

    const Population &population() const;

  private:
    void         one_point_crossover( Chromosome       &offspring,
                                      const Chromosome &parent1,
                                      const Chromosome &parent2 );
    void         mutation( Chromosome &chromosome );
    unsigned int tournament_selection() const;
    void         seed_population( const std::vector<Chromosome> &initial_population );

    const double       mutation_rate;
    const double       crossover_rate;
    const double       elite_rate;
    const unsigned int tournament_size;
    const unsigned int max_threads;

    RNG     &rng;
    Decoder &decoder;

    std::unique_ptr<Population> population_current;
    std::unique_ptr<Population> population_next;
  };

  template <typename RNG, typename Decoder>
  GAEngine<RNG, Decoder>::GAEngine( const std::vector<Chromosome> &initial_population,
                                    double                         mutation_rate,
                                    double                         crossover_rate,
                                    double                         elite_rate,
                                    unsigned int                   tournament_size,
                                    unsigned int                   max_threads,
                                    RNG                           &rng,
                                    Decoder                       &decoder )
    : mutation_rate( mutation_rate )
    , crossover_rate( crossover_rate )
    , elite_rate( elite_rate )
    , tournament_size( tournament_size )
    , max_threads( max_threads )
    , rng( rng )
    , decoder( decoder ) {
    if ( initial_population.empty() || initial_population[0].empty() ) {
      throw std::invalid_argument( "A população inicial não pode ser vazia" );
    }
    if ( crossover_rate < 0.0 || crossover_rate > 1.0 ) {
      throw std::invalid_argument( "Crossover rate deve ser entre 0.0 e 1.0" );
    }
    if ( mutation_rate < 0.0 || mutation_rate > 1.0 ) {
      throw std::invalid_argument( "Mutation rate deve ser entre 0.0 e 1.0" );
    }
    if ( elite_rate < 0.0 || elite_rate > 1.0 ) {
      throw std::invalid_argument( "Elite rate deve ser entre 0.0 e 1.0" );
    }
    if ( tournament_size == 0 ) {
      throw std::invalid_argument( "Tournament size deve ser maior do que zero" );
    }

    seed_population( initial_population );
  }

  template <typename RNG, typename Decoder>
  void
    GAEngine<RNG, Decoder>::seed_population( const std::vector<Chromosome> &initial_population ) {
    population_current = std::make_unique<Population>( initial_population );
    population_next    = std::make_unique<Population>( *population_current );

#pragma omp parallel for num_threads( max_threads )
    for ( int i = 0; i < population_current->size(); ++i ) {
      population_current->set_fitness(
        i, decoder.decode( population_current->get_chromosome_at( i ) ) );
    }
    population_current->sort_fitness();
  }

  template <typename RNG, typename Decoder>
  std::pair<Chromosome, double> GAEngine<RNG, Decoder>::evolve() {
    const unsigned int pop_size = population_current->size();

    unsigned int elite_count = 0;
    if ( elite_rate > 0.0 ) {
      const double raw = elite_rate * static_cast<double>( pop_size );
      elite_count      = static_cast<unsigned int>( std::floor( raw ) );
      if ( elite_count == 0 && pop_size > 0 ) {
        elite_count = 1;
      }
      if ( elite_count > pop_size ) {
        elite_count = pop_size;
      }
    }

    for ( unsigned int e = 0; e < elite_count; ++e ) {
      const auto &elite_chrom                 = population_current->get_chromosome( e );
      const auto  elite_fit                   = population_current->get_fitness( e );
      population_next->get_chromosome_at( e ) = elite_chrom;
      population_next->set_fitness( e, elite_fit );
    }

#pragma omp parallel for num_threads( max_threads )
    for ( int i = elite_count; i < pop_size; ++i ) {
      unsigned int parent1_idx = tournament_selection();
      unsigned int parent2_idx = tournament_selection();
      while ( parent1_idx == parent2_idx ) {
        parent2_idx = tournament_selection();
      }

      const Chromosome &parent1 = population_current->get_chromosome( parent1_idx );
      const Chromosome &parent2 = population_current->get_chromosome( parent2_idx );
      Chromosome        offspring;

      if ( rng.rand() < crossover_rate ) {
        one_point_crossover( offspring, parent1, parent2 );
      } else {
        offspring = ( rng.rand() < 0.5 ) ? parent1 : parent2;
      }

      mutation( offspring );

      population_next->get_chromosome_at( i ) = offspring;
    }

#pragma omp parallel for num_threads( max_threads )
    for ( int i = elite_count; i < pop_size; ++i ) {
      population_next->set_fitness( i, decoder.decode( population_next->get_chromosome_at( i ) ) );
    }

    population_next->sort_fitness();
    std::swap( population_current, population_next );

    return { get_best_chromosome(), best_fitness() };
  }

  template <typename RNG, typename Decoder>
  const Chromosome &GAEngine<RNG, Decoder>::get_best_chromosome() const {
    return population_current->get_chromosome( 0 );
  }

  template <typename RNG, typename Decoder>
  double GAEngine<RNG, Decoder>::best_fitness() const {
    return population_current->get_best_fitness();
  }

  template <typename RNG, typename Decoder>
  const Population &GAEngine<RNG, Decoder>::population() const {
    return *population_current;
  }

  template <typename RNG, typename Decoder>
  unsigned int GAEngine<RNG, Decoder>::tournament_selection() const {
    unsigned int best_idx = rng.randInt( population_current->size() - 1 );
    for ( unsigned int i = 1; i < tournament_size; ++i ) {
      unsigned int candidate_idx = rng.randInt( population_current->size() - 1 );
      if ( population_current->get_fitness( candidate_idx ) <
           population_current->get_fitness( best_idx ) ) {
        best_idx = candidate_idx;
      }
    }
    return best_idx;
  }

  template <typename RNG, typename Decoder>
  void GAEngine<RNG, Decoder>::one_point_crossover( Chromosome       &offspring,
                                                    const Chromosome &parent1,
                                                    const Chromosome &parent2 ) {
    const unsigned int chromosome_size = population_current->chromosome_size();
    const unsigned int crossover_point =
      ( chromosome_size > 1 ) ? rng.randInt( chromosome_size - 1 ) : 0;

    offspring.resize( chromosome_size );
    for ( unsigned int i = 0; i < chromosome_size; ++i ) {
      offspring[i] = ( i < crossover_point ) ? parent1[i] : parent2[i];
    }
  }

  template <typename RNG, typename Decoder>
  void GAEngine<RNG, Decoder>::mutation( Chromosome &chromosome ) {
    for ( unsigned int i = 0; i < chromosome.size(); ++i ) {
      if ( rng.rand() < mutation_rate ) {
        chromosome[i] = rng.randInt( 3 );
      }
    }
  }
}  // namespace r3dp::ga
