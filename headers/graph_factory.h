#pragma once
#include "IGraph.h"
#include <vector>
#include <tuple>
#include <stdexcept>
#include <optional>
#include <concepts>

namespace graph {

    // Фабрика для создания графа (версия с исключениями)
    template <template <typename, typename, bool> class Graph,
        bool Directed = false,
        typename Vertex,
        typename EdgeInfo>
    Graph<Vertex, EdgeInfo, Directed> make_graph(
        const std::vector<Vertex>& vertices,
        const std::vector<std::tuple<Vertex, Vertex, EdgeInfo>>& edges) {
        
        Graph<Vertex, EdgeInfo, Directed> g;

        for (const auto& v : vertices) {
            if (!g.addVertex(v)) {
                throw std::invalid_argument("Duplicate vertex");
            }
        }

        for (const auto& [from, to, info] : edges) {
            if (!g.hasVertex(from) || !g.hasVertex(to)) {
                throw std::invalid_argument("Edge references non-existent vertex");
            }
            if (!g.addEdge(from, to, info)) {
                throw std::runtime_error("Failed to add edge");
            }
        }

        return g;
    }

    // Версия без исключений (optional)
    template <template <typename, typename, bool> class Graph,
        bool Directed = false,
        typename Vertex,
        typename EdgeInfo>    
        std::optional<Graph<Vertex, EdgeInfo, Directed>> try_make_graph(
            const std::vector<Vertex>& vertices,
            const std::vector<std::tuple<Vertex, Vertex, EdgeInfo>>& edges) {

        Graph<Vertex, EdgeInfo, Directed> g;

        for (const auto& v : vertices) {
            if (!g.addVertex(v)) return std::nullopt;
        }

        for (const auto& [from, to, info] : edges) {
            if (!g.hasVertex(from) || !g.hasVertex(to)) return std::nullopt;
            if (!g.addEdge(from, to, info)) return std::nullopt;
        }

        return g;
    }   

} // namespace graph