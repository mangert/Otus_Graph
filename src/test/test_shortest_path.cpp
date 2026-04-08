#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
#include "algorithms/graph_algorithms_shortest_path.h"
#include "graph/GraphAdjMatrix.h"
#include "graph/graph_factory.h"

using namespace graph_algorithms;

// Вспомогательная структура для ребра с весом
using weighted_edge = std::tuple<int, int, double>;

// Структура для хранения ожидаемого результата (набор рёбер)
struct ExpectedShortestPaths {
    std::vector<std::pair<int, int>> edges;  // рёбра дерева кратчайших путей
    std::map<int, double> distances;          // ожидаемые расстояния до каждой вершины
};

// Функция для нормализации ребра (для неориентированного графа)
std::pair<int, int> normalizeEdgeShortest(int v1, int v2) {
    if (v1 < v2) return { v1, v2 };
    return { v2, v1 };
}

// Функция для вывода дерева кратчайших путей
void printShortestPaths(const std::optional<std::vector<Edge<int>>>& result,
    const std::string& name) {
    std::cout << name << ": ";
    if (!result.has_value()) {
        std::cout << "nullopt (граф несвязный, пустой, есть отрицательные веса или неверная стартовая вершина)" << std::endl;
        return;
    }

    const auto& tree = result.value();
    if (tree.empty()) {
        std::cout << "пустое дерево (1 вершина)" << std::endl;
        return;
    }

    std::cout << "[";
    for (size_t i = 0; i < tree.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "(" << tree[i].v1 << "-" << tree[i].v2 << ")";
    }
    std::cout << "]" << std::endl;
}

// Функция для проверки корректности дерева кратчайших путей
bool validateShortestPaths(const std::vector<Edge<int>>& tree,
    const ExpectedShortestPaths& expected) {
    if (tree.size() != expected.edges.size()) {
        std::cout << "      Размеры не совпадают: " << tree.size()
            << " vs " << expected.edges.size() << std::endl;
        return false;
    }

    // Нормализуем и сортируем оба набора
    std::vector<std::pair<int, int>> normalized_tree;
    for (const auto& e : tree) {
        normalized_tree.push_back(normalizeEdgeShortest(e.v1, e.v2));
    }

    auto normalized_expected = expected.edges;
    for (auto& e : normalized_expected) {
        e = normalizeEdgeShortest(e.first, e.second);
    }

    std::sort(normalized_tree.begin(), normalized_tree.end());
    std::sort(normalized_expected.begin(), normalized_expected.end());

    // Сравниваем
    for (size_t i = 0; i < normalized_tree.size(); ++i) {
        if (normalized_tree[i] != normalized_expected[i]) {
            std::cout << "      Ребро не совпадает: ("
                << normalized_tree[i].first << "," << normalized_tree[i].second
                << ") vs (" << normalized_expected[i].first << ","
                << normalized_expected[i].second << ")" << std::endl;
            return false;
        }
    }

    return true;
}

// Функция для тестирования Дейкстры на одном графе
template<bool is_directed = false>
void testDijkstra(const std::vector<int>& vertices,
    const std::vector<weighted_edge>& edges,
    int start_vertex,
    const std::string& test_name,
    const ExpectedShortestPaths& expected) {

    std::cout << "\n  " << test_name << ":" << std::endl;
    std::cout << "    Стартовая вершина: " << start_vertex << std::endl;
    std::cout << "    Тип графа: " << (is_directed ? "ориентированный" : "неориентированный") << std::endl;

    // Создаём граф с нужной направленностью
    auto graph = graph::make_graph<graph::GraphAdjMatrix, is_directed>(
        vertices, edges);

    // Запускаем Дейкстру
    auto result = dijkstra<graph::GraphAdjMatrix, int, double, is_directed>(
        graph, start_vertex);    // Выводим результат
    printShortestPaths(result, "    Дейкстра");

    // Проверяем корректность
    if (result.has_value()) {
        bool ok = validateShortestPaths(result.value(), expected);
        if (ok) {
            std::cout << "    + Дерево кратчайших путей корректно" << std::endl;
        }
        else {
            std::cout << "    ! ОШИБКА: дерево не соответствует ожидаемому!" << std::endl;
        }
    }
    else {
        std::cout << "    ! ОШИБКА: ожидался успешный результат, но получен nullopt!" << std::endl;
    }
}

