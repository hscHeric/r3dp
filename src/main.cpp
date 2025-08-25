#include "CLI/CLI.hpp"
#include "genetic_algorithm/pop_generator.hpp"
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

  auto                       result        = r3dp::core::read_graph_from_file( filename );
  r3dp::core::Vertex         totalVertices = result.first;
  std::set<r3dp::core::Edge> edges         = result.second;
  auto                       graph         = r3dp::core::build_graph_from( totalVertices, edges );

  std::cout << "=========================================\n";
  std::cout << "Resumo do Grafo\n";
  std::cout << "-----------------------------------------\n";
  std::cout << "Total de vertices:      " << boost::num_vertices( graph ) << "\n";
  std::cout << "Total de arestas únicas:" << boost::num_edges( graph ) << "\n";
  std::cout << "-----------------------------------------\n";

  // --- Rodando o Algoritmo Genético ---
  const unsigned int population_size = 100;
  const unsigned int generations     = 50;
  const double       mutation_rate   = 0.01;
  const double       crossover_rate  = 0.8;
  const double       elite_rate      = 0.1;
  const unsigned int tournament_size = 5;
  const unsigned int max_threads     = 4;

  r3dp::core::DefaultRNG rng;
  r3dp::ga::R3DDecoder   decoder( graph );

  // Criação da população inicial usando a nova função
  std::vector<r3dp::ga::Chromosome> initial_population =
    r3dp::ga::create_random_population( population_size, boost::num_vertices( graph ), rng );

  // Instancia o motor do GA
  r3dp::ga::GAEngine<r3dp::core::DefaultRNG, r3dp::ga::R3DDecoder> ga_engine( initial_population,
                                                                              mutation_rate,
                                                                              crossover_rate,
                                                                              elite_rate,
                                                                              tournament_size,
                                                                              max_threads,
                                                                              rng,
                                                                              decoder );

  // Executa as gerações
  ga_engine.evolve( generations );

  // Exibe o resultado final
  const auto &best_chromosome = ga_engine.get_best_chromosome();
  std::cout << "Melhor aptidão (custo): " << ga_engine.best_fitness() << std::endl;
  std::cout << "Melhor cromossomo: ";
  for ( const auto &gene : best_chromosome ) {
    std::cout << static_cast<int>( gene ) << " ";
  }
  std::cout << std::endl;

  std::cout << "=========================================\n";
  return EXIT_SUCCESS;
}
