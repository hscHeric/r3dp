#include "CLI/CLI.hpp"
#include "r3dp.hpp"

#include <boost/graph/graph_traits.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

int main( int argc, char **argv ) {
  CLI::App r3dp_cli( "Metaheurísticas para o problema {3} roman domination" );
  argv = r3dp_cli.ensure_utf8( argv );

  std::string filename = "Default";
  r3dp_cli.add_option( "-f, --file", filename, "Path para o arquivo edges.txt" );

  CLI11_PARSE( r3dp_cli, argc, argv );

  auto result = r3dp::core::read_graph_from_file( filename );

  r3dp::core::Vertex         totalVertices = result.first;
  std::set<r3dp::core::Edge> edges         = result.second;

  auto graph = r3dp::core::build_graph_from( totalVertices, edges );

  std::cout << "=========================================\n";
  std::cout << "Resumo do Grafo\n";
  std::cout << "-----------------------------------------\n";
  std::cout << "Total de vertices:      " << boost::num_vertices( graph ) << "\n";
  std::cout << "Total de arestas únicas:" << boost::num_edges( graph ) << "\n";
  std::cout << "-----------------------------------------\n";

  std::cout << "Arestas remapeadas (u, v):\n";
  for ( const auto &edge : edges ) {
    std::cout << "  " << edge.first << " -- " << edge.second << "\n";
  }

  std::cout << "-----------------------------------------\n";
  std::cout << "Grau de cada vértice:\n";
  for ( r3dp::core::Vertex v = 0; v < boost::num_vertices( graph ); ++v ) {
    std::cout << "  v" << v << " : grau = " << boost::degree( v, graph ) << "\n";
  }

  std::cout << "=========================================\n";
  return EXIT_SUCCESS;
}
