#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include "algorithms/graph_algorithms_mst.h"
#include "graph/GraphIncMatrix.h"
#include "graph/graph_factory.h"

using namespace graph_algorithms;

// Вспомогательная структура для ребра с весом
using weighted_edge = std::tuple<int, int, double>;

// Структура для хранения ожидаемого результата MST (набор ребер)
struct ExpectedMST {
    std::vector<std::pair<int, int>> edges;
    double total_weight;
};

// Функция для нормализации ребра (для неориентированного графа)
std::pair<int, int> normalizeEdge(int v1, int v2) {
    if (v1 < v2) return { v1, v2 };
    return { v2, v1 };
}

// Функция для проверки корректности MST
bool validateMST(const std::vector<Edge<int>>& mst,
    const std::vector<std::pair<int, int>>& expected_edges,
    double expected_weight) {

    if (mst.size() != expected_edges.size()) {
        std::cout << "      Размеры не совпадают: " << mst.size()
            << " vs " << expected_edges.size() << std::endl;
        return false;
    }

    // Нормализуем и сортируем оба набора
    std::vector<std::pair<int, int>> normalized_mst;
    for (const auto& e : mst) {
        normalized_mst.push_back(normalizeEdge(e.v1, e.v2));
    }

    auto normalized_expected = expected_edges;
    for (auto& e : normalized_expected) {
        e = normalizeEdge(e.first, e.second);
    }

    std::sort(normalized_mst.begin(), normalized_mst.end());
    std::sort(normalized_expected.begin(), normalized_expected.end());

    // Сравниваем
    for (size_t i = 0; i < normalized_mst.size(); ++i) {
        if (normalized_mst[i] != normalized_expected[i]) {
            std::cout << "      Ребро не совпадает: ("
                << normalized_mst[i].first << "," << normalized_mst[i].second
                << ") vs (" << normalized_expected[i].first << ","
                << normalized_expected[i].second << ")" << std::endl;
            return false;
        }
    }

    return true;
}

// Функция для вывода MST
void printMST(const std::optional<std::vector<Edge<int>>>& result,
    const std::string& name) {
    std::cout << name << ": ";
    if (!result.has_value()) {
        std::cout << "ГРАФ НЕСВЯЗНЫЙ!" << std::endl;
        return;
    }

    const auto& mst = result.value();
    if (mst.empty()) {
        std::cout << "пустое дерево (1 вершина)" << std::endl;
        return;
    }

    double total_weight = 0.0;
    std::cout << "[";
    for (size_t i = 0; i < mst.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "(" << mst[i].v1 << "-" << mst[i].v2 << ")";
    }
    std::cout << "]" << std::endl;
}

// Функция для сравнения всех трех алгоритмов
void testAllMST(const std::vector<int>& vertices,
    const std::vector<weighted_edge>& edges,
    const std::string& test_name,
    const ExpectedMST& expected) {

    std::cout << "\n  " << test_name << ":" << std::endl;

    // Создаем граф (используем матрицу инцидентности)
    auto graph = graph::make_graph<graph::GraphIncMatrix, false>(
        vertices, edges);

    // Запускаем все три алгоритма
    auto kruskal_result = kruskal(graph);
    auto prim_result = prim(graph, vertices[0]);
    auto boruvka_result = boruvka(graph);

    // Выводим результаты
    printMST(kruskal_result, "    Краскал");
    printMST(prim_result, "    Прим");
    printMST(boruvka_result, "    Борувка");

    // Проверяем корректность
    if (kruskal_result.has_value() && prim_result.has_value() && boruvka_result.has_value()) {
        bool kruskal_ok = validateMST(kruskal_result.value(), expected.edges, expected.total_weight);
        bool prim_ok = validateMST(prim_result.value(), expected.edges, expected.total_weight);
        bool boruvka_ok = validateMST(boruvka_result.value(), expected.edges, expected.total_weight);

        if (kruskal_ok && prim_ok && boruvka_ok) {
            std::cout << "    + Все алгоритмы вернули корректный MST (вес: "
                << expected.total_weight << ")" << std::endl;
        }
        else {
            std::cout << "    ! ОШИБКА: алгоритмы вернули разные результаты!" << std::endl;
        }
    }
    else {
        std::cout << "    ! ОШИБКА: граф несвязный, но ожидается связный!" << std::endl;
    }
}

// Функция для тестирования несвязного графа
void testDisconnectedGraph() {
    std::cout << "\n  Несвязный граф (две компоненты):" << std::endl;

    std::vector<int> vertices = { 1, 2, 3, 4, 5, 6 };
    std::vector<weighted_edge> edges = {
        {1, 2, 1.0}, {2, 3, 2.0},  // компонента 1: 1-2-3
        {4, 5, 3.0}, {5, 6, 4.0}   // компонента 2: 4-5-6
    };

    auto graph = graph::make_graph<graph::GraphIncMatrix, false>(
        vertices, edges);

    auto kruskal_result = kruskal(graph);
    auto prim_result = prim(graph, 1);
    auto boruvka_result = boruvka(graph);

    printMST(kruskal_result, "    Краскал");
    printMST(prim_result, "    Прим");
    printMST(boruvka_result, "    Борувка");

    if (!kruskal_result.has_value() && !prim_result.has_value() && !boruvka_result.has_value()) {
        std::cout << "    + Все алгоритмы корректно определили несвязный граф" << std::endl;
    }
    else {
        std::cout << "    ! ОШИБКА: несвязный граф не был обнаружен!" << std::endl;
    }
}

