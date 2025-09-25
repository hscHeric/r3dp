#define DEBUG
#include "CLI/CLI.hpp"
#include "core/graph.hpp"
#include "core/log.hpp"
#include "meta/brkga/brkga.hpp"
#include "meta/brkga/brkga_decoder.hpp"
#include "meta/brkga/mt_rand.hpp"

#include <cstdint>
#include <filesystem>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <random>
#include <string>
#include <utility>

static uint64_t generate_random_seed() {
  std::random_device                      rd;
  std::mt19937_64                         gen( rd() );
  std::uniform_int_distribution<uint64_t> dist;
  return dist( gen );
}

constexpr unsigned DEFAULT_POPULATION_SIZE    = 5;      // >= 2
constexpr double   DEFAULT_ELITE_FRACTION     = 0.20;   // [0,1]
constexpr double   DEFAULT_MUTANT_FRACTION    = 0.057;  // [0,1]
constexpr double   DEFAULT_ELITE_INHERIT_PROB = 0.70;   // [0,1]
constexpr unsigned DEFAULT_NUM_POPULATIONS    = 3;      // >= 1
constexpr unsigned DEFAULT_NUM_THREADS        = 1;      // >= 1
constexpr unsigned DEFAULT_MIGRATION_INTERVAL = 100;    // >= 1
constexpr unsigned DEFAULT_MIGRATION_SIZE     = 2;      // >= 0
constexpr unsigned DEFAULT_MAX_GENERATIONS    = 0;      // 0 = desabilitado
constexpr unsigned DEFAULT_TIME_LIMIT_SECONDS = 0;      // obrigatório (>0)
constexpr uint64_t DEFAULT_RNG_SEED           = 0;      // 0 = aleatória
constexpr unsigned DEFAULT_NUM_TRIALS         = 1;      // >= 1

struct convergence_point {
  double elapsed_seconds{};
  double fitness_value{};

  friend void to_json( nlohmann::json &j, const convergence_point &p ) {
    j = nlohmann::json{ { "elapsed_seconds", p.elapsed_seconds },
                        { "fitness_value", p.fitness_value } };
  }
};

struct trial_result {
  double                         best_fitness_value = std::numeric_limits<double>::infinity();
  std::vector<convergence_point> convergence_points;
  std::chrono::steady_clock::time_point start_time_point;

  void start_timer() noexcept {
    start_time_point = std::chrono::steady_clock::now();
  }

  void add_point( double fitness_value_now ) {
    const double t = std::chrono::duration_cast<std::chrono::duration<double>>(
                       std::chrono::steady_clock::now() - start_time_point )
                       .count();
    convergence_points.push_back( { t, fitness_value_now } );
  }

  // Apenas salva o valor do melhor fitness e os pontos da curva de convergência
  friend void to_json( nlohmann::json &j, const trial_result &t ) {
    j = nlohmann::json{ { "best_fitness_value", t.best_fitness_value },
                        { "convergence_points", t.convergence_points } };
  }
};

struct graph_summary {
  std::string   graph_name;
  std::uint32_t vertex_count = 0;
  std::uint64_t edge_count   = 0;
  double        density      = 0.0;

  static constexpr double compute_density( std::uint32_t n, std::uint64_t m ) noexcept {
    if ( n < 2 ) {
      return 0.0;
    }
    const long double num = 2.0L * static_cast<long double>( m );
    const long double den = static_cast<long double>( n ) * static_cast<long double>( n - 1U );
    return static_cast<double>( num / den );
  }

  friend void to_json( nlohmann::json &j, const graph_summary &g ) {
    j = nlohmann::json{ { "graph_name", g.graph_name },
                        { "vertex_count", g.vertex_count },
                        { "edge_count", g.edge_count },
                        { "density", g.density } };
  }
};

struct run_results {
  graph_summary             graph;
  std::uint64_t             seed = 0;
  std::vector<trial_result> trials;

  [[nodiscard]] unsigned trial_count() const noexcept {
    return static_cast<unsigned>( trials.size() );
  }

  trial_result &create_trial() {
    trials.emplace_back();
    return trials.back();
  }

  friend void to_json( nlohmann::json &j, const run_results &r ) {
    j = nlohmann::json{ { "graph", r.graph },
                        { "seed", r.seed },
                        { "trial_count", r.trial_count() },
                        { "trials", r.trials } };
  }

