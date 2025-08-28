#include "pop_generator.hpp"

#include <algorithm>
#include <boost/graph/detail/adjacency_list.hpp>
#include <vector>

namespace r3dp::ga {
  // Lembrar de perguntar o atilio se o funcionamento da H1, só funciona se considerar que 0 é
  // unlabed
  static constexpr uint8_t UNLABELED_VERTEX = 255;

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
    std::vector<uint8_t>      labels( boost::num_vertices( graph ), UNLABELED_VERTEX );
    std::vector<core::Vertex> unvisited;

    auto [vb, ve] = boost::vertices( graph );  // Iterador para os vértices
    for ( auto it = vb; it != ve; ++it ) {
      unvisited.push_back( *it );
    }

    auto *default_rng = dynamic_cast<r3dp::core::DefaultRNG *>( &rng );
    if ( default_rng ) {
      default_rng->shuffle( unvisited.begin(), unvisited.end() );
    } else {
      throw std::runtime_error(
        "O objeto RNG fornecido não é um DefaultRNG e a reprodutibilidade não pode ser garantida." );
    }

    auto all_neighbors_labeled = [&]( core::Vertex x ) -> bool {
      auto [nb, ne] = boost::adjacent_vertices( x, graph );
      for ( auto nit = nb; nit != ne; ++nit ) {
        if ( labels[*nit] == UNLABELED_VERTEX )
          return false;
      }
      return true;
    };

    for ( auto v : unvisited ) {
      if ( labels[v] != UNLABELED_VERTEX ) {
        continue;  // quanto estava usando o set os valores eram removidos apos rotular
        // agora tenho que fazer isso para carantir a o terceiro caso
      }

      if ( boost::degree( v, graph ) == 0 ) {
        labels[v] = 2;
      } else if ( all_neighbors_labeled( v ) ) {
        labels[v] = 1;
      } else {
        labels[v] = 0;

        auto [nb, ne] = boost::adjacent_vertices( v, graph );
        if ( nb != ne ) {
          // Aqui tive que normalizar (explicar para o atilio)
          // se não fizer esse sort os resultados não são reproduziveis a partir da seed
          std::vector<core::Vertex> neighbors_vec;
          for ( auto it = nb; it != ne; ++it ) {
            neighbors_vec.push_back( *it );
          }
          std::sort( neighbors_vec.begin(), neighbors_vec.end() );

          auto first    = neighbors_vec[0];
          labels[first] = 3;
          for ( size_t i = 1; i < neighbors_vec.size(); ++i ) {
            auto n    = neighbors_vec[i];
            labels[n] = 2;
          }
        }
      }
    }

