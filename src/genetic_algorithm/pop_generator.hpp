#pragma once

#include "../core/graph.hpp"
#include "../core/rng.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace r3dp::ga {

  using Heuristic =
    std::function<std::vector<uint8_t>( const r3dp::core::Graph &, r3dp::core::RNG & )>;

  std::vector<std::vector<uint8_t>>
    generate_population( const r3dp::core::Graph      &graph,
                         size_t                        population_size,
                         const std::vector<Heuristic> &heuristic_funcs,
                         r3dp::core::RNG              &rng );

  std::vector<uint8_t> h1( const r3dp::core::Graph &graph, r3dp::core::RNG &rng );
  std::vector<uint8_t> h2( const r3dp::core::Graph &graph, r3dp::core::RNG &rng );
  std::vector<uint8_t> h3( const r3dp::core::Graph &graph, r3dp::core::RNG &rng );

  // A heuristica h4 não dá garantias de retornar uma FD{3}R válida
  std::vector<uint8_t> h4( const r3dp::core::Graph &graph, r3dp::core::RNG &rng );

}  // namespace r3dp::ga
