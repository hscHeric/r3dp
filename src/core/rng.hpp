#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace r3dp::core {

  template <typename Engine = std::mt19937_64>
  class RNG final {
    static_assert( std::uniform_random_bit_generator<Engine>,
                   "Engine deve satisfazer std::uniform_random_bit_generator" );

  public:
    using engine_type = Engine;

    static constexpr double DEFAULT_P_TRUE_BOOL = 0.5;

    explicit RNG( uint64_t seed ) noexcept : engine_{ seed } {}

    explicit RNG( std::seed_seq seq ) noexcept : engine_{ seq } {}

    [[nodiscard]] static RNG from_entropy() {
      std::random_device rd;
      std::seed_seq      seq{ rd(), rd(), rd(), rd() };
      return RNG{ seq };
    }

    RNG( const RNG & )                = default;
    RNG &operator=( const RNG & )     = default;
    RNG( RNG && ) noexcept            = default;
    RNG &operator=( RNG && ) noexcept = default;
    ~RNG()                            = default;

    void reseed( uint64_t seed ) noexcept {
      engine_.seed( seed );
    }

    void reseed( std::seed_seq seq ) noexcept {
      engine_.seed( seq );
    }

    void discard( uint64_t n ) noexcept {
      if constexpr ( requires( engine_type &e, uint64_t k ) { e.discard( k ); } ) {
        engine_.discard( n );
      } else {
        for ( uint64_t i = 0; i < n; ++i ) {
          (void)engine_();
        }
      }
    }

    [[nodiscard]] engine_type &engine() noexcept {
      return engine_;
    }

    [[nodiscard]] const engine_type &engine() const noexcept {
      return engine_;
    }

    [[nodiscard]] typename engine_type::result_type operator()() {
      return engine_();
    }

    template <std::integral T>
    [[nodiscard]] T random() {
      constexpr T lo = std::numeric_limits<T>::lowest() + ( std::is_signed_v<T> ? 1 : 0 );
      constexpr T hi = std::numeric_limits<T>::max();
      return std::uniform_int_distribution<T>{ lo, hi }( engine_ );
    }

    template <std::floating_point T = double>
    [[nodiscard]] T random() {
      return uniform01<T>();
    }

    template <std::integral T>
    [[nodiscard]] T random_range( T min, T max ) {
      if ( min > max ) {
        throw std::invalid_argument{ "random_range<T>: min > max" };
      }
      return std::uniform_int_distribution<T>{ min, max }( engine_ );
    }

    template <std::floating_point T>
    [[nodiscard]] T random_range( T min, T max ) {
      if ( min >= max ) {
        throw std::invalid_argument{ "random_range<T>: min >= max" };
      }
      return std::uniform_real_distribution<T>{ min, max }( engine_ );
    }

    [[nodiscard]] bool random_bool( double p_true = DEFAULT_P_TRUE_BOOL ) {
      if ( p_true < 0.0 || p_true > 1.0 ) {
        throw std::invalid_argument{ "random_bool: p_true fora de [0,1]" };
      }
      return std::bernoulli_distribution{ p_true }( engine_ );
    }

    template <std::floating_point T = double>
    [[nodiscard]] T uniform01() {
      return std::uniform_real_distribution<T>{ T( 0 ), T( 1 ) }( engine_ );
    }

    [[nodiscard]] double normal( double mean = 0.0, double stddev = 1.0 ) {
      if ( stddev < 0.0 ) {
        throw std::invalid_argument{ "normal: stddev < 0" };
      }
      return std::normal_distribution<double>{ mean, stddev }( engine_ );
    }

    [[nodiscard]] double exponential( double lambda = 1.0 ) {
      if ( lambda <= 0.0 ) {
        throw std::invalid_argument{ "exponential: lambda <= 0" };
      }
      return std::exponential_distribution<double>{ lambda }( engine_ );
    }

    [[nodiscard]] double cauchy( double x0 = 0.0, double gamma = 1.0 ) {
      if ( gamma <= 0.0 ) {
        throw std::invalid_argument{ "cauchy: gamma <= 0" };
      }
      return std::cauchy_distribution<double>{ x0, gamma }( engine_ );
    }

    template <class It>
    void shuffle( It first, It last ) {
      std::shuffle( first, last, engine_ );
    }

    [[nodiscard]] std::vector<std::size_t> permutation( std::size_t n ) {
      std::vector<std::size_t> idx( n );
      std::iota( idx.begin(), idx.end(), static_cast<std::size_t>( 0 ) );
      shuffle( idx.begin(), idx.end() );
      return idx;
    }

    [[nodiscard]] std::size_t random_index( std::size_t n ) {
      if ( n == 0 ) {
        throw std::invalid_argument{ "random_index: n == 0" };
      }
      return std::uniform_int_distribution<std::size_t>{ 0, n - 1 }( engine_ );
    }

  private:
    engine_type engine_{};
  };

}  // namespace r3dp::core
