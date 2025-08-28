#include "r3dp.hpp"

#include "CLI/CLI.hpp"
#include "core/graph.hpp"
#include "core/rng.hpp"
#include "genetic_algorithm/genetic_algorithm.hpp"
#include "genetic_algorithm/pop_generator.hpp"
#include "genetic_algorithm/r3d_decoder.hpp"

#include <chrono>
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

    unsigned int population_size = 200;
    app.add_option( "-p,--pop-size", population_size, "Tamanho da população (>= 2)" )
      ->check( CLI::Range( 2u, UINT_MAX ) );

    unsigned int tournament_size = 2;
    app
      .add_option(
        "-t,--tournament", tournament_size, "Tamanho do torneio (Operador de Seleção) (>= 2)" )
      ->check( CLI::Range( 2u, UINT_MAX ) );

    double mutation_rate = 0.01;
    app.add_option( "-m,--mutation", mutation_rate, "Taxa de mutação em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double crossover_rate = 0.8;
    app.add_option( "-c,--crossover", crossover_rate, "Taxa de crossover em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double elite_rate = 0.10;
    app.add_option( "-e,--elite", elite_rate, "Fração de elite em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    unsigned int max_threads = 1;
    app.add_option( "-j,--threads", max_threads, "Número de threads (>= 1)" )
      ->check( CLI::Range( 1u, UINT_MAX ) );

    unsigned int time_limit = 0;
    app.add_option( "--time-limit", time_limit, "Tempo máximo em segundos" )->required();

    std::string output_path = "default.json";
    app.add_option( "-o,--output", output_path, "Arquivo de resultados (results.json)" )
      ->required();

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

    uint64_t best_fitness_global = std::numeric_limits<uint64_t>::max();
    std::vector<std::pair<uint64_t, double>> best_improvements;

    auto total_start_time = std::chrono::steady_clock::now();

    for ( size_t trial = 0; trial < trials; ++trial ) {
      // Criando o GAEngine para cada tentativa
      auto initial_population =
        ga::generate_population( graph, population_size, { ga::h1, ga::h2, ga::h3, ga::h4 }, rng );
      ga::GAEngine<core::DefaultRNG, ga::R3DDecoder> ga_engine( initial_population,
                                                                mutation_rate,
                                                                crossover_rate,
                                                                elite_rate,
                                                                tournament_size,
                                                                max_threads,
                                                                rng,
                                                                decoder );

      uint64_t current_best_fitness_in_trial = std::numeric_limits<uint64_t>::max();

      // Loop de evolução baseado no tempo
      while ( true ) {
        auto [current_best_chromosome, current_fitness] = ga_engine.evolve();

        if ( current_fitness < current_best_fitness_in_trial ) {
          current_best_fitness_in_trial = current_fitness;
        }

        if ( current_best_fitness_in_trial < best_fitness_global ) {
          best_fitness_global                              = current_best_fitness_in_trial;
          auto                          current_total_time = std::chrono::steady_clock::now();
          std::chrono::duration<double> elapsed_total_seconds =
            current_total_time - total_start_time;
          best_improvements.push_back( { best_fitness_global, elapsed_total_seconds.count() } );
        }

        auto                          current_total_time    = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_total_seconds = current_total_time - total_start_time;

        if ( elapsed_total_seconds.count() >= time_limit ) {
          break;
        }
      }
    }

    nlohmann::json result;
    result["graph_name"]        = graph_name;
    result["num_vertices"]      = totalVertices;
    result["num_edges"]         = edges.size();
    result["best_fitness"]      = best_fitness_global;
    result["best_improvements"] = best_improvements;
    result["num_trials"]        = trials;

    std::ofstream ofs( output_path );
    if ( ofs ) {
      ofs << result.dump( 4 );
      LOG_MESSAGE( "Resultado salvo em: " << output_path );
    } else {
      LOG_ERR( "Erro ao salvar arquivo JSON" )
    }

    return 0;
  }
}  // namespace r3dp
