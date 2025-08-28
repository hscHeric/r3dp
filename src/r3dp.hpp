#pragma once

#define R3DP_USE_GA
#define DEBUG
#ifdef DEBUG
  #include <iostream>
  #define DEBUG_PRINT( msg ) std::cout << msg << std::endl;
#else
  #define DEBUG_PRINT( msg )  // Nada é impresso se DEBUG não estiver definido
#endif

#include <nlohmann/json.hpp>

namespace r3dp {
  namespace version {
    constexpr unsigned int       major  = 0;
    constexpr unsigned int       minor  = 1;
    constexpr unsigned int       patch  = 0;
    constexpr const char * const string = "0.1.0";
  }  // namespace version

  nlohmann::json parse_and_run_ga( int argc, char **argv );
}  // namespace r3dp
