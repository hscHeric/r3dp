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
      std::vector<double> k( dim );
      for ( size_t i = 0; i < dim; ++i ) {
        unsigned lab = std::min<unsigned>( geno[i], 3 );
        k[i] = ( static_cast<double>( lab ) + 0.5 ) / 4.0;  // 0:0.125, 1:0.375, 2:0.625, 3:0.875
      }
      return k;
    }

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
      const auto   labels = decode( keys );
      const size_t viol   = violation_count( labels );
      const size_t s      = label_sum( labels );

      return static_cast<double>( s ) + ( static_cast<double>( viol ) * penalty );
    }

    [[nodiscard]] static size_t label_sum( const genotype_type &labels ) {
      size_t s = 0;
      for ( auto v : labels ) {
        s += static_cast<size_t>( v );
      }
      return s;
    }

    [[nodiscard]] size_t violation_count( const genotype_type &labels ) const {
      size_t viol = 0;

      using boost::adjacent_vertices;
      using boost::vertices;

      auto [it, end] = vertices( *graph );
      for ( ; it != end; ++it ) {
        const auto v = static_cast<size_t>( *it );
        if ( labels[v] != 0 ) {
          continue;
        }

        bool has_neighbor_ge3 = false;
        auto [nb, nb_end]     = adjacent_vertices( *it, *graph );
        for ( ; nb != nb_end; ++nb ) {
          const auto u = static_cast<size_t>( *nb );
          if ( labels[u] >= 3 ) {
            has_neighbor_ge3 = true;
            break;
          }
        }
        if ( !has_neighbor_ge3 ) {
          ++viol;
        }
      }
      return viol;
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
