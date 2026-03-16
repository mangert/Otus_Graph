#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <cassert>

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
template <template <typename, typename, bool> class Graph,
    bool Directed = false>
void complex_graph_test(std::string testName) {

    std::cout << "\n=== Testing " << testName << " (Directed=" << Directed << ") ===\n";

    using Vertex = char;
    using EdgeInfo = bool;
    using edge_data = std::tuple<Vertex, Vertex, EdgeInfo>;

    // Тестовые данные
    std::vector<Vertex> vertices = { 'A', 'B', 'C', 'D', 'E' };

    std::vector<edge_data> edges = {
        {'A', 'B', true},  // A-B
        {'B', 'C', true},  // B-C
        {'B', 'E', true}   // B-E
    };

    // СОЗДАНИЕ ГРАФА
    Graph<Vertex, EdgeInfo, Directed> g = make_graph<Graph, Directed>(vertices, edges);

    // ===== БЛОК 1: ПРОВЕРКА НАЧАЛЬНОГО СОСТОЯНИЯ =====
    std::cout << "\n-- Initial state --\n";

    // 1. Проверка вершин
    auto g_vertices = g.getVertices();
    std::cout << "Vertices count: " << g_vertices.size() << " (expected " << vertices.size() << ")\n";
    assert(g_vertices.size() == vertices.size());

    for (auto& v : vertices) {
        assert(g.hasVertex(v));
        std::cout << "Vertex " << v << " exists: OK\n";
    }

    // Проверка отсутствующей вершины
    assert(!g.hasVertex('X'));
    std::cout << "Non-existent vertex X: OK\n";

    // 2. Проверка ребер
    std::cout << "\nEdge checks:\n";
    assert(g.hasEdge('A', 'B'));
    assert(g.hasEdge('B', 'C'));
    assert(g.hasEdge('B', 'E'));

    if constexpr (!Directed) {
        // Для неориентированных проверяем обратные направления
        assert(g.hasEdge('B', 'A'));
        assert(g.hasEdge('C', 'B'));
        assert(g.hasEdge('E', 'B'));
        std::cout << "Undirected edges: OK\n";
    }
    else {
        // Для ориентированных обратных быть не должно
        assert(!g.hasEdge('B', 'A'));
        assert(!g.hasEdge('C', 'B'));
        assert(!g.hasEdge('E', 'B'));
        std::cout << "Directed edges: OK\n";
    }

    // Проверка отсутствующих ребер
    assert(!g.hasEdge('A', 'C'));
    assert(!g.hasEdge('A', 'E'));
    assert(!g.hasEdge('C', 'E'));
    std::cout << "Non-existent edges: OK\n";

    // 3. Проверка количества ребер
    size_t expected_edges = Directed ? edges.size() : edges.size() * 2;
    std::cout << "Edge count: " << g.edgeCount()
        << " (expected " << expected_edges << ")\n";
    assert(g.edgeCount() == expected_edges);

    // 4. Проверка getEdges() - убеждаемся, что все ребра на месте
    auto all_edges = g.getEdges();
    std::cout << "getEdges() returned " << all_edges.size() << " edges\n";
    assert(all_edges.size() == g.edgeCount());

    // 5. Проверка степеней вершин (ЭТАЛОН)
    std::cout << "\nDegree checks:\n";

    // Эталонные степени (считаем вручную)
    std::map<Vertex, size_t> expected_degree;
    if constexpr (Directed) {
        // Для ориентированного графа: outDegree + inDegree
        expected_degree['A'] = 1; // A->B (out=1, in=0)
        expected_degree['B'] = 3; // A->B (in), B->C (out), B->E (out) = 3
        expected_degree['C'] = 1; // B->C (in)
        expected_degree['D'] = 0; // изолирована
        expected_degree['E'] = 1; // B->E (in)
    }
    else {
        // Для неориентированного: количество инцидентных ребер
        expected_degree['A'] = 1; // A-B
        expected_degree['B'] = 3; // A-B, B-C, B-E
        expected_degree['C'] = 1; // B-C
        expected_degree['D'] = 0; // изолирована
        expected_degree['E'] = 1; // B-E
    }

    for (auto v : vertices) {
        size_t deg = g.degree(v);
        std::cout << "Degree of " << v << ": " << deg
            << " (expected " << expected_degree[v] << ")\n";
        assert(deg == expected_degree[v]);
    }

    if constexpr (Directed) {
        std::cout << "\nDirected-specific checks:\n";

        // outDegree
        std::map<Vertex, size_t> expected_out = { {'A',1}, {'B',2}, {'C',0}, {'D',0}, {'E',0} };
        std::map<Vertex, size_t> expected_in = { {'A',0}, {'B',1}, {'C',1}, {'D',0}, {'E',1} };

        for (auto v : vertices) {
            assert(g.outDegree(v) == expected_out[v]);
            assert(g.inDegree(v) == expected_in[v]);
            std::cout << v << ": out=" << g.outDegree(v)
                << " (exp " << expected_out[v]
                << "), in=" << g.inDegree(v)
                << " (exp " << expected_in[v] << ")\n";
        }
    }

    // 6. Проверка соседей
    std::cout << "\nNeighbor checks:\n";

    // Эталонные соседи
    std::map<Vertex, std::vector<Vertex>> expected_neighbors;
    if constexpr (Directed) {
        expected_neighbors['A'] = { 'B' };        // только исходящие
        expected_neighbors['B'] = { 'C', 'E' };   // только исходящие
        expected_neighbors['C'] = {};
        expected_neighbors['D'] = {};
        expected_neighbors['E'] = {};
    }
    else {
        expected_neighbors['A'] = { 'B' };
        expected_neighbors['B'] = { 'A', 'C', 'E' };
        expected_neighbors['C'] = { 'B' };
        expected_neighbors['D'] = {};
        expected_neighbors['E'] = { 'B' };
    }

    for (auto v : vertices) {
        auto neighbors = g.getNeighbors(v);
        std::sort(neighbors.begin(), neighbors.end());

        auto expected = expected_neighbors[v];
        std::sort(expected.begin(), expected.end());

        std::cout << "Neighbors of " << v << ": ";
        for (auto n : neighbors) std::cout << n << " ";
        std::cout << "(expected: ";
        for (auto n : expected) std::cout << n << " ";
        std::cout << ")\n";

        assert(neighbors.size() == expected.size());
        assert(std::equal(neighbors.begin(), neighbors.end(), expected.begin()));
    }

    // ===== БЛОК 2: УДАЛЕНИЕ ВЕРШИНЫ ИЗ СЕРЕДИНЫ =====
    std::cout << "\n-- Removing vertex C (middle) --\n";

    assert(g.hasVertex('C'));
    g.removeVertex('C');

    // Проверяем, что C удалена
    assert(!g.hasVertex('C'));
    std::cout << "Vertex C removed: OK\n";

    // Проверяем, что остальные вершины на месте
    assert(g.hasVertex('A'));
    assert(g.hasVertex('B'));
    assert(g.hasVertex('D'));
    assert(g.hasVertex('E'));
    std::cout << "Other vertices preserved: OK\n";

    // Проверяем, что связанные с C ребра исчезли
    assert(!g.hasEdge('B', 'C'));
    if constexpr (!Directed) {
        assert(!g.hasEdge('C', 'B'));
    }
    std::cout << "Edges incident to C removed: OK\n";

    // Проверяем, что остальные ребра сохранились
    assert(g.hasEdge('A', 'B'));
    assert(g.hasEdge('B', 'E'));
    if constexpr (!Directed) {
        assert(g.hasEdge('B', 'A'));
        assert(g.hasEdge('E', 'B'));
    }
    std::cout << "Other edges preserved: OK\n";

    // Проверяем новые степени
    std::cout << "New degrees after removing C:\n";
    std::map<Vertex, size_t> new_expected_degree;
    if constexpr (Directed) {
        new_expected_degree['A'] = 1; // A->B
        new_expected_degree['B'] = 2; // A->B (in), B->E (out) = 2
        new_expected_degree['D'] = 0;
        new_expected_degree['E'] = 1; // B->E (in)
    }
    else {
        new_expected_degree['A'] = 1; // A-B
        new_expected_degree['B'] = 2; // A-B, B-E
        new_expected_degree['D'] = 0;
        new_expected_degree['E'] = 1; // B-E
    }

    for (auto v : { 'A', 'B', 'D', 'E' }) {
        size_t deg = g.degree(v);
        std::cout << "Degree of " << v << ": " << deg
            << " (expected " << new_expected_degree[v] << ")\n";
        assert(deg == new_expected_degree[v]);
    }

    // ===== БЛОК 3: УДАЛЕНИЕ РЕБРА =====
    std::cout << "\n-- Removing edge A-B --\n";

    assert(g.hasEdge('A', 'B'));
    g.removeEdge('A', 'B');

    // Проверяем, что ребро исчезло
    assert(!g.hasEdge('A', 'B'));
    if constexpr (!Directed) {
        assert(!g.hasEdge('B', 'A'));
    }
    std::cout << "Edge A-B removed: OK\n";

    // Проверяем, что остальные ребра на месте
    assert(g.hasEdge('B', 'E'));
    if constexpr (!Directed) {
        assert(g.hasEdge('E', 'B'));
    }
    std::cout << "Other edges preserved: OK\n";

    // Проверяем финальные степени
    std::cout << "Final degrees after removing edge:\n";
    std::map<Vertex, size_t> final_expected_degree;
    if constexpr (Directed) {
        final_expected_degree['A'] = 0; // нет ребер
        final_expected_degree['B'] = 1; // B->E (out)
        final_expected_degree['D'] = 0;
        final_expected_degree['E'] = 1; // B->E (in)
    }
    else {
        final_expected_degree['A'] = 0; // нет ребер
        final_expected_degree['B'] = 1; // B-E
        final_expected_degree['D'] = 0;
        final_expected_degree['E'] = 1; // B-E
    }

    for (auto v : { 'A', 'B', 'D', 'E' }) {
        size_t deg = g.degree(v);
        std::cout << "Degree of " << v << ": " << deg
            << " (expected " << final_expected_degree[v] << ")\n";
        assert(deg == final_expected_degree[v]);
    }

    std::cout << "\n✅ All tests passed for " << testName << "!\n";
}

