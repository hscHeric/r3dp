#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace r3dp::core {
  template <typename P>
  concept Problem =
    requires( const P &cp, typename P::genotype_type g, std::span<const double> keys ) {
      // Assegura que o tipo de genótipo está definido
      typename P::genotype_type;

      // tamanho da instancia do problema
      { cp.dimension() } -> std::convertible_to<size_t>;

      // Limites devem ser vetores de double
      { cp.lower_bounds() } -> std::same_as<std::vector<double>>;
      { cp.upper_bounds() } -> std::same_as<std::vector<double>>;

      // Funções de encode/decode
      { cp.encode( g ) } -> std::same_as<std::vector<double>>;
      { cp.decode( keys ) } -> std::same_as<typename P::genotype_type>;

      // Função de avaliação
      { cp.evaluate( keys ) } -> std::convertible_to<double>;
    };
}  // namespace r3dp::core
