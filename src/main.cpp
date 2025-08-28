#include "CLI/CLI.hpp"
#include "r3dp.hpp"

#include <iostream>

int main( int argc, char **argv ) {
#ifdef R3DP_USE_GA
  try {
    nlohmann::json result = r3dp::parse_and_run_ga( argc, argv );
    std::cout << "Resultado JSON final:\n" << std::setw( 2 ) << result << std::endl;
  } catch ( const CLI::ParseError &e ) {
    return e.get_exit_code();
  }
#else
  std::cout << "R3DP_USE_GA não está definido. O programa não será executado." << std::endl;
#endif
  return EXIT_SUCCESS;
}
