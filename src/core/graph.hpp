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
}  // namespace r3dp::core
