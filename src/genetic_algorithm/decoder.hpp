#pragma once

#include <cstdint>
#include <vector>

namespace r3dp::ga {

  /**
   * @brief Interface para a decodificação de um cromossomo.
   *
   * O decodificador é responsável por traduzir um cromossomo (um vetor de uint8_t)
   * em uma solução para o problema e calcular sua aptidão.
   */
  class Decoder {
  public:
    virtual ~Decoder() = default;

    /**
     * @brief Decodifica um cromossomo e retorna a aptidão da solução.
     * @param chromosome O cromossomo a ser decodificado.
     * @return O valor da aptidão da solução.
     */
    virtual uint64_t decode( std::vector<uint8_t> &chromosome ) = 0;
  };

}  // namespace r3dp::ga