// Функция для тестирования Беллмана-Форда на одном графе
template<bool is_directed = false>
void testBellmanFord(const std::vector<int>& vertices,
    const std::vector<weighted_edge>& edges,
    int start_vertex,
    const std::string& test_name,
    const ExpectedShortestPaths& expected) {

    std::cout << "\n  " << test_name << ":" << std::endl;
    std::cout << "    Стартовая вершина: " << start_vertex << std::endl;
    std::cout << "    Тип графа: " << (is_directed ? "ориентированный" : "неориентированный") << std::endl;

    // Создаём граф с нужной направленностью
    auto graph = graph::make_graph<graph::GraphAdjMatrix, is_directed>(
        vertices, edges);

    // Запускаем Алгоритм Беллмана-Форда
    auto result = bellmanFord<graph::GraphAdjMatrix, int, double, is_directed>(
        graph, start_vertex);    // Выводим результат
    printShortestPaths(result, "    Беллман Форд");

    // Проверяем корректность
    if (result.has_value()) {
        bool ok = validateShortestPaths(result.value(), expected);
        if (ok) {
            std::cout << "    + Дерево кратчайших путей корректно" << std::endl;
        }
        else {
            std::cout << "    ! ОШИБКА: дерево не соответствует ожидаемому!" << std::endl;
        }
    }
    else {
        std::cout << "    ! ОШИБКА: ожидался успешный результат, но получен nullopt!" << std::endl;
    }
}

// Общая функция для тестирования ошибочных ситуаций
template<typename Algorithm>
void testErrorCases(Algorithm algo, const std::string& algo_name) {
    std::cout << "\n  ОШИБОЧНЫЕ СИТУАЦИИ ДЛЯ " << algo_name << ":" << std::endl;

    // 1. Пустой граф
    {
        std::vector<int> vertices = {};
        std::vector<weighted_edge> edges = {};
        auto graph = graph::make_graph<graph::GraphAdjMatrix, false>(vertices, edges);
        auto result = algo(graph, 1);
        std::cout << "    Пустой граф: " << (result.has_value() ? "ошибка - должен быть nullopt" : "корректно nullopt") << std::endl;
    }

    // 2. Несуществующая стартовая вершина
    {
        std::vector<int> vertices = { 1, 2, 3 };
        std::vector<weighted_edge> edges = { {1, 2, 1.0} };
        auto graph = graph::make_graph<graph::GraphAdjMatrix, false>(vertices, edges);
        auto result = algo(graph, 5);
        std::cout << "    Несуществующая стартовая вершина: " << (result.has_value() ? "ошибка" : "корректно nullopt") << std::endl;
    }

    // 3. Отрицательные веса
    {
        std::vector<int> vertices = { 1, 2, 3 };
        std::vector<weighted_edge> edges = { {1, 2, 1.0}, {2, 3, -2.0} };
        auto graph = graph::make_graph<graph::GraphAdjMatrix, true>(vertices, edges);
        auto result = algo(graph, 1);
        // Дейкстра должна вернуть nullopt, Беллман-Форд - результат
        std::cout << "    Отрицательный вес: " << (result.has_value() ? "есть результат" : "nullopt") << std::endl;
    }

    // 4. Несвязный граф
    {
        std::vector<int> vertices = { 1, 2, 3, 4 };
        std::vector<weighted_edge> edges = { {1, 2, 1.0}, {3, 4, 2.0} };
        auto graph = graph::make_graph<graph::GraphAdjMatrix, false>(vertices, edges);
        auto result = algo(graph, 1);
        std::cout << "    Несвязный граф: " << (result.has_value() ? "ошибка - должен быть nullopt" : "корректно nullopt") << std::endl;
    }
}

// ============ ТЕСТОВЫЕ ГРАФЫ ============

// 1. Простой граф с 3 вершинами (треугольник)
std::vector<int> vertices_triangle_sp = { 1, 2, 3 };
std::vector<weighted_edge> edges_triangle_sp = {
    {1, 2, 1.0},
    {2, 3, 2.0},
    {1, 3, 5.0}
};
ExpectedShortestPaths expected_triangle_sp = {
    {{1, 2}, {2, 3}},  // путь до 2: 1-2, до 3: 1-2-3
    {{1, 0.0}, {2, 1.0}, {3, 3.0}}
};

