#include "../src/core/rng.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  std::cout << "Testando r3dp::core::RNG\n";

  // 1. Testando RNG com semente de entropia
  r3dp::core::RNG<> rng_entropy = r3dp::core::RNG<>::from_entropy();
  std::cout << "\nTeste 1: Geracao de 5 numeros aleatorios usando entropia\n";
  for (int i = 0; i < 5; ++i) {
    std::cout << "  Numero: " << rng_entropy() << "\n";
  }

  // 2. Testando RNG com semente fixa
  r3dp::core::RNG<> rng_fixed(12345);
  std::cout
      << "\nTeste 2: Geracao de 5 numeros aleatorios usando semente fixa\n";
  for (int i = 0; i < 5; ++i) {
    std::cout << "  Numero: " << rng_fixed() << "\n";
  }

  // 3. Testando random<T>() para diferentes tipos
  std::cout << "\nTeste 3: Geracao de valores randomicos por tipo\n";
  std::cout << "  int: " << rng_fixed.random<int>() << "\n";
  std::cout << "  float: " << rng_fixed.random<float>() << "\n";
  std::cout << "  char: " << static_cast<int>(rng_fixed.random<char>()) << "\n";
  std::cout << "  long long: " << rng_fixed.random<int64_t>() << "\n";
  std::cout << "  double: " << rng_fixed.random<double>() << "\n";

  // 4. Testando random_range(min, max) para tipos integrais
  std::cout << "\nTeste 4: Geracao de inteiros em intervalos\n";
  std::cout << "  Inteiro em [1, 100]: " << rng_fixed.random_range(1, 100)
            << "\n";
  std::cout << "  Inteiro em [-50, 50]: " << rng_fixed.random_range(-50, 50)
            << "\n";

  // 5. Testando random_range(min, max) para tipos de ponto flutuante
  std::cout << "\nTeste 5: Geracao de floats em intervalos\n";
  std::cout << std::fixed << std::setprecision(6);
  std::cout << "  Float em [0.0, 1.0]: " << rng_fixed.random_range(0.0, 1.0)
            << "\n";
  std::cout << "  Double em [10.5, 20.5]: "
            << rng_fixed.random_range(10.5, 20.5) << "\n";

  // 6. Testando random_bool()
  std::cout
      << "\nTeste 6: Geracao de booleanos com diferentes probabilidades\n";
  std::cout << "  Booleano com 50% de chance: " << std::boolalpha
            << rng_fixed.random_bool() << "\n";
  std::cout << "  Booleano com 80% de chance: " << std::boolalpha
            << rng_fixed.random_bool(0.8) << "\n";

  // 7. Testando uniform01()
  std::cout << "\nTeste 7: Geracao de uniform01() e 1000 numeros aleatorios "
               "para verificar a distribuicao\n";
  std::cout << "  Valor uniform01: " << rng_fixed.uniform01() << "\n";

  std::vector<int> counts(10, 0); // 10 caixas de 0.1
  for (int i = 0; i < 1000; ++i) {
    double r = rng_fixed.uniform01();
    if (r >= 0.0 && r <= 1.0) {
      int bin = static_cast<int>(r * 10.0);
      if (bin == 10) {
        bin = 9; // Correcao para 1.0
      }
      counts[bin]++;
    }
  }
  std::cout << "  Distribuicao de 1000 numeros em [0, 1]:\n";
  for (int i = 0; i < 10; ++i) {
    std::cout << "    Caixa [" << i * 0.1 << ", " << (i + 1) * 0.1
              << "]: " << counts[i] << " numeros\n";
  }

  return 0;
}
