#include "r3dp.hpp"

#include <nlohmann/json.hpp>

int main( int argc, char *argv[] ) {
#ifdef R3DP_USE_GA
  auto ga_status = r3dp::run_ga( argc, argv );
#endif

#ifdef R3DP_USE_BRKGA
  auto brkga_status = r3dp::run_brkga( argc, argv );
#endif
  return 0;
}
