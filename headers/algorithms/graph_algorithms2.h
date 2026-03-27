#pragma once
#include <iostream>
#include <vector>
#include "graph/IGraph.h"
#include "data_structures/UnionFind.h"

namespace graph_algorithms {
	
    template<typename Vertex>
    struct Edge {
        Vertex v1;
        Vertex v2;

        Edge(const Vertex& u, const Vertex& v) : v1(u), v2(v) {}
    };

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


}//namespace graph_algorithms