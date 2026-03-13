#pragma once
#include <vector>
#include <map>
#include <string>
#include "IGraph.h"

namespace graph {
    template <Comparable Vertex, bool Directed = false>
    class GraphAdjArray : public IGraph<Vertex, bool> { //информация о ребрах в этом представлении не сохраняется

    public:
        //---------- Констукторы ---------------//
        GraphAdjArray() = default;
        GraphAdjArray(const GraphAdjArray&) = default;
        GraphAdjArray(GraphAdjArray&&) = default;

        GraphAdjArray& operator=(const GraphAdjArray&) = default;
        GraphAdjArray& operator=(GraphAdjArray&&) = default;
        ~GraphAdjArray() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            if (vertex_to_index.count(v)) return false;

            size_t new_idx = vectors.size();
            vertex_to_index[v] = new_idx;
            index_to_vertex.push_back(v);

            // Добавляем пустую строку
            vectors.emplace_back();

            return true;
        }

        bool removeVertex(const Vertex& v) override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return false;

            size_t idx = it->second;
            size_t last_idx = vectors.size() - 1;

            // Удаляем все вхождения v из других строк
            for (auto& row : vectors) {
                for (auto it_cell = row.begin(); it_cell != row.end(); ) {
                    if (*it_cell == v) {
                        it_cell = row.erase(it_cell);
                    }
                    else {
                        ++it_cell;
                    }
                }
            }
            // Перестановка при удалении не последней вершины
            if (idx != last_idx) {
                Vertex last_v = index_to_vertex[last_idx];
                vertex_to_index[last_v] = idx;
                index_to_vertex[idx] = std::move(index_to_vertex[last_idx]);
                vectors[idx] = std::move(vectors[last_idx]);
            }

            vertex_to_index.erase(v);
            index_to_vertex.pop_back();
            vectors.pop_back();

            return true;
        }

        bool hasVertex(const Vertex& v) const override {
            return vertex_to_index.count(v) > 0;
        }
        virtual std::vector<Vertex> getVertices() const override {
            return { index_to_vertex.begin(), index_to_vertex.end() };
        }
        virtual size_t vertexCount() const override {
            return vectors.size();
        }

        //--------- Работа с ребрами --------------//
        bool addEdge(const Vertex& from, const Vertex& to, const bool& info = true) override {
            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return false;
            }

            if (hasEdge(from, to)) return false;

            // Добавляем в вектор from
            auto& row_from = vectors[it_from->second];
            row_from.push_back(to);  // push_back понятнее чем emplace_back для простых типов

            if constexpr (!Directed) {
                // Для неориентированных добавляем обратное направление
                auto& row_to = vectors[it_to->second];
                row_to.push_back(from);
            }
            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it_from = vertex_to_index.find(from);
            if (it_from == vertex_to_index.end()) return false;

            auto& row_from = vectors[it_from->second];
            for (size_t j = 0; j < row_from.size(); ++j) {
                if (row_from[j] == to) {
                    // Быстрое удаление через swap с последним
                    row_from[j] = row_from.back();
                    row_from.pop_back();

                    if constexpr (!Directed) {
                        auto it_to = vertex_to_index.find(to);
                        if (it_to != vertex_to_index.end()) {
                            auto& row_to = vectors[it_to->second];
                            for (size_t k = 0; k < row_to.size(); ++k) {
                                if (row_to[k] == from) {
                                    row_to[k] = row_to.back();
                                    row_to.pop_back();
                                    break;
                                }
                            }
                        }
                    }
                    return true;
                }
            }
            return false;
        }
        
        bool hasEdge(const Vertex& from, const Vertex& to) const override {
            // Ищем в векторах
            auto it_from = vertex_to_index.find(from);
            if (it_from == vertex_to_index.end()) return false;

            for (const auto& cell : vectors[it_from->second]) {
                if (cell == to) {
                    return true;
                }
            }

            return false;
        }

        std::optional<bool> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            // В этом представлении информация о ребре - только факт существования
            if (hasEdge(from, to)) {
                return true; // ребро есть
            }
            return std::nullopt; // ребра нет
        }

        std::vector<std::tuple<Vertex, Vertex, bool>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, bool>> result;

            for (size_t i = 0; i < vectors.size(); ++i) {
                for (const auto& cell : vectors[i]) {                    
                    result.emplace_back(index_to_vertex[i], cell, true);                    
                }
            }

            return result;
        }

        size_t edgeCount() const override {

            size_t count = 0;

            for (size_t i = 0; i < vectors.size(); ++i) {
                count += vectors[i].size();                
            }
            if constexpr (!Directed) count /= 2;

            return count;
        }
        //--------- Другие операции --------------//
        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return {};

            std::vector<Vertex> neighbors;
            for (const auto& cell : vectors[it->second]) {                
                neighbors.push_back(cell);
                
            }
            if constexpr (Directed) {
                // Для ориентированных графов это уже исходящие соседи
            }
            return neighbors;
        }

        size_t degree(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t deg = vectors[it->second].size();
            
            //если граф ориентированный, то надо собрать еще "концы"
            if (Directed) {
                for (const auto& row : vectors) {
                    for (const auto& cell : row) {
                        if (cell == v) {
                            ++deg;
                            break; // одно ребро на строку максимум
                        }
                    }
                }
            }

            return deg;
        }

        size_t outDegree(const Vertex& v) const requires (Directed) {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t deg = vectors[it->second].size();
            
            return deg;
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            // Для inDegree нужно искать по всем строкам
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t deg = 0;
            for (const auto& row : vectors) {
                for (const auto& cell : row) {
                    if (cell == v) {
                        ++deg;
                        break; // одно ребро на строку максимум
                    }
                }
            }
            return deg;
        }

    private:
        //вспомогательные индексы для вершин
        std::map<Vertex, size_t> vertex_to_index;
        std::vector<Vertex> index_to_vertex;

        // Собственно граф
        std::vector<std::vector<Vertex>> vectors;
        
    };
}