  void save_json( const std::string &filename, int indent = 2 ) const {
    nlohmann::json j = *this;
    std::ofstream  ofs( filename );
    if ( ofs ) {
      ofs << j.dump( indent );
      LOG_MESSAGE( "Resultado salvo em: " << filename );
    } else {
      LOG_ERR( "Erro ao salvar arquivo JSON em: " << filename );
    }
  }
};

inline graph_summary create_graph_summary( std::string name, std::uint32_t n, std::uint64_t m ) {
  return graph_summary{ .graph_name   = std::move( name ),
                        .vertex_count = n,
                        .edge_count   = m,
                        .density      = graph_summary::compute_density( n, m ) };
}

int main( int argc, char *argv[] ) {
  CLI::App app{
    "Algoritmo genético de chave aleatória enviesada para o problema da dominação {3}-romana"
  };
  argv = app.ensure_utf8( argv );

  std::string input_file_path = "default.txt";
  app.add_option( "-f,--file", input_file_path, "Arquivo de arestas (edges.txt)" )
    ->required()
    ->check( CLI::ExistingFile );

  unsigned int population_size = DEFAULT_POPULATION_SIZE;
  app.add_option( "-p,--pop-size", population_size, "Tamanho da população (>= 2)" )
    ->check( CLI::Range( 2U, std::numeric_limits<unsigned>::max() ) );

  double elite_fraction = DEFAULT_ELITE_FRACTION;
  app
    .add_option(
      "--elite-fraction", elite_fraction, "Fração da população que pertence à elite em [0,1]" )
    ->check( CLI::Range( 0.0, 1.0 ) );

  double mutant_fraction = DEFAULT_MUTANT_FRACTION;
  app
    .add_option( "--mutants-fraction", mutant_fraction, "Fração substituída por mutantes em [0,1]" )
    ->check( CLI::Range( 0.0, 1.0 ) );

  double elite_inheritance_prob = DEFAULT_ELITE_INHERIT_PROB;
  app
    .add_option( "--elite-inheritance-prob",
                 elite_inheritance_prob,
                 "Probabilidade de herdar o alelo do pai elite em [0,1]" )
    ->check( CLI::Range( 0.0, 1.0 ) );

  unsigned num_populations = DEFAULT_NUM_POPULATIONS;
  app
    .add_option( "--num-populations", num_populations, "Número de populações independentes (>= 1)" )
    ->check( CLI::PositiveNumber );

  unsigned int num_threads = DEFAULT_NUM_THREADS;
  app.add_option( "-j,--threads", num_threads, "Número de threads (>= 1)" )
    ->check( CLI::PositiveNumber );

  unsigned int time_limit_seconds = DEFAULT_TIME_LIMIT_SECONDS;
  app.add_option( "--time-limit", time_limit_seconds, "Tempo máximo em segundos (> 0)" )
    ->check( CLI::PositiveNumber )
    ->required();

  unsigned int max_generations = DEFAULT_MAX_GENERATIONS;
  app
    .add_option(
      "--max-generations", max_generations, "Máximo de gerações (0 = desabilita; >0 para ativar)" )
    ->check( CLI::Range( 0U, std::numeric_limits<unsigned>::max() ) );

  unsigned migration_interval = DEFAULT_MIGRATION_INTERVAL;
  app
    .add_option(
      "--migration-interval", migration_interval, "Gerações entre trocas entre populações (>= 1)" )
    ->check( CLI::PositiveNumber );

  unsigned migration_size = DEFAULT_MIGRATION_SIZE;
  app
    .add_option(
      "--migration-size", migration_size, "Melhores indivíduos trocados entre populações (>= 0)" )
    ->check( CLI::Range( 0U, std::numeric_limits<unsigned>::max() ) );

  std::string output_file_path = "default.json";
  app.add_option( "-o,--output", output_file_path, "Arquivo de resultados (results.json)" )
    ->required();

  unsigned int num_trials = DEFAULT_NUM_TRIALS;
  app.add_option( "-r,--runs", num_trials, "Número de tentativas (>= 1)" )
    ->check( CLI::Range( 1U, std::numeric_limits<unsigned>::max() ) );

  uint64_t rng_seed_cli = DEFAULT_RNG_SEED;
  app.add_option( "--seed", rng_seed_cli, "Semente do RNG (0 = aleatória)" )
    ->check( CLI::Range( uint64_t{ 0 }, std::numeric_limits<uint64_t>::max() ) );

  CLI11_PARSE( app, argc, argv );

  if ( elite_fraction + mutant_fraction > 1.0 + 1e-12 ) {
    LOG_ERR( "elite-fraction + mutants-fraction não pode exceder 1.0" );
    return 2;
  }
  if ( migration_size > 0 && num_populations < 2 ) {
    LOG_ERR( "migration-size > 0 requer num-populations >= 2" );
    return 2;
  }
  if ( migration_size > population_size ) {
    LOG_ERR( "migration-size não pode exceder pop-size" );
    return 2;
  }

  const uint64_t rng_seed_to_use = ( rng_seed_cli == 0 ) ? generate_random_seed() : rng_seed_cli;

  // ---------- logs ----------
  LOG_VAR( input_file_path );
  LOG_VAR( population_size );
  LOG_VAR( elite_fraction );
  LOG_VAR( mutant_fraction );
  LOG_VAR( elite_inheritance_prob );
  LOG_VAR( num_populations );
  LOG_VAR( num_threads );
  LOG_VAR( time_limit_seconds );
  LOG_VAR( max_generations );
  LOG_VAR( migration_interval );
  LOG_VAR( migration_size );
  LOG_VAR( num_trials );
  LOG_VAR( output_file_path );
  LOG_VAR( rng_seed_to_use );

  r3dp::brkga::MTRand rng( rng_seed_to_use );

  auto [vertex_count_total, edge_list] = r3dp::core::read_graph_from_file( input_file_path );
  const auto graph = r3dp::core::build_graph_from( vertex_count_total, edge_list );

  std::string graph_name = std::filesystem::path( input_file_path ).stem().string();

  LOG_VAR( vertex_count_total );
  LOG_VAR( edge_list.size() );
  LOG_VAR( graph_name );

  run_results run_result;
  run_result.seed  = rng_seed_to_use;
  run_result.graph = create_graph_summary( graph_name, vertex_count_total, edge_list.size() );

  for ( size_t trial_idx = 0; trial_idx < num_trials; ++trial_idx ) {
    LOG_MESSAGE( "Iniciando tentativa: " << trial_idx );

    auto    &trial_result_ref = run_result.create_trial();
    unsigned generation_idx   = 0;

    trial_result_ref.start_timer();

    r3dp::brkga::R3DPDecoder                                          decoder( graph );
    r3dp::brkga::BRKGA<r3dp::brkga::R3DPDecoder, r3dp::brkga::MTRand> algorithm(
      boost::num_vertices( graph ),
      population_size,
      elite_fraction,
      mutant_fraction,
      elite_inheritance_prob,
      decoder,
      rng,
      num_populations,
      num_threads );

    while ( true ) {
      auto elapsed_time_delta =
        std::chrono::steady_clock::now() - trial_result_ref.start_time_point;

      if ( std::chrono::duration_cast<std::chrono::seconds>( elapsed_time_delta ).count() >=
           time_limit_seconds ) {
        LOG_MESSAGE( "Limite de tempo atingido." );
        break;
      }
      if ( max_generations > 0 && generation_idx >= max_generations ) {
        LOG_MESSAGE( "Limite de gerações atingido." );
        break;
      }

      algorithm.evolve();
      generation_idx++;

      double best_fitness_now = algorithm.getBestFitness();
      trial_result_ref.add_point( best_fitness_now );

      if ( best_fitness_now < trial_result_ref.best_fitness_value ) {
        trial_result_ref.best_fitness_value = best_fitness_now;
        LOG_MESSAGE( "Novo melhor fitness encontrado na geração " << generation_idx << ": "
                                                                  << best_fitness_now );
      }

      if ( migration_size > 0 && num_populations > 1 && migration_interval > 0 &&
           generation_idx % migration_interval == 0 ) {
        algorithm.exchangeElite( migration_size );
        LOG_MESSAGE( "Migração de elite executada na geração " << generation_idx );
      }
    }
  }

  run_result.save_json( output_file_path );
  return 0;
}