// 2. Более сложный граф (6 вершин)
std::vector<int> vertices_complex_sp = { 1, 2, 3, 4, 5, 6 };
std::vector<weighted_edge> edges_complex_sp = {
    {1, 2, 7.0},
    {1, 3, 9.0},
    {2, 3, 10.0},
    {2, 4, 15.0},
    {3, 4, 11.0},
    {3, 6, 2.0},
    {4, 5, 6.0},
    {6, 5, 9.0}
};
ExpectedShortestPaths expected_complex_sp = {
    {{1, 2}, {1, 3}, {3, 4}, {3, 6}, {6, 5}},  // правильное дерево
    {{1, 0.0}, {2, 7.0}, {3, 9.0}, {4, 20.0}, {5, 20.0}, {6, 11.0}}
};

// 3. Ориентированный граф (DAG)
std::vector<int> vertices_directed_sp = { 1, 2, 3, 4, 5 };
std::vector<weighted_edge> edges_directed_sp = {
    {1, 2, 2.0},
    {1, 3, 4.0},
    {2, 3, 1.0},
    {2, 4, 7.0},
    {3, 5, 3.0},
    {4, 5, 1.0}
};
ExpectedShortestPaths expected_directed_sp = {
    {{1, 2}, {2, 3}, {2, 4}, {3, 5}},  // правильное дерево для ориентированного графа
    {{1, 0.0}, {2, 2.0}, {3, 3.0}, {4, 9.0}, {5, 6.0}}
};

// 4. Граф с одинаковыми весами (несколько кратчайших путей)
std::vector<int> vertices_equal_sp = { 1, 2, 3, 4 };
std::vector<weighted_edge> edges_equal_sp = {
    {1, 2, 1.0},
    {1, 3, 1.0},
    {2, 4, 1.0},
    {3, 4, 1.0}
};
ExpectedShortestPaths expected_equal_sp = {
    {{1, 2}, {1, 3}, {2, 4}},  // один из возможных вариантов
    {{1, 0.0}, {2, 1.0}, {3, 1.0}, {4, 2.0}}
};

// 5. Граф из одной вершины
std::vector<int> vertices_single_sp = { 1 };
std::vector<weighted_edge> edges_single_sp = {};
ExpectedShortestPaths expected_single_sp = {
    {},
    {{1, 0.0}}
};

// 6. Граф с большими весами
std::vector<int> vertices_large_sp = { 1, 2, 3, 4 };
std::vector<weighted_edge> edges_large_sp = {
    {1, 2, 100.0},
    {1, 3, 200.0},
    {2, 3, 50.0},
    {2, 4, 150.0},
    {3, 4, 100.0}
};
ExpectedShortestPaths expected_large_sp = {
    {{1, 2}, {2, 3}, {2, 4}},  // путь до 3: 1-2-3 (150), до 4: 1-2-4 (250)
    {{1, 0.0}, {2, 100.0}, {3, 150.0}, {4, 250.0}}
};

// 7. Цепочка вершин
std::vector<int> vertices_chain_sp = { 1, 2, 3, 4, 5 };
std::vector<weighted_edge> edges_chain_sp = {
    {1, 2, 2.0},
    {2, 3, 3.0},
    {3, 4, 1.0},
    {4, 5, 4.0}
};
ExpectedShortestPaths expected_chain_sp = {
    {{1, 2}, {2, 3}, {3, 4}, {4, 5}},
    {{1, 0.0}, {2, 2.0}, {3, 5.0}, {4, 6.0}, {5, 10.0}}
};

// 10. Граф с отрицательным весом (ориентированный)
std::vector<int> vertices_negative = { 1, 2, 3, 4 };
std::vector<weighted_edge> edges_negative = {
    {1, 2, 4.0},
    {1, 3, 2.0},
    {3, 2, -1.0},   // ребро ИЗ 3 В 2
    {2, 4, 5.0},
    {3, 4, 3.0}
};
ExpectedShortestPaths expected_negative = {
    {{1, 3}, {3, 2}, {3, 4}},  // 1→3 (2), 3→2 (-1), 3→4 (3)
    {{1, 0.0}, {2, 1.0}, {3, 2.0}, {4, 5.0}}
};

