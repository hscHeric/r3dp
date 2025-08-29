#pragma once

#include <vector>

namespace r3dp::brkga {
  class Population {
    template <class Decoder, class RNG>
    friend class BRKGA;

  public:
    unsigned getN() const;  // Size of each chromosome
    unsigned getP() const;  // Size of population

    // These methods REQUIRE fitness to be sorted, and thus a call to sortFitness() beforehand
    // (this is done by BRKGA, so rest assured: everything will work just fine with BRKGA).
    // Returns the best fitness in this population:
    double getBestFitness() const;

    // Returns the fitness of chromosome i \in {0, ..., getP() - 1}
    double getFitness( unsigned i ) const;

    // Returns (i+1)-th best chromosome, where i = 0 is the best and i = getP() - 1 is the worst:
    const std::vector<double> &getChromosome( unsigned i ) const;

  private:
    Population( const Population &other );
    Population( unsigned n, unsigned p );
    ~Population();

    std::vector<std::vector<double>>         population;  // Population as vectors of prob.
    std::vector<std::pair<double, unsigned>> fitness;     // Fitness (double) of a each chromosome

    void                 sortFitness();  // Sorts 'fitness' by its first parameter
    void                 setFitness( unsigned i, double f );  // Sets the fitness of chromosome i
    std::vector<double> &getChromosome( unsigned i );         // Returns a chromosome

    double &operator()( unsigned i, unsigned j );   // Direct access to allele j of chromosome i
    std::vector<double> &operator()( unsigned i );  // Direct access to chromosome i
  };
}  // namespace r3dp::brkga
