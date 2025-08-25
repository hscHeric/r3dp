#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <cstdint>
#include <string>
#include <utility>

#define R3DP_VERSION_MAJOR  0
#define R3DP_VERSION_MINOR  0
#define R3DP_VERSION_PATCH  1
#define R3DP_VERSION_STRING "0.0.1"

namespace r3dp {
  using Vertex = std::uint32_t;
  using Edge   = std::pair<Vertex, Vertex>;
  using Graph = boost::adjacency_list<boost::setS,  // Arestas armazenadas em um SET (grafo simples)
                                      boost::vecS,  // Vértices armazenados de 0 a n
                                      boost::undirectedS  // Grafo não dirigido
                                      >;

  std::pair<Vertex, std::set<Edge>> read_graph_from_file( const std::string &file_path );
}  // namespace r3dp
