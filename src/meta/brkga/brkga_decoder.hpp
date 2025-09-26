#pragma once
#include "../../core/graph.hpp"

#include <boost/graph/detail/adjacency_list.hpp>
#include <vector>

namespace r3dp::brkga {
  class R3DPDecoder {
  private:
    const core::graph_t &graph;

  public:
    explicit R3DPDecoder( const core::graph_t &g ) : graph( g ) {}

    [[nodiscard]] double decode( const std::vector<double> &chromosome ) const {
      const auto           size = boost::num_vertices( graph );
      std::vector<uint8_t> solution( size );

      for ( double gene : chromosome ) {
        uint8_t mapped = static_cast<uint8_t>( std::min( static_cast<int>( gene * 4.0 ), 3 ) );
        solution.push_back( mapped );
      }
      bool has_violations = true;
      while ( has_violations ) {
        has_violations = false;

        auto vertices = boost::vertices( graph );
        for ( auto v_it = vertices.first; v_it != vertices.second; ++v_it ) {
          core::vertex_t u = *v_it;

          if ( solution[u] == 0 ) {
            int  neighbor_sum = 0;
            auto neighbors    = boost::adjacent_vertices( u, graph );
            for ( auto n_it = neighbors.first; n_it != neighbors.second; ++n_it ) {
              neighbor_sum += solution[*n_it];
            }

            if ( neighbor_sum < 3 ) {
              solution[u]    = 1;
              has_violations = true;
            }
          }

          if ( solution[u] == 1 ) {
            int  neighbor_sum = 0;
            auto neighbors    = boost::adjacent_vertices( u, graph );
            for ( auto n_it = neighbors.first; n_it != neighbors.second; ++n_it ) {
              neighbor_sum += solution[*n_it];
            }

            if ( neighbor_sum < 2 ) {
              solution[u]    = 2;
              has_violations = true;
            }
          }
        }
      }

      double sum_of_labels = 0.0;
      for ( uint8_t label : solution ) {
        sum_of_labels += static_cast<double>( label );
      }

      return sum_of_labels;
    }
  };

}  // namespace r3dp::brkga
