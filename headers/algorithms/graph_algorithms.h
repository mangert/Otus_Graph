#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "graph/IGraph.h"
#include <optional>
#include "data_structures/UnionFind.h"
#include "data_structures/PriorityQueue.h"

// graph_algorithms_.h
// Алгоритмы поиска кратчайшего пути:
// - Дейкстры (Deikstra)

namespace graph_algorithms {

    template<typename Vertex>
    //структура для представления ребра
    struct Edge {
        Vertex v1;
        Vertex v2;
        Edge() = default;
        Edge(const Vertex& u, const Vertex& v) : v1(u), v2(v) {}
    };

    std::optional<size_t> getMin(std::vector<size_t>& labels, std::vector<bool>& visited)
    {
        std::optional<size_t> min_idx = std::nullopt;
        size_t size = labels.size();
        for (size_t i = 0; i != labels.size(); ++i) {
            if (visited[i] == true) continue;
            if (min_idx.has_value())
                if (labels[i] < labels[min_idx.value()]){
                    min_idx = i;
                }            
        }
        return min_idx;
    }

    //Алгоритм Дейкстры
    template<template<graph::Comparable, typename, bool> class Graph,
        graph::Comparable Vertex,
        typename EdgeInfo>
        requires std::is_convertible_v<EdgeInfo, int>  // тут подумать - может, преобразование сделать?
    std::optional<std::vector<Edge<Vertex>>> deikstra(Graph<Vertex, EdgeInfo, false>& graph, Vertex start) {        
        size_t v_count = graph.vertexCount();
        if (v_count == 0) {
            return {};
        }
        std::vector<Edge> result;
        result.reserve(v_count - 1);
        std::vector<Vertex> vertices = graph.getVertices();
        std::map<Vertex, size_t> vertex_to_index;
        for (size_t i = 0; i != v_count; ++i) {
            vertex_to_index[vertices[i]] = i;
        }        
        size_t start_idx = vertex_to_index.at(start);
        std::vector<EdgeInfo> labels(v_count, std::max<EdgeInfo>-1);
        labels[start_idx] = 0;
        std::vector<bool> visited(v_count, false);
        
        while (true) {            
            std::optional <size_t> current_idx = getMin(labels, visited);
            if (!current_idx.has_value())
                break;
            std::vector<Vertex> neighbors = graph.getNeighbors(current);
            Vertex current = vertices[current_idx.value()];
            EdgeInfo weight = labels[current_idx.value()];
            for (auto neighbor : neighbors) {                
                EdgeInfo fullweight = weight + graph.getEdgeInfo(current, neighbor);
                if (fullweight < labels[vertex_to_index[neighbor]])
                    labels[vertex_to_index[neighbor]] = fullweight;
            }
            size_t min_idx = 0;
            for (size_t i = 1; i != neighbors.size(); ++i) {
                if (neighbors[i] < neighbors[min_idx])
                    min_idx = i;
            }
            result.emplace_back(Edge(current, neighbors[min_idx]);
        }
        return result;
    }


}//namespace graph_algorithms