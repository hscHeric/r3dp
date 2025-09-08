#pragma once

#include <string>
#define DEBUG
#define R3DP_USE_BRKGA
#define R3DP_USE_GA

#ifdef DEBUG
  #define LOG_MESSAGE( msg ) \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl;

  #define LOG_VAR( var )                                                                        \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << #var << " = " << ( var ) \
              << std::endl;

  #define LOG_VECTOR( vec )                                                            \
    std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << #vec << " = ["; \
    for ( const auto &elem : vec ) {                                                   \
      std::cout << elem << " ";                                                        \
    }                                                                                  \
    std::cout << "]" << std::endl;

  #define LOG_ERR( msg ) \
    std::cerr << "[ERRO] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl;

#else
  #define LOG_MESSAGE( msg ) \
    do {                     \
    } while ( 0 )
  #define LOG_VAR( var ) \
    do {                 \
    } while ( 0 )
  #define LOG_VECTOR( vec ) \
    do {                    \
    } while ( 0 )
  #define LOG_ERR( msg ) \
    do {                 \
    } while ( 0 )
#endif

namespace r3dp {
  namespace version {
    constexpr unsigned int       major  = 0;
    constexpr unsigned int       minor  = 1;
    constexpr unsigned int       patch  = 0;
    constexpr const char * const string = "0.1.0";
  }  // namespace version

  namespace brkga {
    constexpr unsigned int DEFAULT_POP_SIZE           = 200;
    constexpr double       DEFAULT_ELITE_FRACTION     = 0.10;
    constexpr double       DEFAULT_MUTANTS_FRACTION   = 0.10;
    constexpr double       DEFAULT_ELITE_INHERIT_PROB = 0.70;
    constexpr unsigned     DEFAULT_NUM_POPULATIONS    = 3;
    constexpr unsigned     DEFAULT_MIGRATION_INTERVAL = 100;
    constexpr unsigned     DEFAULT_MIGRATION_SIZE     = 2;
    constexpr unsigned int DEFAULT_MAX_THREADS        = 1;
    constexpr unsigned int DEFAULT_RUNS               = 1;
    const std::string      DEFAULT_INPUT_FILE         = "default.txt";
    const std::string      DEFAULT_OUTPUT_FILE        = "default.json";
  }  // namespace brkga

  int run_brkga( int argc, char **argv );
  int run_hho( int argc, char **argv );
}  // namespace r3dp
