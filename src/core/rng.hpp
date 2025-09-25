#pragma once

#include <algorithm>
#include <random>
#include <stdexcept>

#define DEFAULT_P_TRUE_BOOL 0.5

namespace r3dp::core {
  /**
   * @class RNG
   * @brief Um wrapper para geradores de números pseudoaleatórios.
   *
   * @tparam Engine O tipo do motor de geração de números pseudoaleatórios,
   * com `std::mt19937_64` sendo o padrão.
   */
  template <typename Engine = std::mt19937_64>
  class RNG final {
    using engine_type = Engine;

  public:
    /**
     * @brief Construtor que inicializa o gerador com uma semente de 64 bits.
     * @param seed A semente de 64 bits para inicializar o motor.
     */
    explicit RNG( uint64_t seed ) noexcept : engine_{ seed } {}

    /**
     * @brief Construtor que inicializa o gerador com uma sequência de sementes.
     * @param seq Uma sequência de sementes (std::seed_seq).
     */
    explicit RNG( std::seed_seq &seq ) noexcept : engine_{ seq } {}

    /**
     * @brief Construtor estático que cria um gerador semeado com entropia.
     *
     * Este método utiliza `std::random_device` para obter sementes de alta
     * qualidade para garantir um estado inicial imprevisível.
     *
     * @return Uma nova instância de RNG.
     */
    static RNG from_entropy() {
      std::random_device rd;
      std::seed_seq      seq{ rd(), rd(), rd(), rd() };
      return RNG{ seq };
    }

    /**
     * @brief Construtor de cópia padrão.
     */
    RNG( const RNG & ) = default;

    /**
     * @brief Operador de atribuição de cópia padrão.
     * @return Uma referência para a instância atual.
     */
    RNG &operator=( const RNG & ) = default;

    /**
     * @brief Construtor de movimento padrão.
     */
    RNG( RNG && ) noexcept = default;

    /**
     * @brief Operador de atribuição de movimento padrão.
     * @return Uma referência para a instância atual.
     */
    RNG &operator=( RNG && ) noexcept = default;

    /**
     * @brief Destrutor padrão.
     */
    ~RNG() = default;

    /**
     * @brief Resemeia o gerador com uma nova semente de 64 bits.
     * @param seed A nova semente.
     */
    void reseed( uint64_t seed ) noexcept {
      engine_.seed( seed );
    }

    /**
     * @brief Resemeia o gerador com uma nova sequência de sementes.
     * @param seq A nova sequência de sementes.
     */
    void reseed( std::seed_seq &seq ) noexcept {
      engine_.seed( seq );
    }

    /**
     * @brief Descarta um número especificado de valores do fluxo aleatório.
     * @param n O número de valores a serem descartados.
     */
    void discard( uint64_t n ) noexcept {
      engine_.discard( n );
    }

    /**
     * @brief Obtém uma referência mutável para o motor subjacente.
     * @return Uma referência para o motor.
     */
    [[nodiscard]] engine_type &engine() noexcept {
      return engine_;
    }

    /**
     * @brief Obtém uma referência constante para o motor subjacente.
     * @return Uma referência constante para o motor.
     */
    [[nodiscard]] const engine_type &engine() const noexcept {
      return engine_;
    }

    /**
     * @brief Sobrecarga do operador de chamada de função para gerar um número.
     *
     * Este operador gera um valor aleatório do tipo de retorno do motor.
     *
     * @return O próximo valor aleatório do motor.
     */
    uint64_t operator()() noexcept {
      return engine_();
    }

    /**
     * @brief Gera um número integral aleatório em todo o seu domínio.
     *
     * A distribuição é uniforme, cobrindo o intervalo
     * [numeric_limits<T>::lowest(), numeric_limits<T>::max()].
     *
     * @tparam T O tipo integral para o número aleatório.
     * @return Um número integral aleatório.
     */
    template <std::integral T>
    [[nodiscard]] T random() {
      return std::uniform_int_distribution<T>{ std::numeric_limits<T>::lowest(),
                                               std::numeric_limits<T>::max() }( engine_ );
    }