// Основные тестовые графы

// 1. Простой граф с 3 вершинами (треугольник)
std::vector<int> vertices_triangle = { 1, 2, 3 };
std::vector<weighted_edge> edges_triangle = {
    {1, 2, 1.0},
    {2, 3, 2.0},
    {1, 3, 3.0}
};
ExpectedMST expected_triangle = {
    {{1, 2}, {2, 3}},  // ребра: 1-2 (1.0), 2-3 (2.0)
    3.0
};

// 2. Произвольный граф (6 вершин)
std::vector<int> vertices_example = { 1, 2, 3, 4, 5, 6 };
std::vector<weighted_edge> edges_example = {
    {1, 2, 1.5},
    {1, 4, 2.0},
    {3, 2, 0.5},
    {2, 6, 3.0},
    {4, 6, 6.0},
    {6, 5, 2.0}
};
ExpectedMST expected_example = {
    {{2, 3}, {1, 2}, {1, 4}, {5, 6}, {2, 6}},
    0.5 + 1.5 + 2.0 + 2.0 + 3.0  // = 9.0
};

// 3. Дерево (все ребра в MST)
std::vector<int> vertices_tree_mst = { 1, 2, 3, 4, 5 };
std::vector<weighted_edge> edges_tree_mst = {
    {1, 2, 1.0},
    {1, 3, 2.0},
    {2, 4, 3.0},
    {2, 5, 4.0}
};
ExpectedMST expected_tree = {
    {{1, 2}, {1, 3}, {2, 4}, {2, 5}},
    1.0 + 2.0 + 3.0 + 4.0  // = 10.0
};

// 4. Граф с одинаковыми весами
std::vector<int> vertices_equal = { 1, 2, 3, 4 };
std::vector<weighted_edge> edges_equal = {
    {1, 2, 1.0},
    {2, 3, 1.0},
    {3, 4, 1.0},
    {1, 4, 1.0},
    {2, 4, 1.0}
};
// MST может быть любым, главное - 3 ребра с весом 1.0
ExpectedMST expected_equal = {
    {{1, 2}, {2, 3}, {1, 4}},  // один из возможных вариантов
    3.0
};

// 5. Граф с одной вершиной
std::vector<int> vertices_single = { 1 };
std::vector<weighted_edge> edges_single = {};
ExpectedMST expected_single = {
    {},
    0.0
};

// 6. Полный граф K4
std::vector<int> vertices_k4_mst = { 1, 2, 3, 4 };
std::vector<weighted_edge> edges_k4_mst = {
    {1, 2, 1.0},
    {1, 3, 2.0},
    {1, 4, 3.0},
    {2, 3, 4.0},
    {2, 4, 5.0},
    {3, 4, 6.0}
};
ExpectedMST expected_k4 = {
    {{1, 2}, {1, 3}, {1, 4}},
    1.0 + 2.0 + 3.0  // = 6.0
};

// Функции тестирования
void testKruskal() {
    std::cout << "\n  АЛГОРИТМ КРАСКАЛА (отдельно):" << std::endl;

    auto graph = graph::make_graph<graph::GraphIncMatrix, false>(
        vertices_example, edges_example);
    auto result = kruskal(graph);
    printMST(result, "    Краскал на примере");
}

void testPrim() {
    std::cout << "\n  АЛГОРИТМ ПРИМА (отдельно):" << std::endl;

    auto graph = graph::make_graph<graph::GraphIncMatrix, false>(
        vertices_example, edges_example);
    auto result = prim(graph, 1);
    printMST(result, "    Прим на примере (старт=1)");
}

void testBoruvka() {
    std::cout << "\n  АЛГОРИТМ БОРУВКИ (отдельно):" << std::endl;

    auto graph = graph::make_graph<graph::GraphIncMatrix, false>(
        vertices_example, edges_example);
    auto result = boruvka(graph);
    printMST(result, "    Борувка на примере");
}

// Главная функция тестирования
void testAllMSTAlgorithms() {
    std::cout << "\n";
    std::cout << "\n========== ТЕСТИРОВАНИЕ АЛГОРИТМОВ MST ==========\n";

    // Тест 1: Треугольник
    testAllMST(vertices_triangle, edges_triangle, "Треугольник (1-2=1, 2-3=2, 1-3=3)",
        expected_triangle);

    // Тест 2: Произвольный граф (6 вершин)
    testAllMST(vertices_example, edges_example, "Произвольный граф (6 вершин)",
        expected_example);

    // Тест 3: Дерево
    testAllMST(vertices_tree_mst, edges_tree_mst, "Дерево (5 вершин)",
        expected_tree);

    // Тест 4: Граф с одинаковыми весами
    testAllMST(vertices_equal, edges_equal, "Граф с одинаковыми весами",
        expected_equal);

    // Тест 5: Одна вершина
    testAllMST(vertices_single, edges_single, "Граф из одной вершины",
        expected_single);

    // Тест 6: Полный граф K4
    testAllMST(vertices_k4_mst, edges_k4_mst, "Полный граф K4",
        expected_k4);

    // Тест 7: Несвязный граф
    testDisconnectedGraph();

    // Отдельные тесты
    testKruskal();
    testPrim();
    testBoruvka();

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  ЗАВЕРШЕНИЕ ТЕСТИРОВАНИЯ MST\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Все тесты выполнены!\n";
}