#include <iostream>
#include <vector>

#include "IGraph.h"
#include "GraphEnumeration.h"
#include "GraphAdjMatrix.h"
#include "GraphIncMatrix.h"
#include "GraphEdgeList.h"
#include "GraphAdjVectors.h"
#include "GraphAdjArray.h"
#include "GraphAdjList.h"
#include "GraphStructured.h"
#include "GraphVertexEdgeList.h"

#include "graph_factory.h"

using namespace graph;

//специализация для 3 шаблонных параметров
template <template <typename, typename, bool> class Graph>
void bipartiteGraph(const std::string& gName) {
    std::cout << "\n========== " << gName << " ==========\n";

    using Vertex = int;
    using EdgeInfo = bool;
    using edge_data = std::tuple<Vertex, Vertex, EdgeInfo>;

    // Данные двудольного графа А(3,4) с 5 рёбрами
    std::vector<Vertex> vertices = { 1, 2, 3, 4, 5, 6, 7 };

    std::vector<edge_data> edges = {
        {1, 2, true},  // ребро между долями
        {2, 3, true},  // ребро между долями
        {3, 4, true},  // ребро между долями
        {4, 5, true},  // ребро между долями
        {5, 6, true}   // ребро между долями
        // вершина 7 изолирована
    };

    // Создаём граф (неориентированный)
    Graph<Vertex, EdgeInfo, false> g =
        graph::make_graph<Graph, false>(vertices, edges);

    // Вывод информации о графе
    std::cout << "Двудольный граф А(3,4) с 5 рёбрами\n";
    std::cout << "Левая доля (нечётные): {1, 3, 5}\n";
    std::cout << "Правая доля (чётные): {2, 4, 6, 7}\n\n";

    // Вывод всех вершин
    auto g_vertices = g.getVertices();
    std::cout << "Вершины (" << g_vertices.size() << "): ";
    for (auto v : g_vertices) std::cout << v << " ";
    std::cout << "\n\n";

    // Вывод всех рёбер
    auto g_edges = g.getEdges();
    std::cout << "Рёбра (" << g.edgeCount() << "):\n";
    for (const auto& [from, to, _] : g_edges) {
        std::cout << "  " << from << " -- " << to;
        // Определяем доли
        std::string from_part = (from % 2 == 1) ? "левая" : "правая";
        std::string to_part = (to % 2 == 1) ? "левая" : "правая";
        std::cout << "  (" << from_part << " -> " << to_part << ")\n";
    }
    std::cout << "\n";

    // Степени вершин
    std::cout << "Степени вершин:\n";
    std::map<int, int> expected_degrees = {
        {1, 1}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 1}, {7, 0}
    };
    for (int v : vertices) {
        size_t deg = g.degree(v);
        std::cout << "  deg(" << v << ") = " << deg;
        if (deg == expected_degrees[v]) {
            std::cout << " +\n";
        }
        else {
            std::cout << " ! (ожидалось " << expected_degrees[v] << ")\n";
        }
    }

    // Проверка двудольности (неформальная - просто смотрим, нет ли рёбер внутри доли)
    std::cout << "\nПроверка двудольности:\n";
    bool is_bipartite = true;
    for (const auto& [from, to, _] : g_edges) {
        bool from_odd = (from % 2 == 1);
        bool to_odd = (to % 2 == 1);
        if (from_odd == to_odd) {
            std::cout << "  ! Ребро " << from << "-" << to
                << " внутри одной доли!\n";
            is_bipartite = false;
        }
    }
    if (is_bipartite) {
        std::cout << "  + Все рёбра между разными долями\n";
    }

    std::cout << "====================================\n\n";
}

