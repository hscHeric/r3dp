#include "r3dp.hpp"

#include "CLI/CLI.hpp"
#include "brkga/brkga.hpp"
#include "brkga/brkga_decoder.hpp"
#include "brkga/mt_rand.hpp"
#include "core/graph.hpp"
#include "core/rng.hpp"
#include "genetic_algorithm/genetic_algorithm.hpp"
#include "genetic_algorithm/pop_generator.hpp"
#include "genetic_algorithm/r3d_decoder.hpp"

#include <boost/graph/detail/adjacency_list.hpp>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>
#include <random>
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
      LOG_MESSAGE( "Iniciando tentativa: " << trial );

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

      auto trial_start_time = std::chrono::steady_clock::now();
      auto per_trial_limit  = std::chrono::seconds( static_cast<long long>( time_limit ) );

      uint64_t current_best_fitness_in_trial = std::numeric_limits<uint64_t>::max();

      while ( true ) {
        auto [current_best_chromosome, current_fitness] = ga_engine.evolve();

        if ( current_fitness < current_best_fitness_in_trial ) {
          current_best_fitness_in_trial = current_fitness;
        }

        if ( current_best_fitness_in_trial < best_fitness_global ) {
          LOG_MESSAGE( "Novo melhor fitness encontrado: " << best_fitness_global << " -> "
                                                          << current_best_fitness_in_trial );
          best_fitness_global = current_best_fitness_in_trial;

          auto                          now                   = std::chrono::steady_clock::now();
          std::chrono::duration<double> elapsed_total_seconds = now - total_start_time;
          best_improvements.push_back( { best_fitness_global, elapsed_total_seconds.count() } );
        }

        auto now = std::chrono::steady_clock::now();
        if ( time_limit > 0 ) {
          std::chrono::duration<double> elapsed_trial_seconds = now - trial_start_time;
          if ( elapsed_trial_seconds.count() >= time_limit ) {
            LOG_MESSAGE( "Tempo da tentativa " << trial << " atingido (" << time_limit << "s)" );
            break;
          }
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

  static long unsigned generate_random_seed() {
    std::random_device                           rd;
    std::mt19937_64                              gen( rd() );
    std::uniform_int_distribution<long unsigned> dist;

    return dist( gen );
  }

  int run_brkga( int argc, char **argv ) {
    CLI::App app{
      "Algoritmo genético de chave aleatória enviesada para o problema da dominação {3}-romana "
    };

    std::string file_path = "default.txt";
    app.add_option( "-f,--file", file_path, "Arquivo de arestas (edges.txt)" )->required();

    unsigned int population_size = 200;
    app.add_option( "-p,--pop-size", population_size, "Tamanho da população (>= 2)" )
      ->check( CLI::Range( 2u, UINT_MAX ) );

    double elite_fraction = 0.10;
    app
      .add_option(
        "--elite-fraction", elite_fraction, "Fração da população que pertence a elite em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double mutants_fraction = 0.10;
    app
      .add_option( "--mutants-fraction",
                   mutants_fraction,
                   "Fração da população que vai ser substituida por mutantes em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    double elite_inheritance_prob = 0.70;
    app
      .add_option( "--elite-inheritance-prob",
                   elite_inheritance_prob,
                   "Probabilidade de o descendente herdar o alelo do pai elite em [0,1]" )
      ->check( CLI::Range( 0.0, 1.0 ) );

    unsigned num_populations = 3;
    app
      .add_option( "--num-populations",
                   num_populations,
                   "Número de populações independentes (inteiro positivo)" )
      ->check( CLI::PositiveNumber );

    unsigned int max_threads = 1;
    app.add_option( "-j,--threads", max_threads, "Número de threads (>= 1)" )
      ->check( CLI::Range( CLI::PositiveNumber ) );

    unsigned int time_limit = 0;
    app.add_option( "--time-limit", time_limit, "Tempo máximo em segundos" )
      ->check( CLI::PositiveNumber )
      ->required();

    unsigned migration_interval = 100;
    app
      .add_option( "--migration-interval",
                   migration_interval,
                   "Número de gerações entre trocas de indivíduos entre populações" )
      ->check( CLI::PositiveNumber );

    unsigned migration_size = 2;
    app
      .add_option( "--migration-size",
                   migration_size,
                   "Número de melhores indivíduos a serem trocados entre populações" )
      ->check( CLI::PositiveNumber );

    std::string output_path = "default.json";
    app.add_option( "-o,--output", output_path, "Arquivo de resultados (results.json)" )
      ->required();

    unsigned int trials = 1;
    app.add_option( "-r,--runs", trials, "Número de tentativas" )
      ->check( CLI::Range( 1u, UINT_MAX ) );

    argv = app.ensure_utf8( argv );
    CLI11_PARSE( app, argc, argv );

    long unsigned seed_to_use = generate_random_seed();
    brkga::MTRand rng( seed_to_use );

    auto [totalVertices, edges] = core::read_graph_from_file( file_path );
    const auto  graph           = core::build_graph_from( totalVertices, edges );
    std::string graph_name      = std::filesystem::path( file_path ).stem().string();

    LOG_VAR( totalVertices );
    LOG_VAR( edges.size() );
    LOG_VAR( graph_name );

    double                                 best_fitness_global = std::numeric_limits<double>::max();
    std::vector<std::pair<double, double>> best_improvements;

    for ( size_t trial = 0; trial < trials; trial++ ) {
      LOG_MESSAGE( "Iniciando tentativa: " << trial );

      brkga::R3DPDecoder                              decoder( graph );
      brkga::BRKGA<brkga::R3DPDecoder, brkga::MTRand> algorithm( boost::num_vertices( graph ),
                                                                 population_size,
                                                                 elite_fraction,
                                                                 mutants_fraction,
                                                                 elite_inheritance_prob,
                                                                 decoder,
                                                                 rng,
                                                                 num_populations,
                                                                 max_threads );

      unsigned generation       = 0;
      auto     trial_start_time = std::chrono::steady_clock::now();
      auto     per_trial_limit  = std::chrono::seconds( static_cast<long long>( time_limit ) );

      double current_best_fitness_in_trial = std::numeric_limits<double>::max();
      std::vector<std::pair<double, double>> current_improvements;

      while ( std::chrono::steady_clock::now() - trial_start_time < per_trial_limit ) {
        algorithm.evolve();
        generation++;

        double current_best = algorithm.getBestFitness();

        if ( current_best < current_best_fitness_in_trial ) {
          current_best_fitness_in_trial = current_best;
          auto   now                    = std::chrono::steady_clock::now();
          double seconds = std::chrono::duration<double>( now - trial_start_time ).count();

          current_improvements.emplace_back( current_best, seconds );

          LOG_MESSAGE( "Novo melhor fitness encontrado: " << current_best );
        }

        if ( migration_interval > 0 && generation % migration_interval == 0 ) {
          if ( migration_size > 0 ) {
            algorithm.exchangeElite( migration_size );
          }
        }
      }

      if ( current_best_fitness_in_trial < best_fitness_global ) {
        best_fitness_global = current_best_fitness_in_trial;
        best_improvements   = current_improvements;
      }
    }

    nlohmann::json result;
    result["graph_name"]   = graph_name;
    result["num_vertices"] = totalVertices;
    result["num_edges"]    = edges.size();
    result["best_fitness"] = best_fitness_global;
    result["trials"]       = trials;

    nlohmann::json improvements_json = nlohmann::json::array();
    for ( const auto &[fitness, time] : best_improvements ) {
      improvements_json.push_back( { { "fitness", fitness }, { "time", time } } );
    }
    result["best_improvements"] = improvements_json;

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
