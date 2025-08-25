#include "r3dp.hpp"

#include <cstdlib>
#include <iostream>

int main( int argc, char **argv ) {
  std::cout << "Hello World!\n";
  std::string filename = "graph_data.txt";
  auto        result   = r3dp::read_graph_from_file( filename );

  r3dp::Vertex         totalVertices = result.first;
  std::set<r3dp::Edge> edges         = result.second;

  std::cout << "Total de vertices: " << totalVertices << std::endl;
  std::cout << "Total de arestas unicas: " << edges.size() << std::endl;
  std::cout << "Arestas remapeadas (u, v):" << std::endl;
  for ( const auto &edge : edges ) {
    std::cout << edge.first << " " << edge.second << std::endl;
  }
  return EXIT_SUCCESS;
}
