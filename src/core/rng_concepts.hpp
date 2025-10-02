#pragma once

#include <concepts>
#include <cstdint>
#include <span>
#include <type_traits>

namespace r3dp::core {

  /**
   * Esse conjunto de concepts garante que as funções de geração de números aleatórios aceitem
   * apenas tipos booleanos, inteiros de 8, 16, 32 ou 64 bits (signed ou unsigned), e tipos de ponto
   * flutuante float ou double. O concept final RandValue combina todos os requisitos
   */
  template <class T>
  concept rand_bool = std::same_as<std::remove_cv_t<T>, bool>;

  template <class T>
  concept rand_int =
    std::is_integral_v<std::remove_cv_t<T>> && !std::same_as<std::remove_cv_t<T>, bool> &&
    ( sizeof( std::remove_cv_t<T> ) == 1 || sizeof( std::remove_cv_t<T> ) == 2 ||
      sizeof( std::remove_cv_t<T> ) == 4 || sizeof( std::remove_cv_t<T> ) == 8 );

  template <class T>
  concept rand_float =
    ( std::same_as<std::remove_cv_t<T>, float> || std::same_as<std::remove_cv_t<T>, double> );

  template <class T>
  concept rand_value =
    rand_bool<T> || rand_int<T> ||
    rand_float<T>;  // Ou o valor gerado é de ponto flutuante, ou é um valor inteiro ou é um boleano

  template <class R>
  concept rng = requires( R r, std::span<uint8_t> bytes, uint64_t numer, uint64_t denom ) {
                  // random<T>() para todos os tipos suportados
                  { r.template random<bool>() } -> std::same_as<bool>;
                  { r.template random<int8_t>() } -> std::same_as<int8_t>;
                  { r.template random<uint8_t>() } -> std::same_as<uint8_t>;
                  { r.template random<int16_t>() } -> std::same_as<int16_t>;
                  { r.template random<uint16_t>() } -> std::same_as<uint16_t>;
                  { r.template random<int32_t>() } -> std::same_as<int32_t>;
                  { r.template random<uint32_t>() } -> std::same_as<uint32_t>;
                  { r.template random<int64_t>() } -> std::same_as<int64_t>;
                  { r.template random<uint64_t>() } -> std::same_as<uint64_t>;
                  { r.template random<float>() } -> std::same_as<float>;
                  { r.template random<double>() } -> std::same_as<double>;

                  // random_bool (padrão e com p)
                  { r.random_bool() } -> std::same_as<bool>;
                  { r.random_bool( 0.5 ) } -> std::same_as<bool>;

                  // random_ratio(numer/denom)
                  { r.random_ratio( numer, denom ) } -> std::same_as<bool>;

                  // random_range inteiros [min, max)
                  {
                    r.template random_range<int8_t>( int8_t{ 0 }, int8_t{ 1 } )
                  } -> std::same_as<int8_t>;
                  {
                    r.template random_range<uint8_t>( uint8_t{ 0 }, uint8_t{ 1 } )
                  } -> std::same_as<uint8_t>;
                  {
                    r.template random_range<int16_t>( int16_t{ 0 }, int16_t{ 1 } )
                  } -> std::same_as<int16_t>;
                  {
                    r.template random_range<uint16_t>( uint16_t{ 0 }, uint16_t{ 1 } )
                  } -> std::same_as<uint16_t>;
                  {
                    r.template random_range<int32_t>( int32_t{ 0 }, int32_t{ 1 } )
                  } -> std::same_as<int32_t>;
                  {
                    r.template random_range<uint32_t>( uint32_t{ 0 }, uint32_t{ 1 } )
                  } -> std::same_as<uint32_t>;
                  {
                    r.template random_range<int64_t>( int64_t{ 0 }, int64_t{ 1 } )
                  } -> std::same_as<int64_t>;
                  {
                    r.template random_range<uint64_t>( uint64_t{ 0 }, uint64_t{ 1 } )
                  } -> std::same_as<uint64_t>;

                  // random_range floats [min, max)
                  { r.template random_range<float>( 0.0F, 1.0F ) } -> std::same_as<float>;
                  { r.template random_range<double>( 0.0, 1.0 ) } -> std::same_as<double>;

                  // fill (bytes aleatórios)
                  { r.fill( bytes ) } -> std::same_as<void>;
                };

}  // namespace r3dp::core