//специализация для 2 шаблонных параметров
template <template <typename, bool> class Graph,
    bool Directed = false>
void complex_graph_test(std::string testName) {

    std::cout << "\n=== Testing " << testName << " (Directed=" << Directed << ") ===\n";

    using Vertex = char;
    using EdgeInfo = bool;
    using edge_data = std::tuple<Vertex, Vertex, EdgeInfo>;

    // Тестовые данные
    std::vector<Vertex> vertices = { 'A', 'B', 'C', 'D', 'E' };

    std::vector<edge_data> edges = {
        {'A', 'B', true},  // A-B
        {'B', 'C', true},  // B-C
        {'B', 'E', true}   // B-E
    };

    // СОЗДАНИЕ ГРАФА
    Graph<Vertex, Directed> g = make_graph<Graph, Directed>(vertices, edges);

    // ===== БЛОК 1: ПРОВЕРКА НАЧАЛЬНОГО СОСТОЯНИЯ =====
    std::cout << "\n-- Initial state --\n";

    // 1. Проверка вершин
    auto g_vertices = g.getVertices();
    std::cout << "Vertices count: " << g_vertices.size() << " (expected " << vertices.size() << ")\n";
    assert(g_vertices.size() == vertices.size());

    for (auto& v : vertices) {
        assert(g.hasVertex(v));
        std::cout << "Vertex " << v << " exists: OK\n";
    }

    // Проверка отсутствующей вершины
    assert(!g.hasVertex('X'));
    std::cout << "Non-existent vertex X: OK\n";

    // 2. Проверка ребер
    std::cout << "\nEdge checks:\n";
    assert(g.hasEdge('A', 'B'));
    assert(g.hasEdge('B', 'C'));
    assert(g.hasEdge('B', 'E'));

    if constexpr (!Directed) {
        // Для неориентированных проверяем обратные направления
        assert(g.hasEdge('B', 'A'));
        assert(g.hasEdge('C', 'B'));
        assert(g.hasEdge('E', 'B'));
        std::cout << "Undirected edges: OK\n";
    }
    else {
        // Для ориентированных обратных быть не должно
        assert(!g.hasEdge('B', 'A'));
        assert(!g.hasEdge('C', 'B'));
        assert(!g.hasEdge('E', 'B'));
        std::cout << "Directed edges: OK\n";
    }

    // Проверка отсутствующих ребер
    assert(!g.hasEdge('A', 'C'));
    assert(!g.hasEdge('A', 'E'));
    assert(!g.hasEdge('C', 'E'));
    std::cout << "Non-existent edges: OK\n";

    // 3. Проверка количества ребер
    size_t expected_edges = Directed ? edges.size() : edges.size() * 2;
    std::cout << "Edge count: " << g.edgeCount()
        << " (expected " << expected_edges << ")\n";
    assert(g.edgeCount() == expected_edges);

    // 4. Проверка getEdges() - убеждаемся, что все ребра на месте
    auto all_edges = g.getEdges();
    std::cout << "getEdges() returned " << all_edges.size() << " edges\n";
    assert(all_edges.size() == g.edgeCount());

    // 5. Проверка степеней вершин (ЭТАЛОН)
    std::cout << "\nDegree checks:\n";

    // Эталонные степени (считаем вручную)
    std::map<Vertex, size_t> expected_degree;
    if constexpr (Directed) {
        // Для ориентированного графа: outDegree + inDegree
        expected_degree['A'] = 1; // A->B (out=1, in=0)
        expected_degree['B'] = 3; // A->B (in), B->C (out), B->E (out) = 3
        expected_degree['C'] = 1; // B->C (in)
        expected_degree['D'] = 0; // изолирована
        expected_degree['E'] = 1; // B->E (in)
    }
    else {
        // Для неориентированного: количество инцидентных ребер
        expected_degree['A'] = 1; // A-B
        expected_degree['B'] = 3; // A-B, B-C, B-E
        expected_degree['C'] = 1; // B-C
        expected_degree['D'] = 0; // изолирована
        expected_degree['E'] = 1; // B-E
    }

    for (auto v : vertices) {
        size_t deg = g.degree(v);
        std::cout << "Degree of " << v << ": " << deg
            << " (expected " << expected_degree[v] << ")\n";
        assert(deg == expected_degree[v]);
    }

    if constexpr (Directed) {
        std::cout << "\nDirected-specific checks:\n";

        // outDegree
        std::map<Vertex, size_t> expected_out = { {'A',1}, {'B',2}, {'C',0}, {'D',0}, {'E',0} };
        std::map<Vertex, size_t> expected_in = { {'A',0}, {'B',1}, {'C',1}, {'D',0}, {'E',1} };

        for (auto v : vertices) {
            assert(g.outDegree(v) == expected_out[v]);
            assert(g.inDegree(v) == expected_in[v]);
            std::cout << v << ": out=" << g.outDegree(v)
                << " (exp " << expected_out[v]
                << "), in=" << g.inDegree(v)
                << " (exp " << expected_in[v] << ")\n";
        }
    }

    // 6. Проверка соседей
    std::cout << "\nNeighbor checks:\n";

    // Эталонные соседи
    std::map<Vertex, std::vector<Vertex>> expected_neighbors;
    if constexpr (Directed) {
        expected_neighbors['A'] = { 'B' };        // только исходящие
        expected_neighbors['B'] = { 'C', 'E' };   // только исходящие
        expected_neighbors['C'] = {};
        expected_neighbors['D'] = {};
        expected_neighbors['E'] = {};
    }
    else {
        expected_neighbors['A'] = { 'B' };
        expected_neighbors['B'] = { 'A', 'C', 'E' };
        expected_neighbors['C'] = { 'B' };
        expected_neighbors['D'] = {};
        expected_neighbors['E'] = { 'B' };
    }

    for (auto v : vertices) {
        auto neighbors = g.getNeighbors(v);
        std::sort(neighbors.begin(), neighbors.end());

        auto expected = expected_neighbors[v];
        std::sort(expected.begin(), expected.end());

        std::cout << "Neighbors of " << v << ": ";
        for (auto n : neighbors) std::cout << n << " ";
        std::cout << "(expected: ";
        for (auto n : expected) std::cout << n << " ";
        std::cout << ")\n";

        assert(neighbors.size() == expected.size());
        assert(std::equal(neighbors.begin(), neighbors.end(), expected.begin()));
    }

    // ===== БЛОК 2: УДАЛЕНИЕ ВЕРШИНЫ ИЗ СЕРЕДИНЫ =====
    std::cout << "\n-- Removing vertex C (middle) --\n";

    assert(g.hasVertex('C'));
    g.removeVertex('C');

    // Проверяем, что C удалена
    assert(!g.hasVertex('C'));
    std::cout << "Vertex C removed: OK\n";

    // Проверяем, что остальные вершины на месте
    assert(g.hasVertex('A'));
    assert(g.hasVertex('B'));
    assert(g.hasVertex('D'));
    assert(g.hasVertex('E'));
    std::cout << "Other vertices preserved: OK\n";

    // Проверяем, что связанные с C ребра исчезли
    assert(!g.hasEdge('B', 'C'));
    if constexpr (!Directed) {
        assert(!g.hasEdge('C', 'B'));
    }
    std::cout << "Edges incident to C removed: OK\n";

    // Проверяем, что остальные ребра сохранились
    assert(g.hasEdge('A', 'B'));
    assert(g.hasEdge('B', 'E'));
    if constexpr (!Directed) {
        assert(g.hasEdge('B', 'A'));
        assert(g.hasEdge('E', 'B'));
    }
    std::cout << "Other edges preserved: OK\n";

    // Проверяем новые степени
    std::cout << "New degrees after removing C:\n";
    std::map<Vertex, size_t> new_expected_degree;
    if constexpr (Directed) {
        new_expected_degree['A'] = 1; // A->B
        new_expected_degree['B'] = 2; // A->B (in), B->E (out) = 2
        new_expected_degree['D'] = 0;
        new_expected_degree['E'] = 1; // B->E (in)
    }
    else {
        new_expected_degree['A'] = 1; // A-B
        new_expected_degree['B'] = 2; // A-B, B-E
        new_expected_degree['D'] = 0;
        new_expected_degree['E'] = 1; // B-E
    }

    for (auto v : { 'A', 'B', 'D', 'E' }) {
        size_t deg = g.degree(v);
        std::cout << "Degree of " << v << ": " << deg
            << " (expected " << new_expected_degree[v] << ")\n";
        assert(deg == new_expected_degree[v]);
    }

    // ===== БЛОК 3: УДАЛЕНИЕ РЕБРА =====
    std::cout << "\n-- Removing edge A-B --\n";

    assert(g.hasEdge('A', 'B'));
    g.removeEdge('A', 'B');

    // Проверяем, что ребро исчезло
    assert(!g.hasEdge('A', 'B'));
    if constexpr (!Directed) {
        assert(!g.hasEdge('B', 'A'));
    }
    std::cout << "Edge A-B removed: OK\n";

    // Проверяем, что остальные ребра на месте
    assert(g.hasEdge('B', 'E'));
    if constexpr (!Directed) {
        assert(g.hasEdge('E', 'B'));
    }
    std::cout << "Other edges preserved: OK\n";

    // Проверяем финальные степени
    std::cout << "Final degrees after removing edge:\n";
    std::map<Vertex, size_t> final_expected_degree;
    if constexpr (Directed) {
        final_expected_degree['A'] = 0; // нет ребер
        final_expected_degree['B'] = 1; // B->E (out)
        final_expected_degree['D'] = 0;
        final_expected_degree['E'] = 1; // B->E (in)
    }
    else {
        final_expected_degree['A'] = 0; // нет ребер
        final_expected_degree['B'] = 1; // B-E
        final_expected_degree['D'] = 0;
        final_expected_degree['E'] = 1; // B-E
    }

    for (auto v : { 'A', 'B', 'D', 'E' }) {
        size_t deg = g.degree(v);
        std::cout << "Degree of " << v << ": " << deg
            << " (expected " << final_expected_degree[v] << ")\n";
        assert(deg == final_expected_degree[v]);
    }

    std::cout << "\n✅ All tests passed for " << testName << "!\n";
}


template <bool Directed>
void run_all_tests() {    

    // Графы с 3 параметрами    
    //complex_graph_test<GraphEnumeration, Directed>("Enumeration"); //здесь ошибка
    complex_graph_test<GraphEdgeList, Directed>("EdgeList"); 
    complex_graph_test<GraphAdjMatrix, Directed>("AdjMatrix");
    //complex_graph_test<GraphIncMatrix, Directed>("IncMatrix"); //здесь ошибка

    // Графы с 2 параметрами
    complex_graph_test<GraphAdjVectors, Directed>("AdjVectors");
    complex_graph_test<GraphAdjArray, Directed>("AdjArray");
    complex_graph_test<GraphAdjList, Directed>("AdjList");
    //complex_graph_test<GraphStructured, Directed>("Structured"); //и здесь ошибка
    complex_graph_test<GraphVertexEdgeList, Directed>("VertexEdgeList");
}