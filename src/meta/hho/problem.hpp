#pragma once
#include <vector>

namespace r3dp::hho {
  template <typename T>
  concept hho_problem = requires( T problem, const std::vector<double> &x ) {
                          { problem.get_dimension() } -> std::same_as<int>;
                          {
                            problem.get_lower_bounds()
                          } -> std::same_as<const std::vector<double> &>;
                          {
                            problem.get_upper_bounds()
                          } -> std::same_as<const std::vector<double> &>;
                          { problem.evaluate( x ) } -> std::same_as<double>;
                        };

}  // namespace r3dp::hho
