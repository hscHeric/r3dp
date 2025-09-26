#pragma once

#include "graph.hpp"
#include "problem.hpp"

#include <algorithm>
#include <boost/graph/detail/adjacency_list.hpp>
#include <cstdint>
#include <vector>

namespace r3dp::core {
  struct r3dp_problem {
    using genotype_type = std::vector<uint8_t>;  // Labels de 0 a 3

    explicit r3dp_problem( const graph_t &g, double penalty_m = 1e6 )
      : graph( &g )
      , dim( static_cast<size_t>( boost::num_vertices( g ) ) )
      , penalty( penalty_m )
      , lo( dim, 0.0 )
      , hi( dim, 1.0 ) {}

    [[nodiscard]] size_t dimension() const {
      return dim;
    }

    [[nodiscard]] std::vector<double> lower_bounds() const {
      return lo;
    }

    [[nodiscard]] std::vector<double> upper_bounds() const {
      return hi;
    }

    [[nodiscard]] std::vector<double> encode( const genotype_type &geno ) const {
      // TODO: Converte em uma função dentro do espeço que o algoritmo trabalha
    }

    // Converte um conjunto de chaves vector<double> em uma função de dominação 3 romana
    [[nodiscard]] genotype_type decode( std::span<const double> keys ) const {
      genotype_type output;
      output.reserve( dim );
      for ( auto x : keys ) {
        x        = std::clamp( x, 0.0, 1.0 );
        auto val = static_cast<uint8_t>( x * 4.0 );
        val      = std::min<uint8_t>( val, 3 );
        output.push_back( val );
      }

      return output;
    }

    [[nodiscard]] double evaluate( std::span<const double> keys ) const {
      // TODO: Retorna o fitness de uma possivel solução
    }

    [[nodiscard]] size_t violation_count( const genotype_type &labels ) const {
      // TODO: Retorna o número de vértices que não seguem a restrição
    }

  private:
    const graph_t      *graph;
    size_t              dim;
    double              penalty;
    std::vector<double> lo;
    std::vector<double> hi;
  };

  static_assert( Problem<r3dp_problem>,
                 "A classe r3dp_problem deve atender ao conceito de Problem." );
}  // namespace r3dp::core
