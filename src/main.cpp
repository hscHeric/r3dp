#include "r3dp.hpp"

#include <nlohmann/json.hpp>

int main( int argc, char *argv[] ) {
#ifdef R3DP_USE_GA
  auto status = r3dp::run_ga( argc, argv );
#endif  // R3DP_USE_GA
  //
  return status;
}