// 11. Граф с отрицательным циклом
std::vector<int> vertices_negative_cycle = { 1, 2, 3 };
std::vector<weighted_edge> edges_negative_cycle = {
    {1, 2, 1.0},
    {2, 3, 2.0},
    {3, 1, -5.0}   // образует цикл 1-2-3-1 с суммой -2
};

// ============ ТЕСТЫ С ПОЛЬЗОВАТЕЛЬСКОЙ СТРУКТУРОЙ ============

struct ColourWay {
    double dist;
    std::string colour;
    ColourWay(double dist, std::string colour) : dist(dist), colour(colour) {}

    operator double() const {
        if (colour == "red") return dist * 2;
        if (colour == "green") return dist * 0.5;
        return dist;
    }
};

using weighted_colour = std::tuple<int, int, ColourWay>;

template<typename Algorithm>
void testCustomStructure(Algorithm algo, const std::string& algo_name) {
    std::cout << "\n  ТЕСТ С ПОЛЬЗОВАТЕЛЬСКОЙ СТРУКТУРОЙ (ColourWay):" << std::endl;

    std::vector<int> vertices = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<weighted_colour> edges = {
        {1, 2, ColourWay(7, "red")},
        {1, 3, ColourWay(9, "red")},
        {2, 3, ColourWay(10, "blue")},
        {2, 4, ColourWay(15, "blue")},
        {3, 4, ColourWay(11, "blue")},
        {3, 6, ColourWay(2, "green")},
        {4, 5, ColourWay(6, "blue")},
        {6, 5, ColourWay(9, "blue")},
        {5, 7, ColourWay(2, "blue")},
        {1, 7, ColourWay(2, "blue")},
        {6, 8, ColourWay(4, "blue")},
        {5, 8, ColourWay(1, "blue")}
    };

    auto graph = graph::make_graph<graph::GraphAdjMatrix, false>(vertices, edges);
    auto result = algo(graph, 1);

    printShortestPaths(result, algo_name +" с ColourWay");

    if (result.has_value()) {
        std::cout << "    + Пользовательская структура успешно преобразована в double" << std::endl;
    }
}
// ============ ТЕСТЫ С ОТРИЦАТЕЛЬНЫМ ЦИКЛОМ ============
void testNegativeCycle() {
    std::cout << "\n  Граф с отрицательным циклом:" << std::endl;

    auto graph = graph::make_graph<graph::GraphAdjMatrix, false>(
        vertices_negative_cycle, edges_negative_cycle);

    auto result = bellmanFord(graph, 1);

    std::cout << "    Беллман-Форд: "
        << (result.has_value() ? "есть результат (ошибка!)" : "nullopt (отрицательный цикл)")
        << std::endl;
}

void compareDijkstraAndBellmanFord() {
    std::cout << "\n  СРАВНЕНИЕ ДЕЙКСТРЫ И БЕЛЛМАНА-ФОРДА (граф с отрицательным весом):" << std::endl;

    auto graph = graph::make_graph<graph::GraphAdjMatrix, true>(
        vertices_negative, edges_negative);

    auto dijkstra_result = dijkstra(graph, 1);
    auto bellmanford_result = bellmanFord(graph, 1);

    std::cout << "    Дейкстра: "
        << (dijkstra_result.has_value() ? "есть результат (ошибка!)" : "nullopt (отрицательный вес)")
        << std::endl;

    printShortestPaths(bellmanford_result, "    Беллман-Форд");

    if (bellmanford_result.has_value()) {
        bool ok = validateShortestPaths(bellmanford_result.value(), expected_negative);
        if (ok) {
            std::cout << "    + Беллман-Форд корректно нашёл пути с отрицательным весом" << std::endl;
        }
    }
}

// ============ ГЛАВНАЯ ФУНКЦИЯ ТЕСТИРОВАНИЯ ============

