#pragma once

#include <omp.h>
#include <vector>

namespace r3dp::hho {

  /**
   * @class HHO
   * @brief Implementa o algoritmo de otimização Harris Hawk Optimization (HHO).
   *
   * Esta classe é um template para otimizar problemas de busca usando o algoritmo
   * HHO. Ele opera com dois tipos de templates: uma classe para decodificar a
   * solução em um valor de aptidão (`Decoder`) e uma classe para a geração de
   * números aleatórios (`RNG`).
   *
   * A classe HHO suporta dois métodos para definir os limites de busca: um único
   * par de limites para todas as dimensões, ou um vetor de limites, um para cada
   * dimensão.
   *
   * @tparam Decoder A classe que deve conter o método `double decode(const std::vector<double>&
   * solution)`.
   *
   * @tparam RNG
   */
  template <class Decoder, class RNG>
  class HHO {
  public:
    /**
     * @brief Construtor para otimização com um único par de limites.
     *
     * Este construtor é ideal para problemas onde todos os parâmetros de busca
     * compartilham o mesmo intervalo de valores.
     *
     * @param population_size O tamanho da população de falcões (hawks).
     * @param dimension O número de variáveis do problema (dimensão da solução - aqui será o número
     * de vartices para o r3dp).
     * @param lower_bound O limite inferior único para todas as dimensões.
     * @param upper_bound O limite superior único para todas as dimensões.
     * @param max_iterations O número máximo de iterações.
     * @param max_threads O número máximo de threads a serem usadas com OpenMP.
     * @param ref_decoder Uma referência para o decodificador de aptidão (fitness).
     * @param ref_rng Uma referência para o gerador de números aleatórios.
     */
    HHO( unsigned       population_size,
         unsigned       dimension,
         double         lower_bound,
         double         upper_bound,
         unsigned       max_iterations,
         unsigned       max_threads,
         const Decoder &ref_decoder,
         RNG           &ref_rng );

    /**
     * @brief Construtor para otimização com vetores de limites.
     *
     * Este construtor permite definir limites de busca diferentes para cada
     * variável do problema, oferecendo maior flexibilidade.
     *
     * @param population_size O tamanho da população de falcões (hawks).
     * @param dimension O número de variáveis do problema (dimensão da solução).
     * @param lower_bounds Um vetor de limites inferiores, um para cada dimensão.
     * @param upper_bounds Um vetor de limites superiores, um para cada dimensão.
     * @param max_iterations O número máximo de iterações.
     * @param max_threads O número máximo de threads a serem usadas com OpenMP.
     * @param ref_decoder Uma referência para o decodificador de aptidão (fitness).
     * @param ref_rng Uma referência para o gerador de números aleatórios.
     */
    HHO( unsigned                   population_size,
         unsigned                   dimension,
         const std::vector<double> &lower_bounds,
         const std::vector<double> &upper_bounds,
         unsigned                   max_iterations,
         unsigned                   max_threads,
         const Decoder             &ref_decoder,
         RNG                       &ref_rng );

    // Métodos públicos para interação com o algoritmo

    /**
     * @brief Executa um passo de iteração do algoritmo HHO.
     *
     * Este método realiza uma única iteração do processo de otimização,
     * atualizando as posições dos falcões e a melhor solução global.
     */
    void step();

    /**
     * @brief Retorna a melhor solução encontrada até o momento.
     * @return Um vetor de `double` representando a melhor posição (solução).
     */
    [[nodiscard]] std::vector<double> get_best_solution() const;

    /**
     * @brief Retorna o valor de aptidão (fitness) da melhor solução.
     * @return O valor de aptidão da melhor solução.
     */
    [[nodiscard]] double get_best_fitness() const;

    /**
     * @brief Retorna o número da iteração atual.
     * @return O número da iteração.
     */
    [[nodiscard]] unsigned get_iteration() const;

    /**
     * @brief Reinicia o estado do algoritmo para uma nova execução.
     *
     * Este método redefine o estado interno do algoritmo, incluindo a população
     * de falcões, a melhor solução encontrada e o contador de iterações.
     */
    void reset();

  private:
    const unsigned      dimension;
    const unsigned      population_size;
    std::vector<double> lower_bounds_vec;
    std::vector<double> upper_bounds_vec;
    const unsigned      max_iterations;  // Novo membro de classe
    const unsigned      max_threads;

    // Estado do algoritmo
    double                           rabbit_energy;
    std::vector<std::vector<double>> hawks;
    std::vector<double>              best_position;
    double                           best_fitness;
    unsigned                         iteration;

    // Templates:
    RNG           &ref_rng;
    const Decoder &ref_decoder;

    // Métodos privados
    std::vector<double> levy_flight( int d );
    void                initialize_hawks();
  };

}  // namespace r3dp::hho
