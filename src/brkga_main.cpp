#include <CLI/CLI.hpp>
#include <cstdint>
#include <random>

static uint64_t generate_random_seed() {
  std::random_device                      rd;
  std::mt19937_64                         gen( rd() );
  std::uniform_int_distribution<uint64_t> dist;

  return dist( gen );
}

constexpr double   DEFAULT_POPULATION_SIZE     = 5.0;
constexpr double   DEFAULT_ELITE_FRACTION      = 0.20;
constexpr double   DEFAULT_MUTANT_FRACTION     = 0.057;
constexpr double   DEFAULT_ELITE_INHERIT_PROB  = 0.70;
constexpr unsigned DEFAULT_NUM_POPULATIONS     = 3;
constexpr unsigned DEFAULT_NUM_THREADS         = 2;
constexpr unsigned DEFAULT_EXCHANGE_INTERVAL   = 100;
constexpr unsigned DEFAULT_EXCHANGE_BEST_COUNT = 2;
constexpr unsigned DEFAULT_MAX_GENERATIONS     = 586;
constexpr unsigned DEFAULT_TIME_LIMIT_SECONDS  = 0;
constexpr uint64_t DEFAULT_RNG_SEED            = 0;  // 0 = seed aleatória
constexpr bool     DEFAULT_USE_RANDOM_SEED     = true;
const std::string  DEFAULT_OUTPUT_FILE         = "results.json";
constexpr unsigned DEFAULT_NUM_TRIALS          = 1;

int main( int argc, char *argv[] ) {
  CLI::App app{
    "Algoritmo genético de chave aleatória enviesada para o problema da dominação {3}-romana "
  };
  argv = app.ensure_utf8( argv );

  double      population      = DEFAULT_POPULATION_SIZE;
  double      elite_fraction  = DEFAULT_ELITE_FRACTION;
  double      mutant_fraction = DEFAULT_MUTANT_FRACTION;
  double      inherit_prob    = DEFAULT_ELITE_INHERIT_PROB;
  unsigned    populations     = DEFAULT_NUM_POPULATIONS;
  unsigned    threads         = DEFAULT_NUM_THREADS;
  unsigned    exchange_intvl  = DEFAULT_EXCHANGE_INTERVAL;
  unsigned    exchange_count  = DEFAULT_EXCHANGE_BEST_COUNT;
  unsigned    max_generations = DEFAULT_MAX_GENERATIONS;
  unsigned    time_limit_sec  = DEFAULT_TIME_LIMIT_SECONDS;
  uint64_t    rng_seed        = DEFAULT_RNG_SEED;
  bool        random_seed     = DEFAULT_USE_RANDOM_SEED;
  std::string output_file     = "results.json";
  unsigned    trials          = DEFAULT_NUM_TRIALS;

  std::cout << output_file << "\n";
}
