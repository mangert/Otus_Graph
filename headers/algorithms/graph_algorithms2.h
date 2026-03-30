#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "graph/IGraph.h"
#include <optional>
#include "data_structures/UnionFind.h"
#include "data_structures/PriorityQueue.h"

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
    std::vector<Edge<Vertex>> kruskal(Graph<Vertex, EdgeInfo, false>& graph) {

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
}//namespace graph_algorithms