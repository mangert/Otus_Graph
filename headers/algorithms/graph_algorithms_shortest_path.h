#pragma once
#include <iostream>
#include <vector>
#include <map>
#include "graph/IGraph.h"
#include <optional>
#include <concepts>
#include "data_structures/PriorityQueue.h"
#include "algorithms/graph_algorithms_common.h"


// graph_algorithms_.h
// Алгоритмы поиска кратчайшего пути:
// - Дейкстры (Dijkstra)
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
    // Алогорит возвращает набор ребер дерева кратчайших путеи или 
    // std::nullopt - в следующих случаях: граф пуст (нет вершин), стартовая вершина не принадлежит графу, 
    // граф содержит рёбра с отрицательным весом (алгоритм Дейкстры не работает),
    // граф несвязный (не все вершины достижимы из стартовой)
    template<template<graph::Comparable, typename, bool> class Graph,
        graph::Comparable Vertex,
        typename EdgeInfo, //данные о ребре - должны уметь конвертироваться в Distance
        bool Directed,
        typename Distance = double> //тип для расстояний, должен быть арифметическим
        requires std::is_arithmetic_v<Distance>&&
    std::is_convertible_v<EdgeInfo, Distance>
        std::optional<std::vector<Edge<Vertex>>> dijkstra(
            Graph<Vertex, EdgeInfo, Directed>& graph,
            Vertex start,
            Distance infinity = std::numeric_limits<Distance>::max()) { //значение "бесконечности"     
        
        // 1. Проверка на пустой граф
        size_t v_count = graph.vertexCount();
        if (v_count == 0) {
            return std::nullopt;
        }

        // 2. Получаем вершины и отображение на индекс
        std::vector<Vertex> vertices = graph.getVertices();
        std::map<Vertex, size_t> vertex_to_index;
        for (size_t i = 0; i < v_count; ++i) {
            vertex_to_index[vertices[i]] = i;
        }

        // 3. Проверяем наличие стартовой вершины
        if (vertex_to_index.find(start) == vertex_to_index.end()) {
            return std::nullopt;
        }

        size_t start_idx = vertex_to_index[start];

        // 4. Инициализация
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

        // 5. Основной цикл Дейкстры       

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
        
        // 7. Проверка, что все вершины достижимы
        for (size_t i = 0; i < v_count; ++i) {
            if (distances[i] == infinity) {
                return std::nullopt; // Есть недостижимые вершины
            }
        }        

        // 8. Восстановление рёбер дерева кратчайших путей
        std::vector<Edge<Vertex>> result;
        result.reserve(v_count - 1);

        for (size_t i = 0; i < v_count; ++i) {
            if (parents[i].has_value()) {
                result.emplace_back(parents[i].value(), vertices[i]);
            }
        }

        return result;        
    }


}//namespace graph_algorithms