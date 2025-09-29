#pragma once
#include "../../core/graph.hpp"

#include <boost/graph/detail/adjacency_list.hpp>
#include <vector>

namespace r3dp::hho {
  template <typename T>
  concept hho_problem = requires( T problem, const std::vector<double> &x ) {
                          { problem.get_dimension() } -> std::same_as<size_t>;
                          {
                            problem.get_lower_bounds()
                          } -> std::same_as<const std::vector<double> &>;
                          {
                            problem.get_upper_bounds()
                          } -> std::same_as<const std::vector<double> &>;
                          { problem.evaluate( x ) } -> std::same_as<double>;
                        };

  class r3dp_hho_problem {
  private:
    core::graph_t       g;
    std::vector<double> lo;
    std::vector<double> hi;

  public:
    explicit r3dp_hho_problem( const core::graph_t &g )
      : g( g ), lo( boost::num_vertices( g ), 0.0 ), hi( boost::num_vertices( g ), 1.0 ) {}

    [[nodiscard]] size_t get_dimension() const {
      return boost::num_vertices( g );
    }

    [[nodiscard]] const std::vector<double> &get_lower_bounds() const {
      return lo;
    }

    [[nodiscard]] const std::vector<double> &get_upper_bounds() const {
      return hi;
    }

    [[nodiscard]] static double evaluate( const std::vector<double> &x ) {
      return static_cast<double>( x.size() ) * 3;
    }
  };

  // Aqui o static_assert verifica em tempode compilação se o concept é implementado corretamente
  static_assert( hho_problem<r3dp_hho_problem>,
                 "r3dp_hho_problem não implementa corretamente o conceito 'hho_problem'." );

}  // namespace r3dp::hho
