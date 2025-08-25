#include "r3dp.hpp"

#include <fstream>
#include <iostream>

namespace r3dp {
  std::pair<r3dp::Vertex, std::set<r3dp::Edge>> readGraphFromFile( const std::string &filePath ) {
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
}  // namespace r3dp
