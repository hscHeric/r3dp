#include "r3dp.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>

int main() {
  r3dp();

  std::vector<std::string> vec;
  vec.push_back("test_package");

  r3dp_print_vector(vec);
}
