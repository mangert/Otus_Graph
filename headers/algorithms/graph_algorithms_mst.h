#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "graph/IGraph.h"
#include <optional>
#include "data_structures/UnionFind.h"
#include "data_structures/PriorityQueue.h"

// graph_algorithms_mst.h
// Алгоритмы построения минимального остовного дерева (MST):
// - Краскала (Kruskal)
// - Прима (Prim)
// - Борувки (Boruvka)
namespace graph_algorithms {	
    
    template<typename Vertex>
    //структура для представления ребра
    struct Edge {
        Vertex v1;
        Vertex v2;
        Edge() = default;
        Edge(const Vertex& u, const Vertex& v) : v1(u), v2(v) {}
    };

    //Алгоритм Краскала
    template<template<graph::Comparable, typename, bool> class Graph,
        graph::Comparable Vertex,
        typename EdgeInfo>
        requires graph::Comparable<EdgeInfo>  // EdgeInfo должен быть сравнимым для сортировки
    std::optional <std::vector<Edge<Vertex>>> kruskal(Graph<Vertex, EdgeInfo, false>& graph) {

        // 1. Получаем все ребра
        auto edges = graph.getEdges();  // vector<tuple<Vertex, Vertex, EdgeInfo>>

        // 2. Сортируем по весу (EdgeInfo)
        std::sort(edges.begin(), edges.end(),
            [](const auto& a, const auto& b) {
                return std::get<2>(a) < std::get<2>(b);
            });

        // 3. Инициализируем Union-Find
        UnionFind<Vertex> uf;
        auto vertices = graph.getVertices();  // сохраняем явно
        for (const auto& v : vertices) {
            uf.makeSet(v);
        }

        // 4. Строим MST
        std::vector<Edge<Vertex>> mst;
        mst.reserve(vertices.size() - 1);  // резервируем память

        for (const auto& [from, to, weight] : edges) {
            if (!uf.isConnected(from, to)) {
                uf.unite(from, to);
                mst.emplace_back(from, to);

                // Ранний выход, если собрали все ребра
                if (mst.size() == vertices.size() - 1) {
                    break;
                }
            }
        }
        if (mst.size() != vertices.size() - 1)
            return std::nullopt;
        return mst;
    }

    //Алгоритм Прима    
    template<template<typename, typename, bool> class Graph,
        typename Vertex,
        typename EdgeInfo>
        requires graph::Comparable<EdgeInfo>&& std::is_default_constructible_v<EdgeInfo>
    std::optional<std::vector<Edge<Vertex>>> prim(const Graph<Vertex, EdgeInfo, false>& graph,
        const Vertex& start) {

        using Weight = EdgeInfo;
        using QueueElement = std::pair<Weight, Vertex>;

        // Проверяем существование стартовой вершины
        if (!graph.hasVertex(start)) {
            return std::nullopt;
        }

        auto vertices = graph.getVertices();
        size_t count_v = vertices.size();
        
        //Компаратор для min-heap: меньший вес выше
        auto min_heap_cmp = [](const QueueElement& a, const QueueElement& b) {
            return a.first < b.first;  
            };
        PriorityQueue<QueueElement, decltype(min_heap_cmp)> pq(min_heap_cmp);

        // Храним минимальный вес к каждой вершине
        std::map<Vertex, std::optional<Weight>> min_weight;
        std::map<Vertex, Vertex> parent;
        std::map<Vertex, bool> in_mst;

        // Инициализация
        for (const auto& v : vertices) {
            min_weight[v] = std::nullopt;
            in_mst[v] = false;
        }

        // Начинаем со стартовой вершины
        min_weight[start] = Weight{};
        pq.push(std::pair(Weight{}, start));

        std::vector<Edge<Vertex>> mst;
        mst.reserve(count_v - 1);

        while (!pq.empty()) {
            auto [weight, u] = pq.top();
            pq.pop();

            // Пропускаем, если уже в MST
            if (in_mst[u]) continue;

            // Добавляем u в MST
            in_mst[u] = true;

            // Если не стартовая вершина, добавляем ребро в результат
            if (parent.find(u) != parent.end()) {
                mst.emplace_back(parent[u], u);
            }

            // Обновляем веса соседей
            for (const auto& neighbor : graph.getNeighbors(u)) {
                if (in_mst[neighbor]) continue;

                auto edge_info = graph.getEdgeInfo(u, neighbor);
                if (!edge_info.has_value()) continue;

                Weight w = edge_info.value();

                if (!min_weight[neighbor].has_value() || w < min_weight[neighbor].value()) {
                    min_weight[neighbor] = w;
                    parent[neighbor] = u;
                    pq.push(std::pair(w, neighbor));
                }
            }
        }

        // Проверяем, все ли вершины в MST
        if (mst.size() != count_v - 1) {
            return std::nullopt;  // граф несвязный
        }

        return mst;
    }

    //Алгоритм Борувки
    template<template<typename, typename, bool> class Graph,
        typename Vertex,
        typename EdgeInfo>
        requires graph::Comparable<EdgeInfo>
    std::optional<std::vector<Edge<Vertex>>> boruvka(const Graph<Vertex, EdgeInfo, false>& graph) {

        using Weight = EdgeInfo;

        auto vertices = graph.getVertices();
        size_t count_v = vertices.size();

        if (!count_v) return std::vector<Edge<Vertex>>{};

        // Union-Find для отслеживания компонент
        UnionFind<Vertex> uf;
        for (const auto& v : vertices) {
            uf.makeSet(v);
        }
        size_t count_e = count_v - 1;
        std::vector<Edge<Vertex>> mst;
        mst.reserve(count_e);

        // Пока не соберем все ребра
        using EdgeType = std::tuple<Vertex, Vertex, Weight>;
        //вспомогательная - достает вес из кортажа
        auto getWeight = [](const EdgeType& edge) -> const Weight& {
            return std::get<2>(edge);
            };
        auto edges = graph.getEdges();
        
        while (mst.size() < (count_e)) {

            // Для каждой вершины храним лучшее ребро из ее компоненты
            std::map<Vertex, EdgeType> cheapest;  // component_root -> (from, to, weight)

            // 1. Для каждого ребра проверяем, нужно ли оно          

            for (const auto& [from, to, weight] : edges) {
                
                // Если ребро внутри компоненты - пропускаем
                if (uf.isConnected(from, to)) continue;
                
                Vertex root_from = uf.root(from);
                Vertex root_to = uf.root(to);              

                // Обновляем лучшее ребро для компоненты root_from
                auto it_from = cheapest.find(root_from);
                if (it_from == cheapest.end() ||
                    weight < getWeight(it_from->second)) { //
                    cheapest[root_from] = { from, to, weight };
                }

                // Обновляем лучшее ребро для компоненты root_to
                auto it_to = cheapest.find(root_to);
                if (it_to == cheapest.end() ||
                    weight < getWeight(it_to->second)) {
                    cheapest[root_to] = { from, to, weight };
                }
            }

            // 2. Добавляем все найденные лучшие ребра
            bool added = false;
            for (const auto& [root, edge] : cheapest) {
                auto [from, to, weight] = edge;

                // Проверяем, что ребро все еще нужно (компоненты могли объединиться)
                if (!uf.isConnected(from, to)) {
                    uf.unite(from, to);
                    mst.emplace_back(from, to);
                    added = true;
                }
            }

            // Если не добавили ни одного ребра - граф несвязный
            if (!added) {
                return std::nullopt;
            }
        }

        return mst;
    }
}//namespace graph_algorithms