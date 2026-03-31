#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "algorithms/graph_algorithms_topological.h"
#include "graph/GraphAdjVectors.h"
#include "graph/graph_factory.h"
#include "optional"

using namespace graph_algorithms;

//Вспомогательные функции для вывода
// 1. Для optional<vector<int>> (Тарьян)
void printResult(const std::optional<std::vector<int>>& result, const std::string& name) {
    std::cout << name << ": ";
    if (result.has_value()) {
        const auto& vec = result.value();
        std::cout << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << vec[i];
        }
        std::cout << "]\n";
    }
    else {
        std::cout << "ГРАФ СОДЕРЖИТ ЦИКЛ!\n";
    }
}

// 2. Для optional<vector<vector<int>>> (Демукрон)
void printResult(const std::optional<std::vector<std::vector<int>>>& result, const std::string& name) {
    std::cout << name << ":\n";
    if (!result.has_value()) {
        std::cout << "  ГРАФ СОДЕРЖИТ ЦИКЛ!\n";
        return;
    }

    const auto& levels = result.value();
    if (levels.empty()) {
        std::cout << "  Пустой результат\n";
        return;
    }

    for (size_t level = 0; level < levels.size(); ++level) {
        std::cout << "  Уровень " << level << ": [";
        const auto& vertices = levels[level];
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << vertices[i];
        }
        std::cout << "]\n";
    }
}

// 3. Для vector<pair<int, int>> (Мосты)
void printResult(const std::vector<std::pair<int, int>>& bridges, const std::string& name) {
    std::cout << name << ": ";
    if (bridges.empty()) {
        std::cout << "Мостов нет\n";
        return;
    }
    std::cout << "[";
    for (size_t i = 0; i < bridges.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "(" << bridges[i].first << ", " << bridges[i].second << ")";
    }
    std::cout << "]\n";
}
void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

//Определение графов для тестирования
using edge_data = std::tuple<int, int, bool>;
// Граф 1: Простой DAG (0->1->2->3)
std::vector<int> vertices_dag_simple = { 0, 1, 2, 3 };
std::vector<edge_data> edges_dag_simple = {
    {0, 1, true},
    {1, 2, true},
    {2, 3, true}
};

// Граф 2: Сложный DAG (вершины 1-6)
std::vector<int> vertices_dag_complex = { 1, 2, 3, 4, 5, 6 };
std::vector<edge_data> edges_dag_complex = {
    {1, 2, true},
    {1, 4, true},
    {3, 2, true},
    {2, 6, true},
    {4, 6, true},
    {6, 5, true}
};

// Граф 3: DAG с несколькими корнями (7,8 изолированы)
std::vector<int> vertices_dag_multiroot = { 1, 2, 3, 4, 5, 6, 7, 8 };
std::vector<edge_data> edges_dag_multiroot = {
    {1, 2, true},
    {1, 4, true},
    {3, 2, true},
    {2, 6, true},
    {4, 6, true},
    {6, 5, true},
    {7, 8, true}
};

// Граф 4: С циклом (0->1->2->0)
std::vector<int> vertices_cyclic_simple = { 0, 1, 2 };
std::vector<edge_data> edges_cyclic_simple = {
    {0, 1, true},
    {1, 2, true},
    {2, 0, true}
};

// Граф 5: Сложный с циклами (0->1->2->0 и 3↔4)
std::vector<int> vertices_cyclic_complex = { 0, 1, 2, 3, 4, 5 };
std::vector<edge_data> edges_cyclic_complex = {
    {0, 1, true},
    {1, 2, true},
    {2, 0, true},    // цикл 0-1-2
    {2, 3, true},
    {3, 4, true},
    {4, 5, true},
    {4, 3, true}     // цикл 3-4
};

//Для мостов
// Граф 6: Дерево (все ребра — мосты)
std::vector<int> vertices_tree = { 0, 1, 2, 3, 4, 5, 6 };
std::vector<edge_data> edges_tree = {
    {0, 1, true},
    {0, 2, true},
    {1, 3, true},
    {1, 4, true},
    {2, 5, true},
    {2, 6, true}
};

// Граф 7: Простой путь (все ребра — мосты)
std::vector<int> vertices_path = { 0, 1, 2, 3 };
std::vector<edge_data> edges_path = {
    {0, 1, true},
    {1, 2, true},
    {2, 3, true}
};

// Граф 8: Цикл (нет мостов)
std::vector<int> vertices_cycle = { 0, 1, 2, 3 };
std::vector<edge_data> edges_cycle = {
    {0, 1, true},
    {1, 2, true},
    {2, 3, true},
    {3, 0, true}
};

// Граф 9: Полный граф K4 (нет мостов)
std::vector<int> vertices_k4 = { 0, 1, 2, 3 };
std::vector<edge_data> edges_k4 = {
    {0, 1, true}, {0, 2, true}, {0, 3, true},
    {1, 2, true}, {1, 3, true},
    {2, 3, true}
};

// Граф 10: Двусвязный граф (нет мостов)
std::vector<int> vertices_biconnected = { 0, 1, 2, 3 };
std::vector<edge_data> edges_biconnected = {
    {0, 1, true}, {1, 2, true}, {2, 3, true}, {3, 0, true},
    {0, 2, true}
};

