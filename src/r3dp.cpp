#include "r3dp.hpp"

#include <boost/graph/detail/adjacency_list.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>

namespace r3dp {
  std::pair<r3dp::Vertex, std::set<r3dp::Edge>>
    read_graph_from_file( const std::string &filePath ) {
    std::ifstream file( filePath );
    if ( !file.is_open() ) {
      std::cerr << "Erro ao abrir o arquivo: " << filePath << std::endl;
      return { 0, {} };
    }

    std::set<r3dp::Vertex>  uniqueVertices;
    std::vector<r3dp::Edge> originalEdges;
    r3dp::Vertex            u, v;

    // Lê todas as arestas e todos os vértices
    while ( file >> u >> v ) {
      uniqueVertices.insert( u );
      uniqueVertices.insert( v );
      originalEdges.push_back( { u, v } );
    }

    std::unordered_map<r3dp::Vertex, r3dp::Vertex> remapping;
    r3dp::Vertex                                   counter = 0;
    for ( const auto &vertex : uniqueVertices ) {
      remapping[vertex] = counter++;
    }

    // Criar as arestas remapeadas e remover duplicatas
    std::set<r3dp::Edge> uniqueRemappedEdges;
    for ( const auto &edge : originalEdges ) {
      r3dp::Vertex remapped_u = remapping.at( edge.first );
      r3dp::Vertex remapped_v = remapping.at( edge.second );

      if ( remapped_u == remapped_v ) {
        uniqueRemappedEdges.insert( { remapped_u, remapped_v } );
      } else {
        if ( remapped_u > remapped_v ) {
          std::swap( remapped_u, remapped_v );
        }
        uniqueRemappedEdges.insert( { remapped_u, remapped_v } );
      }
    }

    return { counter, uniqueRemappedEdges };
  }

  Graph build_graph_from( Vertex n, const std::set<Edge> &edges ) {
    Graph g( static_cast<std::size_t>( n ) );  // Aqui está sendo criado um grafo de 0..n-1
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

}  // namespace r3dp
