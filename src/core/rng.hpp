#pragma once
#include <algorithm>
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
    explicit DefaultRNG( std::uint64_t seed = std::random_device{}() ) : gen( seed ) {}

    double rand() override {
      std::uniform_real_distribution<double> d( 0.0, 1.0 );
      return d( gen );
    }

    unsigned int randInt( unsigned int n ) override {
      if ( n == 0 )
        return 0;
      std::uniform_int_distribution<unsigned int> d( 0, n - 1 );
      return d( gen );
    }

    template <class It>
    void shuffle( It first, It last ) {
      std::shuffle( first, last, gen );
    }

    // acesso opcional
    std::mt19937 &engine() {
      return gen;
    }

  private:
    std::mt19937 gen;
  };

}  // namespace r3dp::core
