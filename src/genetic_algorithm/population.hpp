#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace r3dp::ga {

  using Chromosome = std::vector<uint8_t>;
  using Fitness    = uint64_t;

  class Population {
  public:
    /**
     * @brief Construtor para inicializar uma população com cromossomos predefinidos.
     * @param initialPopulation O vetor de cromossomos para a população inicial.
     */
    Population( const std::vector<Chromosome> &initialPopulation );

    /**
     * @brief Construtor de cópia.
     * @param other A população a ser copiada.
     */
    Population( const Population &other );

    unsigned chromosome_size() const;  // Tamanho do cromossomo
    unsigned size() const;             // Tamanho da população

    double                      get_best_fitness() const;
    double                      get_fitness( unsigned i ) const;
    const std::vector<uint8_t> &get_chromosome( unsigned i ) const;

    // Métodos para acesso direto (privado ou friend de GeneticAlgorithm)
    void                  set_fitness( unsigned i, double f );
    void                  sort_fitness();
    std::vector<uint8_t> &get_chromosome_at( unsigned i );

  private:
    std::vector<Chromosome>                 population;
    std::vector<std::pair<Fitness, size_t>> fitness;
  };

}  // namespace r3dp::ga
