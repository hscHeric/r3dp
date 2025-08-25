#pragma once

#include "../core/graph.hpp"  // Contém a definição do tipo Graph
#include "decoder.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <numeric>
#include <vector>

namespace r3dp::ga {

  class R3DDecoder : public Decoder {
  private:
    const core::Graph &m_graph;

  public:
    // Construtor para inicializar a referência ao grafo
    R3DDecoder( const core::Graph &graph ) : m_graph( graph ) {}

    uint64_t decode( const std::vector<uint8_t> &chromosome ) const override;
  };

  uint64_t R3DDecoder::decode( const std::vector<uint8_t> &chromosome ) const {
    return 99;
  }

}  // namespace r3dp::ga
