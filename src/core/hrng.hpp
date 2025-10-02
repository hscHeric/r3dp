#pragma once
#include "rng_concepts.hpp"

#include <cstdint>
#include <limits>
#include <random>
#include <span>
#include <type_traits>

namespace r3dp::core {

  /**
   * @brief RNG padrão baseado em std::mt19937_64.
   */
  class hrng {
  public:
    /// @brief Constrói com semente aleatória do sistema.
    hrng() : hrng( seed_from_device_() ) {}

    /// @brief Constrói com semente explícita.
    /// @param seed valor da semente.
    explicit hrng( std::uint64_t seed )
      : engine( static_cast<std::mt19937_64::result_type>( seed ) ) {}

    /// @brief Re-semeia o gerador.
    /// @param seed nova semente.
    void reseed( std::uint64_t seed ) {
      engine.seed( static_cast<std::mt19937_64::result_type>( seed ) );
    }

    /**
     * @brief Gera um valor aleatório.
     * @return valor pseudoaleatório do tipo T.
     */
    template <rand_value T>
    T random() {
      if constexpr ( std::is_same_v<T, bool> ) {
        return random_bool();
      } else if constexpr ( rand_int<T> ) {
        std::uniform_int_distribution<T> dist( std::numeric_limits<T>::min(),
                                               std::numeric_limits<T>::max() );
        return dist( engine );
      } else {  // float/double
        std::uniform_real_distribution<T> dist( static_cast<T>( 0 ), static_cast<T>( 1 ) );
        return dist( engine );
      }
    }

    /**
     * @brief Gera true com probabilidade p.
     * @param p probabilidade entre 0 e 1.
     * @return true com probabilidade p.
     */
    bool random_bool( double p = 0.5 ) {
      if ( p <= 0.0 ) {
        return false;
      }
      if ( p >= 1.0 ) {
        return true;
      }
      std::bernoulli_distribution dist( p );
      return dist( engine );
    }

    /**
     * @brief Gera true com probabilidade numer/denom.
     * @param numer numerador.
     * @param denom denominador (>0).
     * @return true com probabilidade numer/denom.
     */
    bool random_ratio( std::uint64_t numer, std::uint64_t denom ) {
      if ( denom == 0 ) {
        return false;
      }
      if ( numer >= denom ) {
        return true;
      }
      std::uniform_int_distribution<std::uint64_t> dist( 0, denom - 1 );
      return dist( engine ) < numer;
    }

    /**
     * @brief Inteiro uniforme no intervalo [min, max).
     * @param min limite inferior (inclusivo).
     * @param max limite superior (exclusivo).
     * @return inteiro aleatório em [min, max).
     */
    template <rand_int T>
    T random_range( T min, T max ) {
      if ( min >= max ) {
        return min;
      }

      std::uniform_int_distribution<T> dist( min, static_cast<T>( max - 1 ) );
      return dist( engine );
    }

    /**
     * @brief Float uniforme no intervalo [min, max).
     * @param min limite inferior (inclusivo).
     * @param max limite superior (exclusivo).
     * @return valor em ponto flutuante em [min, max).
     */
    template <rand_float T>
    T random_range( T min, T max ) {
      if ( min >= max ) {
        return min;
      }
      std::uniform_real_distribution<T> dist( min, max );
      return dist( engine );
    }

    /**
     * @brief Preenche um buffer com bytes aleatórios.
     * @param out buffer de saída.
     */
    void fill( std::span<std::uint8_t> out ) {
      std::size_t i = 0, n = out.size();
      while ( i + 8 <= n ) {
        std::uint64_t v = next_u64_();
        out[i + 0]      = static_cast<std::uint8_t>( v & 0xFFU );
        out[i + 1]      = static_cast<std::uint8_t>( ( v >> 8 ) & 0xFFU );
        out[i + 2]      = static_cast<std::uint8_t>( ( v >> 16 ) & 0xFFU );
        out[i + 3]      = static_cast<std::uint8_t>( ( v >> 24 ) & 0xFFU );
        out[i + 4]      = static_cast<std::uint8_t>( ( v >> 32 ) & 0xFFU );
        out[i + 5]      = static_cast<std::uint8_t>( ( v >> 40 ) & 0xFFU );
        out[i + 6]      = static_cast<std::uint8_t>( ( v >> 48 ) & 0xFFU );
        out[i + 7]      = static_cast<std::uint8_t>( ( v >> 56 ) & 0xFFU );
        i += 8;
      }
      if ( i < n ) {
        std::uint64_t v = next_u64_();
        for ( std::size_t k = 0; i < n; ++i, ++k ) {
          out[i] = static_cast<std::uint8_t>( ( v >> ( 8 * k ) ) & 0xFFU );
        }
      }
    }

  private:
    std::mt19937_64 engine;

    static std::uint64_t seed_from_device_() {
      std::random_device rd;
      return ( static_cast<std::uint64_t>( rd() ) << 32 ) ^ static_cast<std::uint64_t>( rd() );
    }

    std::uint64_t next_u64_() {
      return static_cast<std::uint64_t>( engine() );
    }
  };

  static_assert( rng<hrng>, "hrng não satisfaz r3dp::core::rng" );

}  // namespace r3dp::core
