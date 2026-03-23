#pragma once
#include <iostream>
#include <vector>
#include "graph/IGraph.h"
#include "data_structures/Queue.h"
#include "data_structures/Stack.h"

#include <optional>
#include <map>

namespace graph_algorithms {

    namespace { // анонимный неймспейс — содержит внутренние имплементации

        //имплементация алгоритма Демукрона
        template<typename Graph, graph::Comparable Vertex>
        std::optional<std::vector<std::vector<Vertex>>> demukronImpl(const Graph& graph) {
            size_t v_count = graph.vertexCount();
            if (v_count == 0) {
                return std::vector<std::vector<Vertex>>{};
            }

            std::vector<Vertex> vertices = graph.getVertices();
            std::map<Vertex, size_t> vertex_to_index;
            for (size_t i = 0; i < v_count; ++i) {
                vertex_to_index[vertices[i]] = i;
            }

            std::vector<size_t> in_degrees(v_count);
            for (size_t i = 0; i < v_count; ++i) {
                in_degrees[i] = graph.inDegree(vertices[i]);
            }

            std::vector<std::vector<Vertex>> result;
            Queue<size_t> queue;

            // Первый уровень
            std::vector<Vertex> first_level;
            for (size_t i = 0; i < v_count; ++i) {
                if (in_degrees[i] == 0) {
                    first_level.push_back(vertices[i]);
                    queue.push(i);
                }
            }

            if (first_level.empty()) {
                return std::nullopt;
            }
            result.push_back(first_level);

            // Обрабатываем остальные уровни
            size_t level_size = first_level.size();

            while (level_size > 0) {
                std::vector<Vertex> next_level;

                for (size_t i = 0; i < level_size; ++i) {
                    size_t curr_idx = queue.front();
                    queue.pop();

                    Vertex curr_vertex = vertices[curr_idx];
                    std::vector<Vertex> neighbors = graph.getNeighbors(curr_vertex);

                    for (const Vertex& neighbor : neighbors) {
                        size_t neighbor_idx = vertex_to_index[neighbor];
                        in_degrees[neighbor_idx]--;

                        if (in_degrees[neighbor_idx] == 0) {
                            next_level.push_back(neighbor);
                            queue.push(neighbor_idx);
                        }
                    }
                }

                if (next_level.empty()) {
                    break;
                }

                result.push_back(next_level);
                level_size = next_level.size();
            }

            // Проверка, все ли вершины обработаны
            size_t processed = 0;
            for (const auto& level : result) {
                processed += level.size();
            }

            if (processed != v_count) {
                return std::nullopt;
            }

            return result;
        }

        // ============================================================
        // ВСПОМОГАТЕЛЬНЫЕ СТРУКТУРЫ И ФУНКЦИИ
        // ============================================================

        enum class VertexState {
            none,   // не посещена
            seen,   // в процессе обхода (в стеке рекурсии)
            cplt    // полностью обработана
        };

        // Рекурсивный DFS для алгоритма Тарьяна
        template<typename Graph, typename Vertex>
        bool tarjanDfs(
            size_t v,
            const std::vector<Vertex>& vertices,
            const std::map<Vertex, size_t>& vertex_to_index,
            const Graph& graph,
            std::vector<VertexState>& state,
            Stack<Vertex>& result_stack) {
            state[v] = VertexState::seen;

            Vertex current_vertex = vertices[v];
            std::vector<Vertex> neighbors = graph.getNeighbors(current_vertex);

            for (const Vertex& neighbor : neighbors) {
                size_t neighbor_idx = vertex_to_index.at(neighbor);

                if (state[neighbor_idx] == VertexState::none) {
                    // Не посещена - рекурсивный вызов
                    if (!tarjanDfs(neighbor_idx, vertices, vertex_to_index,
                        graph, state, result_stack)) {
                        return false;  // нашли цикл
                    }
                }
                else if (state[neighbor_idx] == VertexState::seen) {
                    // Нашли обратное ребро — значит, есть цикл
                    return false;
                }
                // если state == cplt — пропускаем
            }

            state[v] = VertexState::cplt;
            result_stack.push(current_vertex);
            return true;
        }

        // ============================================================
        // ОСНОВНАЯ РЕАЛИЗАЦИЯ АЛГОРИТМА ТАРЬЯНА
        // ============================================================

        template<typename Graph, typename Vertex>
        std::optional<std::vector<Vertex>> tarjanImpl(const Graph& graph) {
            size_t v_count = graph.vertexCount();
            if (v_count == 0) {
                return std::vector<Vertex>{};
            }

            // Получаем вершины и создаем отображение
            std::vector<Vertex> vertices = graph.getVertices();

            // ОТЛАДКА: выводим все вершины
            std::cout << "Вершины графа: ";
            for (const auto& v : vertices) {
                std::cout << v << " ";
            }
            std::cout << std::endl;

            std::map<Vertex, size_t> vertex_to_index;
            for (size_t i = 0; i < v_count; ++i) {
                vertex_to_index[vertices[i]] = i;
            }

            std::vector<VertexState> state(v_count, VertexState::none);
            Stack<Vertex> result_stack;

            for (size_t i = 0; i != v_count; ++i) {
                if (state[i] == VertexState::none) {                    
                    if (!tarjanDfs(i, vertices, vertex_to_index, graph, state, result_stack)) {
                        return std::nullopt;
                    }
                }
            }

            std::vector<Vertex> result;
            result.reserve(v_count);
            while (!result_stack.empty()) {
                result.push_back(result_stack.top());
                result_stack.pop();
            }            

            return result;
        }
        
    } // anonymos namespace    

    //Алгоритм Демукрона (ярусная топологическая сортировка)
    // Специализация для трех шаблонных параметров (с EdgeInfo)
    template<template<graph::Comparable, typename, bool> class Graph,
        graph::Comparable Vertex,
        typename EdgeInfo>
    std::optional<std::vector<std::vector<Vertex>>> demukron(const Graph<Vertex, EdgeInfo, true>& graph) {
        return demukronImpl<Graph<Vertex, EdgeInfo, true>, Vertex>(graph);
    }

    // Специализация для двух шаблонных параметров (без EdgeInfo)
    template<template<graph::Comparable, bool> class Graph,
        graph::Comparable Vertex>
    std::optional<std::vector<std::vector<Vertex>>> demukron(const Graph<Vertex, true>& graph) {
        return demukronImpl<Graph<Vertex, true>, Vertex>(graph);
    }

    //Алгоритм Тарьяна
    // Специализация для трех шаблонных параметров (с EdgeInfo)
    template<template<graph::Comparable, typename, bool> class Graph,
        graph::Comparable Vertex,
        typename EdgeInfo>
    std::optional<std::vector<Vertex>> tarjan(const Graph<Vertex, EdgeInfo, true>& graph) {
        return tarjanImpl<Graph<Vertex, EdgeInfo, true>, Vertex>(graph);
    }

    // Специализация для двух шаблонных параметров (без EdgeInfo)
    template<template<graph::Comparable, bool> class Graph,
        graph::Comparable Vertex>
    std::optional<std::vector<Vertex>> tarjan(const Graph<Vertex, true>& graph) {
        return tarjanImpl<Graph<Vertex, true>, Vertex>(graph);
    }
} // namespace graph_algorithms
