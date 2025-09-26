#pragma once

#include "graph.hpp"
#include "problem.hpp"

#include <algorithm>
#include <boost/graph/detail/adjacency_list.hpp>
#include <cstdint>
#include <nlohmann/detail/input/parser.hpp>
#include <numeric>
#include <vector>

/*
 * O problema que eu estou observando aqui é que o evaluate percorre muitas vezes o grafo, por causa
 * das funções chamadas, acredito que se escrever uma função especificamente para o evaluate consiga
 * um resultado melhor, por enquanto vou deixar isso, mas vou fazer uma função melhor.
 *
 * Lembrar de avaliar qual modelo escolher, se definir uma penalidade é melhor do que fazer evaluate
 * apenas de funções válidas
 * */
namespace r3dp::core {
  struct r3dp_problem {
    using genotype_type = std::vector<uint8_t>;  // Labels de 0 a 3

    explicit r3dp_problem( const graph_t &g, double penalty_m = 1e6 )
      : graph( g )
      , dim( static_cast<size_t>( boost::num_vertices( g ) ) )
      , penalty( penalty_m )
      , lo( dim, 0.0 )
      , hi( dim, 1.0 ) {}

    [[nodiscard]] size_t dimension() const {
      return dim;
    }

    [[nodiscard]] const std::vector<double> &lower_bounds() const {
      return lo;
    }

    [[nodiscard]] const std::vector<double> &upper_bounds() const {
      return hi;
    }

    [[nodiscard]] std::vector<double> encode( const genotype_type &geno ) const {
      // TODO: Converte em uma função dentro do espeço que o algoritmo trabalha
      std::vector<double> keys;
      keys.reserve( dim );

      for ( auto v : geno ) {
        if ( v < 0 || v > 3 ) {
          throw std::invalid_argument( "encode_solution: valor fora do intervalo [0,3]" );
        }
        keys.push_back( static_cast<double>( v ) / 3.0 );
      }

      return keys;
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
      auto f    = decode( keys );
      auto soma = std::accumulate( f.begin(), f.end(), 0 );

      return soma + ( static_cast<double>( violation_count( f ) ) * penalty );
    }

    [[nodiscard]] size_t violation_count( const genotype_type &labels ) const {
      // TODO: Retorna o número de vértices que não seguem a restrição
      assert( labels.size() == boost::num_vertices( graph ) &&
              "violation_count: labels.size() != num_vertices(graph)" );

      size_t violation_counter = 0;
      auto [vi, vi_end]        = boost::vertices( graph );
      for ( ; vi != vi_end; ++vi ) {
        // vi -> index do vértice, tenho que percorrer todos os vértices verificando as restrições

        const auto u = static_cast<size_t>(
          *vi );  // Estou usando o vertex_index como um uint32_t por isso posso fazer esse cast

        const auto u_label = labels[u];
        if ( u_label <= 1 ) {
          size_t sum        = u_label;
          auto [ai, ai_end] = boost::adjacent_vertices( *vi, graph );
          for ( ; ai != ai_end; ++ai ) {
            sum += labels[static_cast<std::size_t>( *ai )];
            if ( sum >= 3 ) {
              break;  // já satisfaz a restrição
            }
          }

          if ( sum < 3 ) {
            ++violation_counter;
          }
        }
      }

      return violation_counter;
    }

  private:
    const graph_t       graph;
    size_t              dim;
    double              penalty;
    std::vector<double> lo;
    std::vector<double> hi;
  };

  static_assert( Problem<r3dp_problem>,
                 "A classe r3dp_problem deve atender ao conceito de Problem." );
}  // namespace r3dp::core
