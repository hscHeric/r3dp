#pragma once

#include "../../core/problem.hpp"
#include "../../core/rng.hpp"

#include <cstdint>

namespace r3dp::hho {

  template <core::problem problem_type>
  class HHO {
  public:
    using genotype_type = typename problem_type::genotype_type;

    HHO( size_t        population_size,
         uint32_t      max_iterations,
         uint32_t      max_threads,
         problem_type &problem,
         core::RNG<>  &rng );

    void step();

    [[nodiscard]] const std::vector<double> &get_best_solution() const;
    [[nodiscard]] double                     get_best_fitness() const;
    [[nodiscard]] size_t                     get_iteration() const;

    void reset();

  private:
    const size_t   population_size;
    const size_t   max_iterations;
    const uint32_t max_threads;

    problem_type &problem;
    core::RNG<>  &rng;

    const size_t        dimension;
    std::vector<double> lower_bounds;
    std::vector<double> upper_bounds;

    std::vector<std::vector<double>> hawks;
    std::vector<double>              rabbit_position;
    double                           best_fitness;
    size_t                           iteration;

    std::vector<double> levy_flight( size_t d );
    inline void         initialize_hawks();
  };
}  // namespace r3dp::hho
