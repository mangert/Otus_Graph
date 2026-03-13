#pragma once
#include <vector>
#include <map>
#include <string>
#include "IGraph.h"

namespace graph {
    
    template <typename Vertex, typename EdgeInfo, bool Directed = false>
    class GraphAdjMatrix : public IGraph<Vertex, EdgeInfo> {

    public:
        //---------- Констукторы ---------------//
        GraphAdjMatrix() = default;
        GraphAdjMatrix(const GraphAdjMatrix&) = default;
        GraphAdjMatrix(GraphAdjMatrix&&) = default;

        GraphAdjMatrix& operator= (const GraphAdjMatrix&) = default;
        GraphAdjMatrix& operator= (GraphAdjMatrix&&) = default;
        ~GraphAdjMatrix() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            if (vertex_to_index.count(v)) return false; //одинаковые вершины не добавляем

            size_t new_index = matrix.size();
            vertex_to_index[v] = new_index;
            index_to_vertex.push_back(v);

            // Расширяем матрицу
            for (auto& row : matrix) {
                row.push_back(std::nullopt);
            }
            matrix.emplace_back(new_index + 1, std::nullopt);

            return true;
        }

        bool removeVertex(const Vertex& v) override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return false;

            size_t idx = it->second;
            size_t last_idx = matrix.size() - 1;
            Vertex last_vertex = index_to_vertex[last_idx];

            // Если удаляем не последнюю вершину - нужно переставить
            if (idx != last_idx) {
                // Перемещаем последнюю вершину на место удаляемой
                index_to_vertex[idx] = last_vertex;
                vertex_to_index[last_vertex] = idx;

                // Переставляем строки и столбцы в матрице
                matrix[idx] = std::move(matrix[last_idx]);
                for (auto& row : matrix) {
                    row[idx] = row[last_idx];
                }
            }

            // Удаляем последнюю строку и столбец
            matrix.pop_back();
            for (auto& row : matrix) {
                row.pop_back();
            }

            index_to_vertex.pop_back();
            vertex_to_index.erase(v);

            return true;
        }

        bool hasVertex(const Vertex& v) const override {
            return vertex_to_index.count(v) > 0;
        }

        std::vector<Vertex> getVertices() const override {
            return { index_to_vertex.begin(), index_to_vertex.end() };
        }

        size_t vertexCount() const override {
            return matrix.size();
        }

        //--------- Работа с ребрами --------------//
        bool addEdge(const Vertex& from, const Vertex& to, const EdgeInfo& info = EdgeInfo()) override {
            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return false;
            }

            matrix[it_from->second][it_to->second] = info;
            if constexpr (!Directed)
                matrix[it_to->second][it_from->second] = info; // неориентированный

            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return false;
            }

            bool removed = false;

            if (matrix[it_from->second][it_to->second].has_value()) {
                matrix[it_from->second][it_to->second] = std::nullopt;
                removed = true;
            }

            if constexpr (!Directed) {
                if (matrix[it_to->second][it_from->second].has_value()) {
                    matrix[it_to->second][it_from->second] = std::nullopt;
                    removed = true;
                }
            }

            return removed;
        }

        bool hasEdge(const Vertex& from, const Vertex& to) const override {
            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return false;
            }

            if (matrix[it_from->second][it_to->second].has_value()) {
                return true;
            }

            if constexpr (!Directed) {
                // Для неориентированного графа проверяем обратное направление
                return matrix[it_to->second][it_from->second].has_value();
            }

            return false;
        }

        std::optional<EdgeInfo> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return std::nullopt;
            }

            if (matrix[it_from->second][it_to->second].has_value()) {
                return matrix[it_from->second][it_to->second];
            }

            if constexpr (!Directed) {
                return matrix[it_to->second][it_from->second];
            }

            return std::nullopt;
        }

        std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> result;

            for (size_t i = 0; i < matrix.size(); ++i) {
                size_t j;
                if constexpr (!Directed) j = i + 1;
                else j = 0;
                for (; j < matrix.size(); ++j) {
                    if (matrix[i][j].has_value()) {
                        result.emplace_back(index_to_vertex[i],
                            index_to_vertex[j],
                            matrix[i][j].value());
                    }
                }
            }
            return result;
        }

        size_t edgeCount() const override {
            size_t count = 0;
            for (size_t i = 0; i < matrix.size(); ++i) {
                size_t j;
                if constexpr (!Directed) j = i + 1;
                else j = 0;
                for (; j < matrix.size(); ++j) {
                    if (matrix[i][j].has_value()) count++;
                }
            }
            return count;
        }

        //--------- Другие операции --------------//    
        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) {
                return {};
            }

            size_t idx = it->second;
            std::vector<Vertex> neighbors;

            for (size_t j = 0; j < matrix.size(); ++j) {
                if (matrix[idx][j].has_value()) {
                    neighbors.emplace_back(index_to_vertex[j]);
                }
            }
            return neighbors;
        }

        size_t degree(const Vertex& v) const override {
            if constexpr (Directed) {
                return countOut(v) + countIn(v);
            }
            else {
                return countOut(v); // в неориентированном графе - одинаковые
            }
        }

        //эти методы доступны только для ориентированных графов
        size_t outDegree(const Vertex& v) const requires (Directed) {
            return countOut(v);
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            return countIn(v);
        }

    private:
        size_t countOut(const Vertex& v) const {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t idx = it->second;
            size_t deg = 0;
            for (size_t j = 0; j < matrix.size(); ++j) {
                if (matrix[idx][j].has_value()) ++deg;
            }
            return deg;
        }

        size_t countIn(const Vertex& v) const {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t idx = it->second;
            size_t deg = 0;
            for (size_t i = 0; i < matrix.size(); ++i) {
                if (matrix[i][idx].has_value()) ++deg;
            }
            return deg;
        }

    private:
        std::map<Vertex, size_t> vertex_to_index;
        std::vector<Vertex> index_to_vertex;
        std::vector<std::vector<std::optional<EdgeInfo>>> matrix;

    };
}//namespace graph