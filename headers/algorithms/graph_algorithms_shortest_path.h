#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "graph/IGraph.h"
#include <optional>
#include <concepts>
#include "data_structures/PriorityQueue.h"
#include "algorithms/graph_algorithms_common.h"


// graph_algorithms_shortest_paht.h
// Алгоритмы поиска кратчайшего пути:
// - Дейкстры (Dijkstra)
// - Беллмана-Форда (Bellman Ford)
namespace graph_algorithms {
    
    namespace { // анонимный неймспейс — содержит служебные функции       
        
        //служебная функция для поиска минимума меток (алгоритм Дейкстры)
        template<typename Distance = double>
            requires std::is_arithmetic_v<Distance>
        std::optional<size_t> getMin(std::vector<Distance>& labels, std::vector<bool>& visited) {
            std::optional<size_t> min_idx = std::nullopt;
            size_t size = labels.size();
            for (size_t i = 0; i != labels.size(); ++i) {
                if (visited[i] == true) continue;
                if (min_idx.has_value()) {
                    if (labels[i] < labels[min_idx.value()])
                        min_idx = i;

                }
                else min_idx = i;
            }
            return min_idx;
        }
    } // anonymos namespace

    //Алгоритм Дейкстры для поиска кратчайших путей во взвешенном графе
    // Алгоритм возвращает набор ребер дерева кратчайших путеи или 
    // std::nullopt - в следующих случаях: граф пуст (нет вершин), стартовая вершина не принадлежит графу, 
    // граф содержит рёбра с отрицательным весом (алгоритм Дейкстры не работает),
    // граф несвязный (не все вершины достижимы из стартовой)
    template<template<graph::Comparable, typename, bool> class Graph,
        graph::Comparable Vertex,
        typename EdgeInfo, //данные о ребре - должны уметь конвертироваться в Distance
        bool Directed, typename Distance = double> //дополнительнный тип для расстояний, должен быть арифметическим
        requires std::is_arithmetic_v<Distance>&& std::is_convertible_v<EdgeInfo, Distance>
        std::optional<std::vector<Edge<Vertex>>> dijkstra(Graph<Vertex, EdgeInfo, Directed>& graph,
            Vertex start, Distance infinity = std::numeric_limits<Distance>::max()) { //значение "бесконечности"     
        
        //Проверка на пустой граф
        size_t v_count = graph.vertexCount();
        if (v_count == 0) {
            return std::nullopt;
        }

        //Получаем вершины и отображение на индекс
        std::vector<Vertex> vertices = graph.getVertices();
        std::map<Vertex, size_t> vertex_to_index;
        for (size_t i = 0; i < v_count; ++i) {
            vertex_to_index[vertices[i]] = i;
        }

        //Проверяем наличие стартовой вершины
        if (vertex_to_index.find(start) == vertex_to_index.end()) {
            return std::nullopt;
        }

        size_t start_idx = vertex_to_index[start];

        //Инициализация
        std::vector<Distance> distances(v_count, infinity);
        std::vector<bool> visited(v_count, false);
        std::vector<std::optional<Vertex>> parents(v_count, std::nullopt);

        // Приоритетная очередь: храним пары (расстояние, индекс_вершины)        
        struct QueueItem {
            Distance distance;
            size_t vertex_index;            
        };
        
        auto min_heap_cmp = [](const QueueItem& a, const QueueItem& b) {
            return a.distance < b.distance;
            };        
        PriorityQueue<QueueItem, decltype(min_heap_cmp)> pq(min_heap_cmp);        

        distances[start_idx] = 0;
        pq.push({ 0, start_idx });

        //Основной цикл Дейкстры       
        while (!pq.empty()) {
            QueueItem current = pq.top();
            pq.pop();
            size_t current_idx = current.vertex_index;

            // Пропускаем устаревшие записи
            if (visited[current_idx] || current.distance > distances[current_idx]) {
                continue;
            }

            visited[current_idx] = true;
            Vertex current_vertex = vertices[current_idx];

            std::vector<Vertex> neighbors = graph.getNeighbors(current_vertex);

            for (const auto& neighbor : neighbors) {         
                // Пропускаем петли
                if (current_vertex == neighbor) {
                    continue;
                }

                auto edge_info_opt = graph.getEdgeInfo(current_vertex, neighbor);
                if (!edge_info_opt.has_value()) {
                    continue;
                }

                Distance edge_weight = static_cast<Distance>(edge_info_opt.value());

                // Проверка на отрицательный вес
                if (edge_weight < 0) {
                    return std::nullopt; // Алгоритм Дейкстры не работает с отрицательными весами
                }

                size_t neighbor_idx = vertex_to_index[neighbor];

                // Релаксация ребра
                if (!visited[neighbor_idx] &&
                    distances[current_idx] != infinity &&
                    distances[current_idx] + edge_weight < distances[neighbor_idx]) {

                    distances[neighbor_idx] = distances[current_idx] + edge_weight;
                    parents[neighbor_idx] = current_vertex;
                    pq.push({ distances[neighbor_idx], neighbor_idx });
                }
            }
        }
        
        //Проверка, что все вершины достижимы
        for (size_t i = 0; i < v_count; ++i) {
            if (distances[i] == infinity) {
                return std::nullopt; // Есть недостижимые вершины
            }
        }        

        //Восстановление рёбер дерева кратчайших путей
        std::vector<Edge<Vertex>> result;
        result.reserve(v_count - 1);

        for (size_t i = 0; i < v_count; ++i) {
            if (parents[i].has_value()) {
                result.emplace_back(parents[i].value(), vertices[i]);
            }
        }

        return result;        
    }   


