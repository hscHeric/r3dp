#pragma once

#include "../core/graph.hpp"
#include "decoder.hpp"

#include <boost/graph/detail/adjacency_list.hpp>
#include <vector>

namespace r3dp::ga {

  class R3DDecoder : public Decoder {
  private:
    const core::Graph &m_graph;

  public:
    R3DDecoder( const core::Graph &graph ) : m_graph( graph ) {}

    // Just the declaration here
    uint64_t decode( std::vector<uint8_t> &chromosome ) override;
  };
}  // namespace r3dp::ga