    /**
     * @brief Gera um número integral aleatório em um intervalo especificado.
     * @tparam T O tipo integral para o número aleatório.
     * @param min O limite inferior (inclusivo) do intervalo.
     * @param max O limite superior (inclusivo) do intervalo.
     * @return Um número integral aleatório no intervalo [min, max].
     * @throw std::invalid_argument se `min` for maior que `max`.
     */
    template <std::integral T>
    [[nodiscard]] T random_range( T min, T max ) {
      if ( min > max ) {
        throw std::invalid_argument{ "random_range<T>: min > max" };
      }
      return std::uniform_int_distribution<T>{ min, max }( engine_ );
    }

    /**
     * @brief Gera um número de ponto flutuante aleatório em um intervalo.
     * @tparam T O tipo de ponto flutuante.
     * @param min O limite inferior (inclusivo) do intervalo.
     * @param max O limite superior (inclusivo) do intervalo.
     * @return Um número de ponto flutuante aleatório no intervalo [min, max].
     * @throw std::invalid_argument se `min` for maior que `max`.
     */
    template <std::floating_point T>
    [[nodiscard]] T random_range( T min, T max ) {
      if ( min > max ) {
        throw std::invalid_argument{ "random_range<T>: min > max" };
      }
      return std::uniform_real_distribution<T>{ min, max }( engine_ );
    }

    /**
     * @brief Gera um número de ponto flutuante aleatório em todo o seu domínio.
     *
     * A distribuição é uniforme, cobrindo o intervalo
     * [0, 1].
     *
     * não há como cobrir toda a range de pontos flutuante
     *
     * @tparam T O tipo de ponto flutuante para o número aleatório.
     * @return Um número de ponto flutuante aleatório.
     */
    template <std::floating_point T>
    [[nodiscard]] T random() {
      return random_range<T>( T( 0 ), T( 1 ) );
    }

    /**
     * @brief Gera um valor booleano aleatório com uma probabilidade de ser
     * verdadeiro.
     * @param p_true A probabilidade (entre 0.0 e 1.0) de o resultado ser `true`.
     * @return `true` com probabilidade `p_true`, caso contrário `false`.
     * @throw std::invalid_argument se `p_true` não estiver no intervalo [0, 1].
     */
    [[nodiscard]] bool random_bool( double p_true = DEFAULT_P_TRUE_BOOL ) {
      if ( p_true < 0.0 || p_true > 1.0 ) {
        throw std::invalid_argument{ "random_bool: p nao pertence a [0,1]" };
      }
      return std::bernoulli_distribution{ p_true }( engine_ );
    }

    /**
     * @brief Gera um número de ponto flutuante com distribuição uniforme no
     * intervalo [0, 1].
     * @tparam T O tipo de ponto flutuante, padrão é `double`.
     * @return Um número aleatório no intervalo [0, 1].
     */
    template <std::floating_point T = double>
    [[nodiscard]] T uniform01() {
      return std::uniform_real_distribution<T>{ T( 0 ), T( 1 ) }( engine_ );
    }

    /**
     * @brief Gera um número com distribuição normal (gaussiana).
     * @param mean A média da distribuição.
     * @param stddev O desvio padrão da distribuição.
     * @return Um número com distribuição normal.
     * @throw std::invalid_argument se `stddev` for negativo.
     */
    [[nodiscard]] double normal( double mean = 0.0, double stddev = 1.0 ) {
      if ( stddev < 0.0 ) {
        throw std::invalid_argument{ "normal: stddev < 0" };
      }
      return std::normal_distribution<double>{ mean, stddev }( engine_ );
    }

    /**
     * @brief Gera um número com distribuição exponencial.
     * @param lambda O parâmetro de taxa ($$ \lambda $$) da distribuição.
     * @return Um número com distribuição exponencial.
     * @throw std::invalid_argument se `lambda` for menor ou igual a zero.
     */
    [[nodiscard]] double exponential( double lambda = 1.0 ) {
      if ( lambda <= 0.0 ) {
        throw std::invalid_argument{ "exponential: lambda <= 0" };
      }
      return std::exponential_distribution<double>{ lambda }( engine_ );
    }