    //Алгоритм Беллмана-Форда для поиска кратчайших путей во взвешенном графе
    // Алгоритм возвращает набор ребер дерева кратчайших путеи или 
    //std::nullopt - в следующих случаях: граф пуст (нет вершин), стартовая вершина не принадлежит графу,
    //граф содержит отрицательный цикл, достижимый из стартовой вершины, граф несвязный 
    // (не все вершины достижимы из стартовой) 
    //в отличие от Дейкстры, работает с отрицательными весами, но не работает с отрицательными циклами
    //(для неориентированных графов все веса должны быть неотрицательными, иначе 
    // алгоритм обнаружит отрицательный цикл и вернёт nullopt)
    template<template<graph::Comparable, typename, bool> class Graph,
        graph::Comparable Vertex,
        typename EdgeInfo, //данные о ребре - должны уметь конвертироваться в Distance
        bool Directed,
        typename Distance = double>
        requires std::is_arithmetic_v<Distance>&& std::is_convertible_v<EdgeInfo, Distance>
        std::optional<std::vector<Edge<Vertex>>> bellmanFord(Graph<Vertex, EdgeInfo, Directed>& graph,
            Vertex start,  Distance infinity = std::numeric_limits<Distance>::max()) {
        
        //Проверка на пустой граф
        size_t v_count = graph.vertexCount();
        if (v_count == 0) {
            return std::nullopt;
        }

        //Получаем вершины и отображение на индекс
        std::vector<Vertex> vertices = graph.getVertices();
        std::map<Vertex, size_t> vertex_to_index;
        for (size_t i = 0; i < v_count; ++i) {
            vertex_to_index[vertices[i]] = i;
        }

        //Проверяем наличие стартовой вершины
        if (vertex_to_index.find(start) == vertex_to_index.end()) {
            return std::nullopt;
        }

        size_t start_idx = vertex_to_index[start];

        //Инициализация
        std::vector<Distance> distances(v_count, infinity);
        std::vector<std::optional<Vertex>> parents(v_count, std::nullopt);

        distances[start_idx] = 0;

        //Собираем все ребра для удобства
        struct RawEdge {
            size_t from_idx;
            size_t to_idx;
            Distance weight;
            Vertex from_vertex;
            Vertex to_vertex;
        };
        
        std::vector<RawEdge> all_edges;
        all_edges.reserve(Directed ? graph.edgeCount() : graph.edgeCount() * 2);
        auto edges = graph.getEdges();
        for (const auto& [from, to, info] : edges) {
            // Пропускаем петли
            if (from == to) {
                continue;
            }
            auto from_it = vertex_to_index.find(from);
            auto to_it = vertex_to_index.find(to);
          
            Distance weight = static_cast<Distance>(info);
            size_t from_idx = from_it->second;
            size_t to_idx = to_it->second;

            // Добавляем прямое ребро
            all_edges.push_back({ from_idx, to_idx, weight, from, to });

            // Для неориентированного графа добавляем обратное ребро
            if constexpr (!Directed) {
                all_edges.push_back({ to_idx, from_idx, weight, to, from });
            }
        }
        size_t e_count = v_count - 1;
        //Основной цикл: V-1 проходов релаксации
        for (size_t i = 0; i != e_count; ++i) {
            bool updated = false;

            for (const auto& edge : all_edges) {
                if (distances[edge.from_idx] != infinity &&
                    distances[edge.from_idx] + edge.weight < distances[edge.to_idx]) {

                    distances[edge.to_idx] = distances[edge.from_idx] + edge.weight;
                    parents[edge.to_idx] = edge.from_vertex;
                    updated = true;
                }
            }
            // Ранний выход, если ничего не изменилось
            if (!updated) {
                break;
            }
        }

        //Проверка на отрицательные циклы (ещё один проход)
        for (const auto& edge : all_edges) {
            if (distances[edge.from_idx] != infinity &&
                distances[edge.from_idx] + edge.weight < distances[edge.to_idx]) {
                // Найден отрицательный цикл, достижимый из стартовой вершины
                return std::nullopt;
            }
        }

        //Проверка, что все вершины достижимы
        for (size_t i = 0; i < v_count; ++i) {
            if (distances[i] == infinity) {
                return std::nullopt; // Есть недостижимые вершины
            }
        }

        //Восстановление ребер дерева кратчайших путей
        std::vector<Edge<Vertex>> result;
        result.reserve(e_count);

        for (size_t i = 0; i < v_count; ++i) {
            if (parents[i].has_value()) {
                result.emplace_back(parents[i].value(), vertices[i]);
            }
        }
        return result;
    }

}//namespace graph_algorithms