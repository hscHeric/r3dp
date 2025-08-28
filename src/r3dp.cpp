#include "CLI/CLI.hpp"
#include "core/graph.hpp"
#include "core/rng.hpp"
#include "genetic_algorithm/genetic_algorithm.hpp"
#include "genetic_algorithm/pop_generator.hpp"
#include "genetic_algorithm/r3d_decoder.hpp"

#include <cstdint>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace r3dp {

  int run_ga( int argc, char *argv[] ) {
    CLI::App app{ "Algoritmo genético para o problema da dominação {3}-romana " };

    std::string file_path = "default.txt";
    app.add_option( "-f,--file", file_path, "Arquivo de arestas (edges.txt)" )->required();

    unsigned int time_limit = 0;
    app.add_option( "--time-limit", time_limit, "Tempo máximo em segundos" )->required();

    std::string output_path = "default.json";  // Valor padrão para o arquivo de saída
    app.add_option( "-o,--output", output_path, "Arquivo de resultados (results.json)" )
      ->required();

    unsigned int population_size = 200;
    app.add_option( "-p,--pop-size", population_size, "Tamanho da população (>= 2)" )
      ->check( CLI::Range( 2u, UINT_MAX ) );

    unsigned int tournament_size = 2;
    app
      .add_option(
        "-t,--tournament", population_size, "Tamanho do torneio (Operador de Seleção) (>= 2)" )
      ->check( CLI::Range( 2u, UINT_MAX ) );

    double crossover_rate = 0.8;
    app.add_option( "-c,--crossover", crossover_rate, "Taxa de crossover em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double mutation_rate = 0.01;
    app.add_option( "-m,--mutation", mutation_rate, "Taxa de mutação em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double elite_rate = 0.10;
    app.add_option( "-e,--elite", elite_rate, "Fração de elite em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    unsigned int max_threads = 1;
    app.add_option( "-j,--threads", max_threads, "Número de threads (>= 1)" )
      ->check( CLI::Range( 1u, UINT_MAX ) );

    unsigned int trials = 1;
    app.add_option( "-r,--runs", trials, "Número de tentativas" )
      ->check( CLI::Range( 1u, UINT_MAX ) );

    argv = app.ensure_utf8( argv );
    CLI11_PARSE( app, argc, argv );

    auto [totalVertices, edges] = core::read_graph_from_file( file_path );
    auto        graph           = core::build_graph_from( totalVertices, edges );
    std::string graph_name      = std::filesystem::path( file_path ).stem().string();

    core::DefaultRNG rng;
    ga::R3DDecoder   decoder( graph );

    uint64_t                                 best_fitness = std::numeric_limits<uint64_t>::max();
    std::vector<std::pair<uint64_t, double>> best_improvements;
    std::vector<ga::Heuristic>               heuristics = { ga::h1, ga::h2, ga::h3, ga::h4 };
    for ( size_t trial = 0; trial < trials; ++trial ) {
      auto init_pop = ga::generate_population( graph, population_size, heuristics, rng );
      ga::GAEngine<core::DefaultRNG, ga::R3DDecoder> ga_engine( init_pop,
                                                                mutation_rate,
                                                                crossover_rate,
                                                                elite_rate,
                                                                tournament_size,
                                                                max_threads,
                                                                rng,
                                                                decoder );

      uint64_t local_best_fitness = std::numeric_limits<uint64_t>::max();
      auto     start_time         = std::chrono::steady_clock::now();

      while ( true ) {
        auto [current_best_chromosome, current_fitness] = ga_engine.evolve();

        if ( current_fitness < local_best_fitness ) {
          local_best_fitness = current_fitness;
          best_improvements.push_back(
            { local_best_fitness,
              std::chrono::duration<double>( std::chrono::steady_clock::now() - start_time )
                .count() } );
        }

        auto                          current_time    = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = current_time - start_time;

        if ( elapsed_seconds.count() >= time_limit ) {
          break;
        }
      }

      if ( local_best_fitness < best_fitness ) {
        best_fitness = local_best_fitness;
      }
    }

    // Criando o JSON com os resultados
    nlohmann::json result;
    result["graph_name"]        = graph_name;
    result["num_vertices"]      = totalVertices;
    result["num_edges"]         = edges.size();
    result["best_fitness"]      = best_fitness;
    result["best_improvements"] = best_improvements;
    result["num_trials"]        = trials;

    std::ofstream ofs( output_path );
    if ( ofs ) {
      ofs << result.dump( 4 );  // Escreve o JSON com indentação de 4 espaços
      std::cout << "Resultado salvo em: " << output_path << std::endl;
    } else {
      std::cerr << "Erro ao salvar o arquivo JSON." << std::endl;
    }

    return 0;
  }

}  // namespace r3dp