    /**
     * @brief Gera um número com distribuição de Cauchy.
     * @param x0 O parâmetro de localização (mediana) da distribuição.
     * @param gamma O parâmetro de escala da distribuição.
     * @return Um número com distribuição de Cauchy.
     * @throw std::invalid_argument se `gamma` for menor ou igual a zero.
     */
    [[nodiscard]] double cauchy( double x0 = 0.0, double gamma = 1.0 ) {
      if ( gamma <= 0.0 ) {
        throw std::invalid_argument{ "cauchy: gamma <= 0" };
      }
      return std::cauchy_distribution<double>{ x0, gamma }( engine_ );
    }

    /**
     * @brief Embaralha a ordem dos elementos em um intervalo.
     * @tparam It O tipo de iterador.
     * @param first O iterador para o início do intervalo.
     * @param last O iterador para o final do intervalo.
     */
    template <class It>
    void shuffle( It first, It last ) {
      std::shuffle( first, last, engine_ );
    }

    /**
     * @brief Gera uma permutação aleatória de índices.
     *
     * Cria um vetor de índices de 0 a `n-1` e o embaralha.
     *
     * @param n O número de elementos na permutação.
     * @return Um `std::vector<size_t>` contendo os índices embaralhados.
     */
    [[nodiscard]] std::vector<size_t> permutation( size_t n ) {
      std::vector<size_t> idx( n );
      std::iota( idx.begin(), idx.end(), 0 );
      shuffle( idx.begin(), idx.end() );
      return idx;
    }

    /**
     * @brief Retorna um índice aleatório no intervalo [0, n-1].
     * @param n O número de elementos.
     * @return Um índice aleatório.
     * @throw std::invalid_argument se `n` for zero.
     */
    [[nodiscard]] size_t random_index( size_t n ) {
      if ( n == 0 ) {
        throw std::invalid_argument{ "random_index: n == 0" };
      }
      const auto r = random_range<uint64_t>( 0, static_cast<uint64_t>( n - 1 ) );
      return static_cast<size_t>( r );
    }

    /**
     * @brief Seleciona uma amostra aleatória de `k` índices de `n` elementos sem
     * reposição.
     * @param n O número total de elementos.
     * @param k O tamanho da amostra.
     * @return Um `std::vector<size_t>` contendo `k` índices aleatórios únicos.
     * @throw std::invalid_argument se `k` for maior que `n`.
     */
    [[nodiscard]] std::vector<size_t> sample_without_replacement( size_t n, size_t k ) {
      if ( k > n ) {
        throw std::invalid_argument{ "sample_without_replacement: k > n" };
      }
      std::vector<size_t> idx( n );
      std::iota( idx.begin(), idx.end(), 0 );
      for ( size_t i = 0; i < k; ++i ) {
        size_t j = i + random_index( n - i );
        std::swap( idx[i], idx[j] );
      }
      idx.resize( k );
      return idx;
    }

    /**
     * @brief Seleciona um índice com base em um vetor de pesos.
     * @param weights Um vetor de pesos. O índice com maior peso tem maior
     * probabilidade de ser selecionado.
     * @return O índice aleatório selecionado.
     * @throw std::invalid_argument se o vetor de pesos estiver vazio.
     */
    [[nodiscard]] size_t weighted_index( const std::vector<double> &weights ) {
      const size_t n = weights.size();
      if ( n == 0 ) {
        throw std::invalid_argument{ "weighted_index: vetor vazio" };
      }
      double sum = 0.0;
      for ( double w : weights ) {
        if ( w > 0.0 ) {
          sum += w;
        }
      }
      if ( sum <= 0.0 ) {
        return random_index( n );
      }
      double r = uniform01<double>() * sum;
      double c = 0.0;
      for ( size_t i = 0; i < n; ++i ) {
        const double w = ( weights[i] > 0.0 ? weights[i] : 0.0 );
        c += w;
        if ( r <= c ) {
          return i;
        }
      }
      return n - 1;
    }

  private:
    engine_type engine_;  ///< O motor de geração de números pseudoaleatórios.
  };
}  // namespace r3dp::core
