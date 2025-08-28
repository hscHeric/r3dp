#include "r3dp.hpp"

#include "CLI/CLI.hpp"
#include "core/graph.hpp"
#include "core/rng.hpp"
#include "genetic_algorithm/genetic_algorithm.hpp"
#include "genetic_algorithm/pop_generator.hpp"
#include "genetic_algorithm/r3d_decoder.hpp"

#include <climits>
#include <cstdint>

using json = nlohmann::json;

namespace r3dp {

  static double calculate_density( unsigned int vertices, unsigned int edges ) {
    if ( vertices <= 1 )
      return 0.0;  // Densidade é zero para grafos sem ou com um único vértice
    double max_edges = ( vertices * ( vertices - 1 ) ) / 2.0;
    return edges / max_edges;
  }

  nlohmann::json parse_and_run_ga( int argc, char **argv ) {
    CLI::App app( "Algoritmo Genético para o problema {3}-Roman Domination (r3dp)" );

    std::string file_edges;
    app.add_option( "-f,--file", file_edges, "Arquivo de arestas (edges.txt)" )
      ->required();  // O arquivo de arestas é obrigatório para a execução

    unsigned int population_size = 200;
    app.add_option( "-p,--pop-size", population_size, "Tamanho da população (>= 2)" )
      ->check( CLI::Range( 2u, UINT_MAX ) );

    double mutation_rate = 0.03;
    app.add_option( "-m,--mutation", mutation_rate, "Taxa de mutação em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double crossover_rate = 0.9;
    app.add_option( "-c,--crossover", crossover_rate, "Taxa de crossover em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double elite_rate = 0.1;
    app.add_option( "-e,--elite", elite_rate, "Fração de elite em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    unsigned int tournament_size = 5;
    app.add_option( "-t,--tournament", tournament_size, "Tamanho do torneio (>= 1)" )
      ->check( CLI::Range( 1u, UINT_MAX ) );

    unsigned int max_threads = 8;
    app.add_option( "-j,--threads", max_threads, "Número de threads (>= 1)" )
      ->check( CLI::Range( 1u, UINT_MAX ) );

    unsigned int time_limit = 0;
    app.add_option( "--time-limit", time_limit, "Tempo máximo em segundos (0 = desabilita)" )
      ->required();  // O tempo limite é obrigatório para a execução.

    std::string out_path;
    app.add_option( "-o,--out", out_path, "Arquivo para salvar melhor cromossomo (texto)" );

    // Parâmetro para o número de tentativas.
    unsigned int trials = 1;
    app.add_option( "--trials", trials, "Número de tentativas (>= 1)" )
      ->check( CLI::Range( 1u, UINT_MAX ) );

    argv = app.ensure_utf8( argv );
    CLI11_PARSE( app, argc, argv );

    try {
      auto [totalVertices, edges] = core::read_graph_from_file( file_edges );
      auto graph                  = core::build_graph_from( totalVertices, edges );
      auto delta_g                = core::max_degree( graph );

      namespace fs           = std::filesystem;
      std::string graph_name = fs::path( file_edges ).stem().string();
      DEBUG_PRINT( "Resumo do Grafo:\n"
                   << " Nome do Grafo: " << graph_name << "\n"
                   << "  Vértices: " << boost::num_vertices( graph ) << "\n"
                   << "  Arestas:  " << boost::num_edges( graph ) << "\n"
                   << "  Grau máximo: " << delta_g << "\n" );

      double density = calculate_density( boost::num_vertices( graph ), boost::num_edges( graph ) );
      json   all_trials_results = json::array();
      uint64_t best_fitness     = UINT64_MAX;

      core::DefaultRNG rng;
      ga::R3DDecoder   decoder( graph );

      std::vector<ga::Heuristic> heuristics{ ga::h1, ga::h2, ga::h3, ga::h4 };
      for ( size_t trial = 0; trial < trials; ++trial ) {
        DEBUG_PRINT( "\n--- Rodando tentativa " << trial + 1 << " de " << trials << " ---" );

        auto init_pop = ga::generate_population( graph, population_size, heuristics, rng );
        ga::GAEngine<core::DefaultRNG, ga::R3DDecoder> ga_engine( init_pop,
                                                                  mutation_rate,
                                                                  crossover_rate,
                                                                  elite_rate,
                                                                  tournament_size,
                                                                  max_threads,
                                                                  rng,
                                                                  decoder );

        auto   start = std::chrono::steady_clock::now();
        double best  = ga_engine.best_fitness();
      }

    } catch ( const std::exception &e ) {
      std::cerr << "Erro: " << e.what() << "\n";
      return json::array();
    }
  }
}  // namespace r3dp
