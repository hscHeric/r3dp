#include <boost/graph/adjacency_list.hpp>
#include <cstdint>
#include <utility>

namespace r3dp::core {
  using Vertex = uint32_t;
  using Edge   = std::pair<Vertex, Vertex>;
  using Graph = boost::adjacency_list<boost::setS,  // Arestas armazenadas em um SET (grafo simples)
                                      boost::vecS,  // Vértices armazenados de 0 a n
                                      boost::undirectedS  // Grafo não dirigido
                                      >;
  // lê o arquivo da lista de arestas e devolve o número de vértices e as arestas normalizadas
  std::pair<Vertex, std::set<Edge>> read_graph_from_file( const std::string &file_path );

  // Monta o grafo a partir da função de read_graph_from_file
  Graph build_graph_from( Vertex n, const std::set<Edge> &edges );

  bool                     is_valid_fdr3( const Graph &g, const std::vector<uint8_t> &labels );
  std::vector<std::size_t> violating_vertices_fdr3( const Graph                &g,
                                                    const std::vector<uint8_t> &labels );

  /**
   * @brief Retorna o grau máximo Δ(G) do grafo (sem laços).
   *
   * Pré-condição: o grafo é simples (sem múltiplas arestas paralelas e sem laços).
   */
  size_t max_degree( const Graph &g );
}  // namespace r3dp::core
