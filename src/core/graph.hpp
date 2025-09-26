#include <boost/graph/adjacency_list.hpp>
#include <cstdint>
#include <utility>

namespace r3dp::core {
  using vertex_t = uint32_t;
  using edge_t   = std::pair<vertex_t, vertex_t>;
  using graph_t =
    boost::adjacency_list<boost::setS,        // Arestas armazenadas em um SET (grafo simples)
                          boost::vecS,        // Vértices armazenados de 0 a n
                          boost::undirectedS  // Grafo não dirigido
                          >;
  // lê o arquivo da lista de arestas e devolve o número de vértices e as arestas normalizadas
  std::pair<vertex_t, std::set<edge_t>> read_graph_from_file( const std::string &file_path );

  // Monta o grafo a partir da função de read_graph_from_file
  graph_t build_graph_from( vertex_t n, const std::set<edge_t> &edges );

  bool                     is_valid_fdr3( const graph_t &g, const std::vector<uint8_t> &labels );
  std::vector<std::size_t> violating_vertices_fdr3( const graph_t              &g,
                                                    const std::vector<uint8_t> &labels );

  /**
   * @brief Retorna o grau máximo Δ(G) do grafo (sem laços).
   *
   * Pré-condição: o grafo é simples (sem múltiplas arestas paralelas e sem laços).
   */
  size_t max_degree( const graph_t &g );
}  // namespace r3dp::core
