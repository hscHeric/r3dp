#include "pop_generator.hpp"

#include <boost/graph/detail/adjacency_list.hpp>
#include <vector>

namespace r3dp::ga {
  // Lembrar de perguntar o atilio se o funcionamento da H1, só funciona se considerar que 0 é
  // unlabed
  static constexpr uint8_t UNLABELED_VERTEX = 0;

  std::vector<std::vector<uint8_t>>
    generate_population( const r3dp::core::Graph      &graph,
                         size_t                        population_size,
                         const std::vector<Heuristic> &heuristic_funcs,
                         r3dp::core::RNG              &rng ) {
    if ( heuristic_funcs.empty() ) {
      throw std::invalid_argument( "O vetor de funções heurísticas não pode estar vazio." );
    }

    std::vector<std::vector<uint8_t>> population( population_size );
    std::size_t                       num_heuristics = heuristic_funcs.size();

#pragma omp parallel for
    for ( std::size_t i = 0; i < population_size; ++i ) {
      const auto &heuristic_func = heuristic_funcs[i % num_heuristics];
      population[i]              = heuristic_func( graph, rng );
    }

    return population;
  }

  std::vector<uint8_t> h1( const r3dp::core::Graph &graph, r3dp::core::RNG &rng ) {
    std::set<core::Vertex> unvisited;
    std::vector<uint8_t>   labels( boost::num_vertices( graph ), UNLABELED_VERTEX );
    auto [vb, ve] = boost::vertices( graph );
    for ( auto it = vb; it != ve; ++it ) {
      unvisited.insert( *it );
    }

    auto all_neighbors_labeled = [&]( core::Vertex x ) -> bool {
      auto [nb, ne] = boost::adjacent_vertices( x, graph );
      for ( auto nit = nb; nit != ne; ++nit ) {
        if ( labels[*nit] == UNLABELED_VERTEX )
          return false;
      }
      return true;
    };

    while ( !unvisited.empty() ) {
      unsigned int random_index = rng.randInt( unvisited.size() - 1 );
      auto         it           = std::next( unvisited.begin(), random_index );
      core::Vertex v            = *it;

      if ( boost::degree( v, graph ) == 0 ) {
        labels[v] = 2;
        unvisited.erase( v );
      } else if ( all_neighbors_labeled( v ) ) {
        labels[v] = 1;
        unvisited.erase( v );
      } else {
        labels[v] = 3;

        // percorre todos os vizinhos de v
        auto [nb, ne] = boost::adjacent_vertices( v, graph );
        for ( auto nit = nb; nit != ne; ++nit ) {
          core::Vertex n = *nit;
          if ( labels[n] == UNLABELED_VERTEX ) {
            labels[n] = 0;
            unvisited.erase( n );
          }
        }
        unvisited.erase( v );
      }
    }

    return labels;
  }
}  // namespace r3dp::ga
