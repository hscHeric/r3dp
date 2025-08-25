#pragma once

#include "decoder.hpp"

#include <cstdint>

namespace r3dp::ga {

  class R3DDecoder : public Decoder {
  public:
    uint64_t decode( const std::vector<uint8_t> &chromosome ) const override;
  };

  uint64_t R3DDecoder::decode( const std::vector<uint8_t> &chromosome ) const {
    return chromosome.size() * chromosome[0];
  }

}  // namespace r3dp::ga
