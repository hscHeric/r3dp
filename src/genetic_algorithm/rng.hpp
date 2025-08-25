#pragma once

#include <random>

namespace r3dp::ga {

  /**
   * @brief Interface para um gerador de números aleatórios.
   *
   * Requer métodos para gerar números aleatórios de diferentes tipos
   * para uso no algoritmo genético.
   */
  class RNG {
  public:
    virtual ~RNG() = default;

    /**
     * @brief Gera um número de ponto flutuante no intervalo [0, 1).
     * @return Um número de ponto flutuante aleatório.
     */
    virtual double rand() = 0;

    /**
     * @brief Gera um número inteiro aleatório no intervalo [0, n].
     * @param n O limite superior inclusivo.
     * @return Um número inteiro aleatório.
     */
    virtual unsigned int randInt( unsigned int n ) = 0;
  };

}  // namespace r3dp::ga
