#include "graph.hpp"

#include <fstream>
#include <iostream>

namespace r3dp::core {
  std::pair<Vertex, std::set<Edge>> read_graph_from_file( const std::string &filePath ) {
    std::ifstream file( filePath );
    if ( !file.is_open() ) {
      std::cerr << "Erro ao abrir o arquivo: " << filePath << std::endl;
      return { 0, {} };
    }

    std::set<Vertex>  uniqueVertices;
    std::vector<Edge> originalEdges;
    Vertex            u, v;

    // Lê todas as arestas e todos os vértices
    while ( file >> u >> v ) {
      uniqueVertices.insert( u );
      uniqueVertices.insert( v );
      originalEdges.push_back( { u, v } );
    }

    std::unordered_map<Vertex, Vertex> remapping;
    Vertex                             counter = 0;
    for ( const auto &vertex : uniqueVertices ) {
      remapping[vertex] = counter++;
    }

    // Criar as arestas remapeadas e remover duplicatas
    std::set<Edge> uniqueRemappedEdges;
    for ( const auto &edge : originalEdges ) {
      Vertex remapped_u = remapping.at( edge.first );
      Vertex remapped_v = remapping.at( edge.second );

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

}  // namespace r3dp::core
