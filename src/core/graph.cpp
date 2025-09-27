#include "graph.hpp"

#include <fstream>
#include <iostream>

namespace r3dp::core {
  std::pair<vertex_t, std::set<edge_t>> read_graph_from_file( const std::string &file_path ) {
    std::ifstream file( file_path );
    if ( !file.is_open() ) {
      std::cerr << "Erro ao abrir o arquivo: " << file_path << std::endl;
      return { 0, {} };
    }

    std::set<vertex_t>  uniqueVertices;
    std::vector<edge_t> originaledge_ts;
    vertex_t            u = 0, v = 0;

    // Lê todas as arestas e todos os vértices
    while ( file >> u >> v ) {
      uniqueVertices.insert( u );
      uniqueVertices.insert( v );
      originaledge_ts.emplace_back( u, v );
    }

    std::unordered_map<vertex_t, vertex_t> remapping;
    vertex_t                               counter = 0;
    for ( const auto &vertex : uniqueVertices ) {
      remapping[vertex] = counter++;
    }

    // Criar as arestas remapeadas e remover duplicatas
    std::set<edge_t> uniqueRemappededge_ts;
    for ( const auto &edge : originaledge_ts ) {
      vertex_t remapped_u = remapping.at( edge.first );
      vertex_t remapped_v = remapping.at( edge.second );

      if ( remapped_u != remapped_v ) {
        if ( remapped_u > remapped_v ) {
          std::swap( remapped_u, remapped_v );
        }
        uniqueRemappededge_ts.insert( { remapped_u, remapped_v } );
      }
    }

    return { counter, uniqueRemappededge_ts };
  }

  graph_t build_graph_from( vertex_t n, const std::set<edge_t> &edges ) {
    graph_t g( static_cast<std::size_t>( n ) );  // Aqui está sendo criado um grafo de 0..n-1
    for ( auto [u, v] : edges ) {
      if ( u == v ) {
        continue;
      }
      if ( u >= n || v >= n ) {
        throw std::out_of_range( "Existe uma aresta fora dos limites" );
      }
      if ( v < u ) {
        std::swap( u, v );  // normaliza ordem
      }
      boost::add_edge( u, v, g );
    }

    return g;
  }

  bool is_valid_fdr3( const graph_t &g, const std::vector<uint8_t> &labels ) {
    using VD = boost::graph_traits<graph_t>::vertex_descriptor;

    const auto n = boost::num_vertices( g );
    if ( labels.size() != n ) {
      throw std::invalid_argument( "labels.size() != num_vertices(graph)" );
    }

    auto closed_sum = [&]( VD v ) -> uint32_t {
      // v é um índice quando usamos vecS; convertemos para size_t
      uint32_t s    = labels[static_cast<std::size_t>( v )];
      auto [nb, ne] = boost::adjacent_vertices( v, g );
      for ( auto it = nb; it != ne; ++it ) {
        s += labels[static_cast<std::size_t>( *it )];
      }
      return s;
    };

    auto [vb, ve] = boost::vertices( g );
    for ( auto it = vb; it != ve; ++it ) {
      VD            v  = *it;
      const uint8_t fv = labels[static_cast<std::size_t>( v )];
      if ( fv == 0 || fv == 1 ) {
        if ( closed_sum( v ) < 3 ) {
          return false;
        }
      }
    }
    return true;
  }

  std::vector<std::size_t> violating_vertices_fdr3( const graph_t              &g,
                                                    const std::vector<uint8_t> &labels ) {
    using VD = boost::graph_traits<graph_t>::vertex_descriptor;

    const auto n = boost::num_vertices( g );
    if ( labels.size() != n ) {
      throw std::invalid_argument( "labels.size() != num_vertices(graph)" );
    }

    std::vector<std::size_t> bad;

    auto closed_sum = [&]( VD v ) -> uint32_t {
      uint32_t s    = labels[static_cast<std::size_t>( v )];
      auto [nb, ne] = boost::adjacent_vertices( v, g );
      for ( auto it = nb; it != ne; ++it ) {
        s += labels[static_cast<std::size_t>( *it )];
      }
      return s;
    };

    auto [vb, ve] = boost::vertices( g );
    for ( auto it = vb; it != ve; ++it ) {
      VD            v   = *it;
      const auto    idx = static_cast<std::size_t>( v );
      const uint8_t fv  = labels[idx];
      if ( fv == 0 || fv == 1 ) {
        if ( closed_sum( v ) < 3 ) {
          bad.push_back( idx );
        }
      }
    }
    return bad;
  }

  size_t max_degree( const graph_t &g ) {
    using boost::vertices;

    size_t maxdeg = 0;

    auto [vi, vi_end] = vertices( g );
    for ( ; vi != vi_end; ++vi ) {
      // Como não há laços, degree(*vi, g) é exatamente o nº de vizinhos de *vi.
      maxdeg = std::max( maxdeg, degree( *vi, g ) );
    }
    return maxdeg;
  }

}  // namespace r3dp::core
