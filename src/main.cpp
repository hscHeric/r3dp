#include "CLI/CLI.hpp"
#include "core/graph.hpp"
#include "genetic_algorithm/genetic_algorithm.hpp"
#include "genetic_algorithm/pop_generator.hpp"
#include "genetic_algorithm/r3d_decoder.hpp"

#include <boost/graph/graph_traits.hpp>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using r3dp::core::is_valid_fdr3;
using json = nlohmann::json;

int main( int argc, char **argv ) {
  CLI::App app( "Metaheurísticas para o problema {3}-Roman Domination (r3dp)" );

  // ---------------- CLI args ----------------
  std::string file_edges;
  app.add_option( "-f,--file", file_edges, "Arquivo de arestas (edges.txt)" )->required();

  unsigned int population_size = 200;
  app.add_option( "-p,--pop-size", population_size, "Tamanho da população (>= 2)" )
    ->check( CLI::Range( 2u, UINT_MAX ) );

  unsigned int generations = 0;
  app.add_option( "-g,--generations", generations, "Máximo de gerações (0 = desabilita)" )
    ->check( CLI::Range( 0u, UINT_MAX ) );

  double mutation_rate = 0.01;
  app.add_option( "-m,--mutation", mutation_rate, "Taxa de mutação em [0,1]" )
    ->check( CLI::Range( 0.0, 1.0 ) );

  double crossover_rate = 0.8;
  app.add_option( "-c,--crossover", crossover_rate, "Taxa de crossover em [0,1]" )
    ->check( CLI::Range( 0.0, 1.0 ) );

  double elite_rate = 0.10;
  app.add_option( "-e,--elite", elite_rate, "Fração de elite em [0,1]" )
    ->check( CLI::Range( 0.0, 1.0 ) );

  unsigned int tournament_size = 5;
  app.add_option( "-t,--tournament", tournament_size, "Tamanho do torneio (>= 1)" )
    ->check( CLI::Range( 1u, UINT_MAX ) );

  unsigned int max_threads = 8;
  app.add_option( "-j,--threads", max_threads, "Número de threads (>= 1)" )
    ->check( CLI::Range( 1u, UINT_MAX ) );

  // Parada por tempo
  unsigned int time_limit = 0;
  app.add_option( "--time-limit", time_limit, "Tempo máximo em segundos (0 = desabilita)" );

  // Parada por estagnação
  unsigned int patience = 0;
  app.add_option(
    "--patience", patience, "Parar se não houver melhora por N gerações (0 = desabilita)" );

  double min_delta = 0.0;
  app.add_option( "--min-delta", min_delta, "Melhora mínima para resetar a paciência" )
    ->check( CLI::Range( 0.0, 1e100 ) );

  unsigned int log_every = 0;
  app.add_option( "--log-every", log_every, "Logar a cada N gerações (0 = silencioso)" );

  std::string out_path;
  app.add_option( "-o,--out", out_path, "Arquivo para salvar melhor cromossomo (texto)" );

  // Novo: caminho do JSON (opcional). Se vazio, cai em <nome_do_grafo>_result.json
  std::string json_path;
  app.add_option( "--json", json_path, "Arquivo JSON para salvar os resultados" );

  // Parse
  argv = app.ensure_utf8( argv );
  CLI11_PARSE( app, argc, argv );

  try {
    // ---------------- Grafo ----------------
    auto [totalVertices, edges] = r3dp::core::read_graph_from_file( file_edges );
    auto graph                  = r3dp::core::build_graph_from( totalVertices, edges );
    auto delta_g                = r3dp::core::max_degree( graph );

    // Derivar nome do grafo a partir do path do arquivo (sem extensão)
    namespace fs           = std::filesystem;
    std::string graph_name = fs::path( file_edges ).stem().string();

    std::cout << "Resumo do Grafo:\n"
              << "  Vértices: " << boost::num_vertices( graph ) << "\n"
              << "  Arestas:  " << boost::num_edges( graph ) << "\n"
              << "  Grau máximo: " << delta_g << "\n";

    // ---------------- GA ----------------
    r3dp::core::DefaultRNG rng;
    r3dp::ga::R3DDecoder   decoder( graph );

    auto init_pop =
      r3dp::ga::create_random_population( population_size, boost::num_vertices( graph ), rng );

    r3dp::ga::GAEngine<r3dp::core::DefaultRNG, r3dp::ga::R3DDecoder> ga_engine( init_pop,
                                                                                mutation_rate,
                                                                                crossover_rate,
                                                                                elite_rate,
                                                                                tournament_size,
                                                                                max_threads,
                                                                                rng,
                                                                                decoder );

    auto start = std::chrono::steady_clock::now();

    double              best  = ga_engine.best_fitness();
    unsigned int        stale = 0;
    unsigned int        g     = 0;
    std::vector<double> fitness_improvements;

    while ( true ) {
      ++g;
      ga_engine.evolve();

      double current  = ga_engine.best_fitness();
      bool   improved = ( best - current ) > min_delta;
      if ( improved ) {
        best  = current;
        stale = 0;

        fitness_improvements.push_back( current );
      } else {
        stale++;
      }

      if ( log_every > 0 && g % log_every == 0 ) {
        std::cout << "Geração " << g << " | melhor custo: " << current << " | estagnação: " << stale
                  << "\n";
      }

      // Critérios de parada
      bool stop = false;
      if ( generations > 0 && g >= generations )
        stop = true;
      if ( time_limit > 0 ) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::steady_clock::now() - start )
                         .count();
        if ( elapsed >= time_limit )
          stop = true;
      }
      if ( patience > 0 && stale >= patience )
        stop = true;

      if ( stop )
        break;
    }

    auto      end = std::chrono::steady_clock::now();
    long long elapsed_us =
      std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();

    const auto &best_chromosome = ga_engine.get_best_chromosome();
    std::cout << "\nResultado final após " << g << " gerações:\n";
    std::cout << "Melhor aptidão (custo): " << ga_engine.best_fitness() << "\n";

    if ( is_valid_fdr3( graph, best_chromosome ) ) {
      std::cout << "A solução encontrada é VÁLIDA.\n";
    } else {
      auto violating_vertices = r3dp::core::violating_vertices_fdr3( graph, best_chromosome );
      std::cout << "A solução encontrada é INVÁLIDA. Vértices violados:\n";
      for ( const auto &v : violating_vertices ) {
        std::cout << v << " ";
      }
      std::cout << "\n";
    }

    std::cout << "Melhor cromossomo: ";
    for ( auto gene : best_chromosome )
      std::cout << int( gene ) << " ";
    std::cout << "\n";

    if ( !out_path.empty() ) {
      std::ofstream ofs( out_path );
      for ( std::size_t i = 0; i < best_chromosome.size(); ++i ) {
        if ( i )
          ofs << " ";
        ofs << int( best_chromosome[i] );
      }
      ofs << "\n";
      std::cout << "Cromossomo salvo em: " << out_path << "\n";
    }

    if ( json_path.empty() ) {
      json_path = graph_name + "_result.json";
    }

    json j;
    j["graph"]         = graph_name;
    j["vertices"]      = static_cast<unsigned int>( boost::num_vertices( graph ) );
    j["edges"]         = static_cast<unsigned int>( boost::num_edges( graph ) );
    j["elapsed_us"]    = elapsed_us;
    j["final_fitness"] = ga_engine.best_fitness();

    j["labels"] = json::array();
    for ( auto gene : best_chromosome ) {
      j["labels"].push_back( static_cast<int>( gene ) );
    }

    // vetor com o fitness em cada melhora
    j["fitness_improvements"] = fitness_improvements;

    // grava arquivo
    {
      std::ofstream jofs( json_path );
      if ( !jofs ) {
        throw std::runtime_error( "Não foi possível abrir o arquivo JSON para escrita: " +
                                  json_path );
      }
      jofs << std::setw( 2 ) << j << "\n";
    }
    std::cout << "JSON salvo em: " << json_path << "\n";

  } catch ( const std::exception &e ) {
    std::cerr << "Erro: " << e.what() << "\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