    return labels;
  }

  std::vector<uint8_t> h2( const r3dp::core::Graph &graph, r3dp::core::RNG &rng ) {
    std::vector<uint8_t>      labels( boost::num_vertices( graph ), UNLABELED_VERTEX );
    std::vector<core::Vertex> unvisited;

    auto [vb, ve] = boost::vertices( graph );  // Iterador para os vértices
    for ( auto it = vb; it != ve; ++it ) {
      unvisited.push_back( *it );
    }

    auto *default_rng = dynamic_cast<r3dp::core::DefaultRNG *>( &rng );
    if ( default_rng ) {
      default_rng->shuffle( unvisited.begin(), unvisited.end() );
    } else {
      throw std::runtime_error(
        "O objeto RNG fornecido não é um DefaultRNG e a reprodutibilidade não pode ser garantida." );
    }

    auto all_neighbors_labeled = [&]( core::Vertex x ) -> bool {
      auto [nb, ne] = boost::adjacent_vertices( x, graph );
      for ( auto nit = nb; nit != ne; ++nit ) {
        if ( labels[*nit] == UNLABELED_VERTEX )
          return false;
      }
      return true;
    };

    for ( auto v : unvisited ) {
      if ( labels[v] != UNLABELED_VERTEX ) {
        continue;
      }

      if ( all_neighbors_labeled( v ) ) {
        labels[v] = 2;
      } else {
        labels[v] = 2;

        auto [nb, ne] = boost::adjacent_vertices( v, graph );
        if ( nb != ne ) {
          // remover esses dois for depois
          // minha ideia é que os experimentos deveriam ser reproduziveis, mas como em diversas
          // partes do código estou usando parallel for da openmp, isso não vai ser possivel
          std::vector<core::Vertex> neighbors_vec;
          for ( auto it = nb; it != ne; ++it ) {
            neighbors_vec.push_back( *it );
          }

          for ( size_t i = 0; i < neighbors_vec.size(); ++i ) {
            auto n    = neighbors_vec[i];
            labels[n] = 0;
          }
        }
      }
    }

    return labels;
  }

  std::vector<uint8_t> h3( const r3dp::core::Graph &graph, r3dp::core::RNG &rng ) {
    std::vector<uint8_t>      labels( boost::num_vertices( graph ), UNLABELED_VERTEX );
    std::vector<core::Vertex> unvisited;

    auto [vb, ve] = boost::vertices( graph );  // Iterador para os vértices
    for ( auto it = vb; it != ve; ++it ) {
      unvisited.push_back( *it );
    }

    std::sort( unvisited.begin(), unvisited.end(), [&]( core::Vertex a, core::Vertex b ) {
      return boost::degree( a, graph ) > boost::degree( b, graph );
    } );

    auto all_neighbors_labeled = [&]( core::Vertex x ) -> bool {
      auto [nb, ne] = boost::adjacent_vertices( x, graph );
      for ( auto nit = nb; nit != ne; ++nit ) {
        if ( labels[*nit] == UNLABELED_VERTEX )
          return false;
      }
      return true;
    };

    for ( auto v : unvisited ) {
      if ( labels[v] != UNLABELED_VERTEX ) {
        continue;  // quanto estava usando o set os valores eram removidos apos rotular
      }

      if ( all_neighbors_labeled( v ) ) {
        labels[v] = 2;
      } else {
        labels[v] = 3;

        // rotula todos os vizinhos com 0
        auto [nb, ne] = boost::adjacent_vertices( v, graph );
        for ( ; nb != ne; ++nb ) {
          labels[*nb] = 0;
        }
      }
    }
    return labels;
  }

  std::vector<uint8_t> h4( const r3dp::core::Graph &graph, r3dp::core::RNG &rng ) {
    std::vector<uint8_t>      labels( boost::num_vertices( graph ), UNLABELED_VERTEX );
    std::vector<core::Vertex> unvisited;

    auto [vb, ve] = boost::vertices( graph );  // Iterador para os vértices
    for ( auto it = vb; it != ve; ++it ) {
      unvisited.push_back( *it );
    }

    auto *default_rng = dynamic_cast<r3dp::core::DefaultRNG *>( &rng );
    if ( default_rng ) {
      default_rng->shuffle( unvisited.begin(), unvisited.end() );
    } else {
      throw std::runtime_error(
        "O objeto RNG fornecido não é um DefaultRNG e a reprodutibilidade não pode ser garantida." );
    }

    auto all_neighbors_labeled = [&]( core::Vertex x ) -> bool {
      auto [nb, ne] = boost::adjacent_vertices( x, graph );
      for ( auto nit = nb; nit != ne; ++nit ) {
        if ( labels[*nit] == UNLABELED_VERTEX )
          return false;
      }
      return true;
    };

    for ( auto v : unvisited ) {
      if ( labels[v] != UNLABELED_VERTEX ) {
        continue;
      }

      if ( all_neighbors_labeled( v ) ) {
        labels[v] = 1;
      } else {
        labels[v] = 0;

        auto [nb, ne] = boost::adjacent_vertices( v, graph );
        if ( nb != ne ) {
          std::vector<core::Vertex> neighbors_vec;
          for ( auto it = nb; it != ne; ++it ) {
            neighbors_vec.push_back( *it );
          }

          for ( size_t i = 0; i < neighbors_vec.size(); ++i ) {
            auto n    = neighbors_vec[i];
            labels[n] = 1;
          }
        }
      }
    }

    return labels;
  }
}  // namespace r3dp::ga
