#pragma once
#include "IGraph.h"
#include <vector>
#include <tuple>
#include <stdexcept>
#include <optional>
#include <concepts>

namespace graph {

    // Основная версия - для полного набора шаблонных параметров
    template <template <Comparable, typename, bool> class Graph,
        bool Directed = false,
        Comparable Vertex,
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

    //специализация для двух шаблонных параметров
    template <template <Comparable, bool> class Graph,
        bool Directed = false,
        Comparable Vertex>
    Graph<Vertex, Directed> make_graph(
        const std::vector<Vertex>& vertices,
        const std::vector<std::tuple<Vertex, Vertex, bool>>& edges) {

        Graph<Vertex, Directed> g;

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

} // namespace graph