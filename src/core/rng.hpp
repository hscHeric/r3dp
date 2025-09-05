#pragma once

#include <random>
#include <stdexcept>

namespace r3dp::core {

  /**
   * @brief Classe responsável pela geração de números aleatórios.
   */
  class RNG {
  private:
    RNG()                         = delete;  ///< Impede a criação de instâncias de RNG.
    ~RNG()                        = delete;  ///< Impede a destruição de instâncias de RNG.
    RNG( const RNG & )            = delete;  ///< Impede a cópia de instâncias de RNG.
    void operator=( const RNG & ) = delete;  ///< Impede a atribuição de instâncias de RNG.

    /**
     * @brief Gera um motor de números aleatórios de forma thread-safe.
     *
     * Este método retorna uma referência para o gerador de números aleatórios,
     * garantindo que ele seja gerado uma única vez por thread.
     *
     * @return std::mt19937& O gerador de números aleatórios.
     */
    static std::mt19937 &engine() {
      static thread_local std::mt19937 engine{ std::random_device{}() };
      return engine;
    }

  public:
    /**
     * @brief Gera um número aleatório inteiro.
     *
     * Esta função retorna um número inteiro aleatório.
     *
     * @tparam T Tipo de dado (deve ser um tipo inteiro).
     * @return T Número aleatório gerado.
     */
    template <typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
    static T random() {
      return std::uniform_int_distribution<T>()( engine() );
    }

    /**
     * @brief Gera um número aleatório de ponto flutuante.
     *
     * Esta função retorna um número de ponto flutuante aleatório.
     *
     * @tparam T Tipo de dado (deve ser um tipo de ponto flutuante).
     * @return T Número aleatório gerado.
     */
    template <typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
    static T random() {
      return std::uniform_real_distribution<T>()( engine() );
    }

    /**
     * @brief Gera um número aleatório inteiro dentro de um intervalo.
     *
     * Esta função retorna um número aleatório inteiro dentro do intervalo especificado
     * pelo usuário.
     *
     * @tparam T Tipo de dado (deve ser um tipo inteiro).
     * @param min Valor mínimo (inclusive) do intervalo.
     * @param max Valor máximo (inclusive) do intervalo.
     * @return T Número aleatório gerado no intervalo [min, max].
     * @throws std::invalid_argument Se o valor de `min` for maior que o de `max`.
     */
    template <typename T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
    static T random_range( T min, T max ) {
      if ( min > max )
        throw std::invalid_argument( "O valor mínimo não pode ser maior que o valor máximo." );
      return std::uniform_int_distribution<T>( min, max )( engine() );
    }

    /**
     * @brief Gera um número aleatório de ponto flutuante dentro de um intervalo.
     *
     * Esta função retorna um número de ponto flutuante aleatório dentro do intervalo
     * especificado pelo usuário.
     *
     * @tparam T Tipo de dado (deve ser um tipo de ponto flutuante).
     * @param min Valor mínimo (inclusive) do intervalo.
     * @param max Valor máximo (inclusive) do intervalo.
     * @return T Número aleatório gerado no intervalo [min, max].
     * @throws std::invalid_argument Se o valor de `min` for maior que o de `max`.
     */
    template <typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
    static T random_range( T min, T max ) {
      if ( min > max )
        throw std::invalid_argument( "O valor mínimo não pode ser maior que o valor máximo." );
      return std::uniform_real_distribution<T>( min, max )( engine() );
    }

    /**
     * @brief Gera um valor booleano aleatório.
     *
     * Esta função retorna um valor booleano aleatório (`true` ou `false`).
     *
     * @return bool Valor booleano aleatório gerado.
     */
    static bool random_bool() {
      return random_range<int>( 0, 1 ) == 1;
    }
  };

}  // namespace r3dp::core