//специализация для двух шаблонных параметров
template <template <typename, bool> class Graph>
void bipartiteGraph(const std::string& gName) {
    std::cout << "\n========== " << gName << " ==========\n";

    using Vertex = int;
    using EdgeInfo = bool;
    using edge_data = std::tuple<Vertex, Vertex, EdgeInfo>;

    // Данные двудольного графа А(3,4) с 5 рёбрами
    std::vector<Vertex> vertices = { 1, 2, 3, 4, 5, 6, 7 };

    std::vector<edge_data> edges = {
        {1, 2, true},  // ребро между долями
        {2, 3, true},  // ребро между долями
        {3, 4, true},  // ребро между долями
        {4, 5, true},  // ребро между долями
        {5, 6, true}   // ребро между долями
        // вершина 7 изолирована
    };

    // Создаём граф (неориентированный)
    Graph<Vertex, false> g =
        graph::make_graph<Graph, false>(vertices, edges);

    // Вывод информации о графе
    std::cout << "Двудольный граф А(3,4) с 5 рёбрами\n";
    std::cout << "Левая доля (нечётные): {1, 3, 5}\n";
    std::cout << "Правая доля (чётные): {2, 4, 6, 7}\n\n";

    // Вывод всех вершин
    auto g_vertices = g.getVertices();
    std::cout << "Вершины (" << g_vertices.size() << "): ";
    for (auto v : g_vertices) std::cout << v << " ";
    std::cout << "\n\n";

    // Вывод всех рёбер
    auto g_edges = g.getEdges();
    std::cout << "Рёбра (" << g.edgeCount() << "):\n";
    for (const auto& [from, to, _] : g_edges) {
        std::cout << "  " << from << " -- " << to;
        // Определяем доли
        std::string from_part = (from % 2 == 1) ? "левая" : "правая";
        std::string to_part = (to % 2 == 1) ? "левая" : "правая";
        std::cout << "  (" << from_part << " -> " << to_part << ")\n";
    }
    std::cout << "\n";

    // Степени вершин
    std::cout << "Степени вершин:\n";
    std::map<int, int> expected_degrees = {
        {1, 1}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 1}, {7, 0}
    };
    for (int v : vertices) {
        size_t deg = g.degree(v);
        std::cout << "  deg(" << v << ") = " << deg;
        if (deg == expected_degrees[v]) {
            std::cout << " +\n";
        }
        else {
            std::cout << " ! (ожидалось " << expected_degrees[v] << ")\n";
        }
    }

    // Проверка двудольности (неформальная - просто смотрим, нет ли рёбер внутри доли)
    std::cout << "\nПроверка двудольности:\n";
    bool is_bipartite = true;
    for (const auto& [from, to, _] : g_edges) {
        bool from_odd = (from % 2 == 1);
        bool to_odd = (to % 2 == 1);
        if (from_odd == to_odd) {
            std::cout << "  ! Ребро " << from << "-" << to
                << " внутри одной доли!\n";
            is_bipartite = false;
        }
    }
    if (is_bipartite) {
        std::cout << "  + Все рёбра между разными долями\n";
    }

    std::cout << "====================================\n\n";
}


void testAllRepresentations() {
    std::cout << "\n========== ТЕСТИРОВАНИЕ ВСЕХ ПРЕДСТАВЛЕНИЙ ДВУДОЛЬНОГО ГРАФА ==========\n";

    // Графы с 3 параметрами (Vertex, EdgeInfo, Directed)
    bipartiteGraph<graph::GraphEnumeration>("Перечисление множеств");
    bipartiteGraph<graph::GraphEdgeList>("Перечень рёбер");
    bipartiteGraph<graph::GraphAdjMatrix>("Матрица смежности");
    bipartiteGraph<graph::GraphIncMatrix>("Матрица инцидентности");

    // Графы с 2 параметрами (Vertex, Directed)
    bipartiteGraph<graph::GraphAdjVectors>("Векторы смежности");
    bipartiteGraph<graph::GraphAdjArray>("Массивы смежности");
    bipartiteGraph<graph::GraphAdjList>("Списки смежности");
    bipartiteGraph<graph::GraphStructured>("Структура с оглавлением");
    bipartiteGraph<graph::GraphVertexEdgeList>("Список вершин и список рёбер");
}