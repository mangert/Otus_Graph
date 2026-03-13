#pragma once
#include <vector>
#include <map>
#include <set>
#include "IGraph.h"

namespace graph {
    
    template <Comparable Vertex, typename EdgeInfo, bool Directed = false>
    class GraphIncMatrix : public IGraph<Vertex, EdgeInfo> {

    public:
        //---------- Констукторы ---------------//
        GraphIncMatrix() = default;
        GraphIncMatrix(const GraphIncMatrix&) = default;
        GraphIncMatrix(GraphIncMatrix&&) = default;

        GraphIncMatrix& operator= (const GraphIncMatrix&) = default;
        GraphIncMatrix& operator= (GraphIncMatrix&&) = default;
        ~GraphIncMatrix() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            if (vertex_to_index.count(v)) return false;

            size_t new_idx = index_to_vertex.size();
            vertex_to_index[v] = new_idx;
            index_to_vertex.emplace_back(v);

            // Добавляем столбец для новой вершины во все существующие ребра
            for (auto& row : matrix) {
                row.push_back(0);
            }

            return true;
        }

        bool removeVertex(const Vertex& v) override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return false;

            size_t v_idx = it->second;

            // Собираем все ребра, связанные с v
            std::vector<std::pair<Vertex, Vertex>> edges_to_remove;

            for (size_t e_idx = 0; e_idx < matrix.size(); ++e_idx) {
                if (matrix[e_idx][v_idx] != 0) {
                    const auto& edge = index_to_edge[e_idx];
                    edges_to_remove.emplace_back(edge.from, edge.to);
                }
            }

            // Удаляем собранные ребра
            for (const auto& [from, to] : edges_to_remove) {
                removeEdge(from, to);  // Этот метод сам учтет направленность
            }

            // Теперь удаляем вершину (столбец)
            size_t last_v_idx = index_to_vertex.size() - 1;
            if (v_idx != last_v_idx) {
                Vertex last_vertex = index_to_vertex[last_v_idx];
                vertex_to_index[last_vertex] = v_idx;
                index_to_vertex[v_idx] = last_vertex;

                // Переставляем столбцы
                for (auto& row : matrix) {
                    row[v_idx] = row[last_v_idx];
                }
            }

            // Удаляем последний столбец
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

        virtual size_t vertexCount() const override {
            return vertex_to_index.size();
        }

        //--------- Работа с ребрами --------------//
        bool addEdge(const Vertex& from, const Vertex& to, const EdgeInfo& info = EdgeInfo()) override {
            // Проверяем существование вершин
            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return false;
            }

            // Проверяем, нет ли уже такого ребра
            auto edge_key = std::make_pair(from, to);
            if (edge_to_index.count(edge_key)) return false;

            // Добавляем новое ребро
            size_t edge_idx = index_to_edge.size();
            edge_to_index[edge_key] = edge_idx;
            index_to_edge.push_back({ from, to, info });

            // Добавляем новую строку в матрицу
            std::vector<int> new_row(vertexCount(), 0);

            // Заполняем в зависимости от типа графа
            if constexpr (Directed) {
                if (from == to) { // петля
                    new_row[it_from->second] = 2;
                }
                else {
                    new_row[it_from->second] = -1; // начало
                    new_row[it_to->second] = 1;    // конец
                }
            }
            else {
                // Неориентированный
                ++new_row[it_from->second] = 1;
                ++new_row[it_to->second] = 1;
            }