void testAllShortestPathAlgorithms() {
    std::cout << "\n";
    std::cout << "\n========== ТЕСТИРОВАНИЕ АЛГОРИТМА ДЕЙКСТРЫ ==========\n";

    // Тест 1: Треугольник
    testDijkstra(vertices_triangle_sp, edges_triangle_sp, 1,
        "Треугольник (1-2=1, 2-3=2, 1-3=5)",
        expected_triangle_sp);

    // Тест 2: Сложный граф (6 вершин)
    testDijkstra(vertices_complex_sp, edges_complex_sp, 1,
        "Сложный граф (6 вершин, веса 7,9,10,15,11,2,6,9)",
        expected_complex_sp);

    // Тест 3: Ориентированный граф
    testDijkstra<true>(vertices_directed_sp, edges_directed_sp, 1,
        "Ориентированный граф (DAG)",
        expected_directed_sp);

    // Тест 4: Граф с одинаковыми весами
    testDijkstra(vertices_equal_sp, edges_equal_sp, 1,
        "Граф с одинаковыми весами (несколько кратчайших путей)",
        expected_equal_sp);

    // Тест 5: Граф из одной вершины
    testDijkstra(vertices_single_sp, edges_single_sp, 1,
        "Граф из одной вершины",
        expected_single_sp);

    // Тест 6: Граф с большими весами
    testDijkstra(vertices_large_sp, edges_large_sp, 1,
        "Граф с большими весами (100,200,50,150,100)",
        expected_large_sp);

    // Тест 7: Цепочка вершин
    testDijkstra(vertices_chain_sp, edges_chain_sp, 1,
        "Цепочка вершин (2,3,1,4)",
        expected_chain_sp);  
    

    // Тест 8: Пользовательская структура
    testCustomStructure([](auto& graph, int start) {
        return dijkstra(graph, start);
        }, "    Дейкстра");

    // Тест 9: Ошибочные ситуации
    testErrorCases([](auto& graph, int start) {
        return dijkstra(graph, start);
        }, "Дейкстры");

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  ЗАВЕРШЕНИЕ ТЕСТИРОВАНИЯ ДЕЙКСТРЫ\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Все тесты выполнены!\n";
    
    std::cout << "\n";
    std::cout << "\n========== ТЕСТИРОВАНИЕ АЛГОРИТМА БЕЛЛМАНА-ФОРДА ==========\n";
    
    // Тест 1: Треугольник
    testBellmanFord(vertices_triangle_sp, edges_triangle_sp, 1,
        "Треугольник (1-2=1, 2-3=2, 1-3=5)",
        expected_triangle_sp);
    
    // Тест 2: Сложный граф (6 вершин)
    testBellmanFord(vertices_complex_sp, edges_complex_sp, 1,
        "Сложный граф (6 вершин, веса 7,9,10,15,11,2,6,9)",
        expected_complex_sp);

    // Тест 3: Ориентированный граф
    testBellmanFord<true>(vertices_directed_sp, edges_directed_sp, 1,
        "Ориентированный граф (DAG)",
        expected_directed_sp);

    // Тест 4: Граф с одинаковыми весами
    testBellmanFord(vertices_equal_sp, edges_equal_sp, 1,
        "Граф с одинаковыми весами (несколько кратчайших путей)",
        expected_equal_sp);

    // Тест 5: Граф из одной вершины
    testBellmanFord(vertices_single_sp, edges_single_sp, 1,
        "Граф из одной вершины",
        expected_single_sp);

    // Тест 6: Граф с большими весами
    testBellmanFord(vertices_large_sp, edges_large_sp, 1,
        "Граф с большими весами (100,200,50,150,100)",
        expected_large_sp);

    // Тест 7: Цепочка вершин
    testBellmanFord(vertices_chain_sp, edges_chain_sp, 1,
        "Цепочка вершин (2,3,1,4)",
        expected_chain_sp);

    // Тест 8: Отрицательный вес
    testBellmanFord<true>(vertices_negative, edges_negative, 1,
        "Негативный вес",
        expected_negative);    

    // Тест 9: Пользовательская структура
    testCustomStructure([](auto& graph, int start) {
        return bellmanFord(graph, start);
        }, "    Беллман Форд");

    // Тест 10: Ошибочные ситуации
    testErrorCases([](auto& graph, int start) {
        return bellmanFord(graph, start);
        }, "Беллмана Форда");

    compareDijkstraAndBellmanFord();
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  ЗАВЕРШЕНИЕ ТЕСТИРОВАНИЯ БЕЛЛМАНА-ФОРДА\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Все тесты выполнены!\n";
    
    
};
