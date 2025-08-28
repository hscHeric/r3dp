
#include "r3d_decoder.hpp"

namespace r3dp::ga {

  uint64_t R3DDecoder::decode( std::vector<uint8_t> &chromosome ) {
    const auto n = boost::num_vertices( m_graph );
    if ( chromosome.size() != n ) {
      throw std::invalid_argument( "Tamanho do chromossomo" );
    }
    for ( auto x : chromosome )
      if ( x > 3 )
        throw std::invalid_argument( "Rotulo fora de {0,1,2,3}" );

    auto sum_neighbors = [&]( core::Vertex v ) -> uint32_t {
      uint32_t s    = 0;
      auto [nb, ne] = boost::adjacent_vertices( v, m_graph );
      for ( auto it = nb; it != ne; ++it )
        s += chromosome[static_cast<std::size_t>( *it )];
      return s;
    };

    auto [vb, ve] = boost::vertices( m_graph );
    for ( auto vit = vb; vit != ve; ++vit ) {
      core::Vertex v  = *vit;
      uint8_t     &lv = chromosome[static_cast<std::size_t>( v )];

      if ( lv == 0 ) {
        uint32_t s = sum_neighbors( v );
        if ( s < 3 ) {
          if ( s == 2 )
            lv = 1;
          else
            lv = 2;
        }
      } else if ( lv == 1 ) {
        uint32_t s = sum_neighbors( v );
        if ( s < 2 ) {
          lv = 2;
        }
      }
    }

    // soma dos rótulos corrigidos
    uint64_t cost = 0;
    for ( uint8_t x : chromosome )
      cost += x;
    return cost;
  }
}  // namespace r3dp::ga
