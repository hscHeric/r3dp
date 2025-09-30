#pragma once

#include "hho_problem.hpp"

#include <cstdint>
#include <random>
#include <vector>

namespace r3dp::hho {

  template <hho_problem problem_type>
  class HHO {
  private:
    const size_t   population_size;
    const size_t   max_iterations;
    const uint32_t max_threads;

    size_t                           iteration{};
    std::vector<std::vector<double>> hawks;
    std::vector<double>              rabbit_position;
    double                           rabbit_fitness{};

    const problem_type &ref_problem;

    std::mt19937_64 rng;
    uint32_t        seed;
    void            initialize_hawks();

  public:
    HHO( size_t              population_size,
         size_t              max_iterations,
         uint32_t            max_threads,
         const problem_type &ref_problem,
         uint32_t            seed = 0 );

    HHO( size_t              population_size,
         size_t              max_iterations,
         uint32_t            max_threads,
         const problem_type &ref_problem );

    void step();
    void step_parallel();

    [[nodiscard]] const std::vector<double> &get_best_solution() const;
    [[nodiscard]] double                     get_best_fitness() const;
    [[nodiscard]] size_t                     get_iteration() const;

    void reset();
  };

}  // namespace r3dp::hho
