#pragma once

#define DEBUG
#define R3DP_USE_GA

#include "core/graph.hpp"
#include "core/rng.hpp"

#include <nlohmann/json.hpp>

#ifdef R3DP_USE_GA
  #include "genetic_algorithm/genetic_algorithm.hpp"
  #include "genetic_algorithm/pop_generator.hpp"
  #include "genetic_algorithm/r3d_decoder.hpp"
#endif
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

  int run_ga( int argc, char *argv[] );
}  // namespace r3dp