// Граф 11: Граф с мостами (два треугольника, соединенные мостом)
std::vector<int> vertices_with_bridges = { 0, 1, 2, 3, 4, 5 };
std::vector<edge_data> edges_with_bridges = {
    {0, 1, true}, {1, 2, true}, {2, 0, true},  // треугольник 0-1-2
    {2, 3, true},                               // мост
    {3, 4, true}, {4, 5, true}, {5, 3, true}    // треугольник 3-4-5
};

//Собтвенно тесты
void testDemukron() {
    printSeparator("АЛГОРИТМ ДЕМУКРОНА (ярусная топологическая сортировка)");

    // Тест 1: Простой DAG
    auto graph1 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_dag_simple, edges_dag_simple);
    auto result1 = demukron(graph1);
    printResult(result1, "Простой DAG (0->1->2->3)");

    // Тест 2: Сложный DAG
    auto graph2 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_dag_complex, edges_dag_complex);
    auto result2 = demukron(graph2);
    printResult(result2, "Сложный DAG (1-6)");

    // Тест 3: DAG с несколькими корнями
    auto graph3 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_dag_multiroot, edges_dag_multiroot);
    auto result3 = demukron(graph3);
    printResult(result3, "DAG с несколькими корнями (1-8)");

    // Тест 4: Простой граф с циклом
    auto graph4 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_cyclic_simple, edges_cyclic_simple);
    auto result4 = demukron(graph4);
    printResult(result4, "Граф с циклом (0->1->2->0)");

    // Тест 5: Сложный граф с циклами
    auto graph5 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_cyclic_complex, edges_cyclic_complex);
    auto result5 = demukron(graph5);
    printResult(result5, "Сложный граф с циклами");
}

void testTarjan() {
    printSeparator("АЛГОРИТМ ТАРЬЯНА (топологическая сортировка)");

    // Тест 1: Простой DAG
    auto graph1 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_dag_simple, edges_dag_simple);
    auto result1 = tarjan(graph1);
    printResult(result1, "Простой DAG (0->1->2->3)");

    // Тест 2: Сложный DAG
    auto graph2 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_dag_complex, edges_dag_complex);
    auto result2 = tarjan(graph2);
    printResult(result2, "Сложный DAG (1-6)");

    // Тест 3: DAG с несколькими корнями
    auto graph3 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_dag_multiroot, edges_dag_multiroot);
    auto result3 = tarjan(graph3);
    printResult(result3, "DAG с несколькими корнями (1-8)");

    // Тест 4: Простой граф с циклом
    auto graph4 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_cyclic_simple, edges_cyclic_simple);
    auto result4 = tarjan(graph4);
    printResult(result4, "Граф с циклом (0->1->2->0)");

    // Тест 5: Сложный граф с циклами
    auto graph5 = graph::make_graph<graph::GraphAdjVectors, true>(vertices_cyclic_complex, edges_cyclic_complex);
    auto result5 = tarjan(graph5);
    printResult(result5, "Сложный граф с циклами");
}

void testBridges() {
    printSeparator("АЛГОРИТМ ПОИСКА МОСТОВ");

    // Тест 1: Дерево (все ребра — мосты)
    auto graph1 = graph::make_graph<graph::GraphAdjVectors, false>(vertices_tree, edges_tree);
    auto result1 = findBridges(graph1);
    printResult(result1, "Дерево (7 вершин, 6 ребер)");

    // Тест 2: Простой путь (все ребра — мосты)
    auto graph2 = graph::make_graph<graph::GraphAdjVectors, false>(vertices_path, edges_path);
    auto result2 = findBridges(graph2);
    printResult(result2, "Простой путь (4 вершины)");

    // Тест 3: Цикл (нет мостов)
    auto graph3 = graph::make_graph<graph::GraphAdjVectors, false>(vertices_cycle, edges_cycle);
    auto result3 = findBridges(graph3);
    printResult(result3, "Цикл C4");

    // Тест 4: Полный граф K4 (нет мостов)
    auto graph4 = graph::make_graph<graph::GraphAdjVectors, false>(vertices_k4, edges_k4);
    auto result4 = findBridges(graph4);
    printResult(result4, "Полный граф K4");

    // Тест 5: Двусвязный граф (нет мостов)
    auto graph5 = graph::make_graph<graph::GraphAdjVectors, false>(vertices_biconnected, edges_biconnected);
    auto result5 = findBridges(graph5);
    printResult(result5, "Двусвязный граф (квадрат с диагональю)");

    // Тест 6: Граф с мостами (два треугольника + мост)
    auto graph6 = graph::make_graph<graph::GraphAdjVectors, false>(vertices_with_bridges, edges_with_bridges);
    auto result6 = findBridges(graph6);
    printResult(result6, "Граф с мостами (два треугольника, соединенные мостом)");
}

void testAllAlgorithms() {
    std::cout << "\n";
    std::cout << "\n========== ТЕСТИРОВАНИЕ АЛГОРИТМОВ НА ГРАФАХ ==========\n";

    testDemukron();
    testTarjan();
    testBridges();

    printSeparator("ЗАВЕРШЕНИЕ ТЕСТИРОВАНИЯ");
    std::cout << "Все тесты выполнены!\n";
}