            matrix.push_back(std::move(new_row));
            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            if constexpr (Directed) {
                return removeEdgeInternal(from, to);
            }
            else {
                // Для неориентированного пробуем оба порядка
                if (removeEdgeInternal(from, to)) return true;
                if (removeEdgeInternal(to, from)) return true;
                return false;
            }
        }

        bool hasEdge(const Vertex& from, const Vertex& to) const override {
            return edge_to_index.count({ from, to }) > 0;
        }

        std::optional<EdgeInfo> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            auto it = edge_to_index.find({ from, to });
            if (it == edge_to_index.end()) return std::nullopt;
            return index_to_edge[it->second].info;
        }

        std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> result;
            for (const auto& [key, idx] : edge_to_index) {
                const auto& e = index_to_edge[idx];
                result.emplace_back(e.from, e.to, e.info);
            }
            return result;
        }

        size_t edgeCount() const override {
            return matrix.size();
        }

        //--------- Другие операции --------------//    
        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return {};

            size_t v_idx = it->second;
            std::set<Vertex> neighbors; // используем set для уникальности

            for (size_t e_idx = 0; e_idx < matrix.size(); ++e_idx) {
                int val = matrix[e_idx][v_idx];

                if constexpr (Directed) {
                    if (val == -1) { // v - начало ребра -> ищем конец
                        for (size_t other = 0; other < vertexCount(); ++other) {
                            if (matrix[e_idx][other] == 1) {
                                neighbors.insert(index_to_vertex[other]);
                            }
                        }
                    }
                    else if (val == 1) { // v - конец ребра -> ищем начало
                        for (size_t other = 0; other < vertexCount(); ++other) {
                            if (matrix[e_idx][other] == -1) {
                                neighbors.insert(index_to_vertex[other]);
                            }
                        }
                    }
                    else if (val == 2) { // петля
                        neighbors.insert(v); // добавляем саму себя
                    }
                }
                else {
                    // Неориентированный - любое положительное значение значит инцидентность
                    if (val > 0) {
                        for (size_t other = 0; other < vertexCount(); ++other) {
                            if (other != v_idx && matrix[e_idx][other] > 0) {
                                neighbors.insert(index_to_vertex[other]);
                            }
                        }
                    }
                }
            }

            return { neighbors.begin(), neighbors.end() };
        }

        size_t degree(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t v_idx = it->second;
            size_t deg = 0;

            for (size_t e_idx = 0; e_idx < matrix.size(); ++e_idx) {
                int val = matrix[e_idx][v_idx];

                if constexpr (Directed) {
                    if (val != 0) ++deg; // любое ненулевое значение
                    // Для петель считаем 2
                    if (val == 2) ++deg; // еще раз, если петля считается дважды
                }
                else {
                    if (val == 1) ++deg;
                    else if (val == 2) deg += 2; // петля дает степень 2
                }
            }

            return deg;
        }

        // Для ориентированных графов
        size_t outDegree(const Vertex& v) const requires (Directed) {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t v_idx = it->second;
            size_t deg = 0;

            for (size_t e_idx = 0; e_idx < matrix.size(); ++e_idx) {
                if (matrix[e_idx][v_idx] == -1) ++deg; // начало ребра
                else if (matrix[e_idx][v_idx] == 2) ++deg; // петля (и out, и in)
            }

            return deg;
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t v_idx = it->second;
            size_t deg = 0;

            for (size_t e_idx = 0; e_idx < matrix.size(); ++e_idx) {
                if (matrix[e_idx][v_idx] == 1) ++deg; // конец ребра
                else if (matrix[e_idx][v_idx] == 2) ++deg; // петля
            }

            return deg;
        }

    private:
        // Вспомогательный метод, который удаляет ребро по точному совпадению (from, to)
        bool removeEdgeInternal(const Vertex& from, const Vertex& to) {
            auto it = edge_to_index.find({ from, to });
            if (it == edge_to_index.end()) return false;

            size_t e_idx = it->second;
            size_t last_idx = index_to_edge.size() - 1;

            // Если удаляем не последнее ребро - переставляем
            if (e_idx != last_idx) {
                const auto& last_edge = index_to_edge[last_idx];
                edge_to_index[{last_edge.from, last_edge.to}] = e_idx;
                index_to_edge[e_idx] = last_edge;
                matrix[e_idx] = std::move(matrix[last_idx]);
            }

            // Удаляем последнее ребро
            edge_to_index.erase({ from, to });
            matrix.pop_back();
            index_to_edge.pop_back();

            return true;
        }

    private:
        std::map<Vertex, size_t> vertex_to_index;
        std::vector<Vertex> index_to_vertex;

        //Отображение для ребер
        struct EdgeDescriptor {
            Vertex from;
            Vertex to;
            EdgeInfo info;
        };
        std::map<std::pair<Vertex, Vertex>, size_t> edge_to_index;
        std::vector<EdgeDescriptor> index_to_edge;

        // Матрица инцидентности: строки = ребра, столбцы = вершины
        // Значения: для неориентированного - 0/1/2 (петля)
        //           для ориентированного - -1/0/1/2 (петля)
        std::vector<std::vector<int>> matrix;  // int, потому что нужны -1, 0, 1, 2
    };
} //namespace graph