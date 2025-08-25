#pragma once

#include <random>

namespace r3dp::core {

  class RNG {
  public:
    virtual ~RNG()                                 = default;
    virtual double       rand()                    = 0;
    virtual unsigned int randInt( unsigned int n ) = 0;
  };

  class DefaultRNG : public RNG {
  public:
    double rand() override {
      return static_cast<double>( gen() ) / gen.max();
    }

    unsigned int randInt( unsigned int n ) override {
      return std::uniform_int_distribution<>( 0, n )( gen );
    }

  private:
    mutable std::mt19937 gen{ std::random_device{}() };
  };

}  // namespace r3dp::core
