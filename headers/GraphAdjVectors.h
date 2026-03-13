#pragma once
#include <vector>
#include <map>
#include <string>
#include "IGraph.h"

namespace graph {
    template <typename Vertex, bool Directed = false>
    class GraphAdjVectors : public IGraph<Vertex, bool> { //информация о ребрах в этом представлении не сохраняется
    
    public:
        //---------- Констукторы ---------------//
        GraphAdjVectors() = default;
        GraphAdjVectors(const GraphAdjVectors&) = default;
        GraphAdjVectors(GraphAdjVectors&&) = default;

        GraphAdjVectors& operator=(const GraphAdjVectors&) = default;
        GraphAdjVectors& operator=(GraphAdjVectors&&) = default;
        ~GraphAdjVectors() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            if (vertex_to_index.count(v)) return false;

            size_t new_idx = vectors.size();
            vertex_to_index[v] = new_idx;
            index_to_vertex.push_back(v);
            
            // Добавляем строку текущей длины max_degree, заполненную nullopt
            vectors.emplace_back(max_degree, std::nullopt);

            return true;
        }

        bool removeVertex(const Vertex& v) override {
            auto it = vertex_to_index.find(v);  
            if (it == vertex_to_index.end()) return false;

            size_t idx = it->second;
            size_t last_idx = vectors.size() - 1;

            if (idx != last_idx) {
                Vertex last_v = index_to_vertex[last_idx];
                vertex_to_index[last_v] = idx;  // обновляем индекс последней вершины
                index_to_vertex[idx] = std::move(index_to_vertex[last_idx]);
                vectors[idx] = std::move(vectors[last_idx]);
            }

            vertex_to_index.erase(v);
            index_to_vertex.pop_back();
            vectors.pop_back();

            // Удаляем ссылки на v из оставшихся строк
            for (auto& row : vectors) {
                for (auto& cell : row) {
                    if (cell.has_value() && cell.value() == v) {
                        cell = std::nullopt;
                    }
                }
            }
            update_max_degree_after_remove();
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
            // info игнорируется, так как в этом представлении ребро либо есть, либо нет

            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return false;
            }

            // Проверяем, нет ли уже ребра
            if (hasEdge(from, to)) return false;

            // Добавляем в вектор from
            auto& row_from = vectors[it_from->second];

            // Ищем свободное место
            bool placed = false;
            for (auto& cell : row_from) {
                if (!cell.has_value()) {
                    cell = to;
                    placed = true;
                    break;
                }
            }

            if (!placed) {
                // Нет свободного места - расширяем все строки
                size_t new_degree = max_degree + 1;
                for (auto& row : vectors) {
                    row.resize(new_degree);
                }
                max_degree = new_degree;
                // Теперь добавляем в новое место
                row_from[max_degree - 1] = to;
            }

            if constexpr (!Directed) {
                // Для неориентированных добавляем обратное направление
                auto& row_to = vectors[it_to->second];

                placed = false;
                for (auto& cell : row_to) {
                    if (!cell.has_value()) {
                        cell = from;
                        placed = true;
                        break;
                    }
                }

                if (!placed) {
                    // Опять расширяем (если не расширили выше)
                    if (max_degree == row_to.size()) {
                        size_t new_degree = max_degree + 1;
                        for (auto& row : vectors) {
                            row.resize(new_degree);
                        }
                        max_degree = new_degree;
                    }
                    row_to[max_degree - 1] = from;
                }
            }
            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it_from = vertex_to_index.find(from);
            if (it_from == vertex_to_index.end()) return false;

            auto& row_from = vectors[it_from->second];
            for (auto& cell : row_from) {
                if (cell.has_value() && cell.value() == to) {
                    cell = std::nullopt;
                    
                    if constexpr (!Directed) {
                        // Для неориентированных удаляем и обратное
                        auto it_to = vertex_to_index.find(to);
                        if (it_to != vertex_to_index.end()) {
                            auto& row_to = vectors[it_to->second];
                            for (auto& c : row_to) {
                                if (c.has_value() && c.value() == from) {
                                    c = std::nullopt;
                                    break;
                                }
                            }
                        }                        
                    }
                    update_max_degree_after_remove();
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
                if (cell.has_value() && cell.value() == to) {                    
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
                    if (cell.has_value()) {
                        result.emplace_back(index_to_vertex[i], cell.value(), true);
                    }
                }
            }

            return result;
        }        
        
        size_t edgeCount() const override {
            
            size_t count = 0;
            
            for (size_t i = 0; i < vectors.size(); ++i) {
                for (const auto& cell : vectors[i]) {
                    if (cell.has_value()) {
                        ++count;
                    }
                }
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
                if (cell.has_value()) {
                    neighbors.push_back(cell.value());
                }
            }
            if constexpr (Directed) {
                // Для ориентированных графов это уже исходящие соседи
            }
            return neighbors;
        }

        size_t degree(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t deg = 0;
            for (const auto& cell : vectors[it->second]) {
                if (cell.has_value()) ++deg;
            }
            //если граф ориентированный, то надо собрать еще "концы"
            if (Directed) {
                for (const auto& row : vectors) {
                    for (const auto& cell : row) {
                        if (cell.has_value() && cell.value() == v) {
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

            size_t deg = 0;
            for (const auto& cell : vectors[it->second]) {
                if (cell.has_value()) ++deg;
            }

            return deg;
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            // Для inDegree нужно искать по всем строкам
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t deg = 0;
            for (const auto& row : vectors) {
                for (const auto& cell : row) {
                    if (cell.has_value() && cell.value() == v) {
                        ++deg;
                        break; // одно ребро на строку максимум
                    }
                }
            }
            return deg;
        }

        void shrink_to_fit() {
            // Находим новую максимальную степень
            size_t new_max = 0;
            for (const auto& row : vectors) {
                size_t count = 0;
                for (const auto& cell : row) {
                    if (cell.has_value()) count++;
                }
                new_max = std::max(new_max, count);
            }

            // Сжимаем все строки
            for (auto& row : vectors) {
                // Уплотняем: собираем все значения в начало
                std::vector<std::optional<Vertex>> new_row;
                new_row.reserve(new_max);

                for (auto& cell : row) {
                    if (cell.has_value()) {
                        new_row.push_back(std::move(cell));
                    }
                }
                new_row.resize(new_max); // добиваем nullopt до нужной длины

                row = std::move(new_row);
            }

            max_degree = new_max;
        }

    private:       

        void update_max_degree_after_remove() {
            // Пересчитываем с нуля после удаления
            max_degree = 0;
            for (const auto& row : vectors) {
                size_t count = 0;
                for (const auto& cell : row) {
                    if (cell.has_value()) ++count;
                }
                max_degree = std::max(max_degree, count);
            }
        }

    private:
        std::map<Vertex, size_t> vertex_to_index;
        std::vector<Vertex> index_to_vertex;

        // Матрица V × max_degree, хранит вершины или nullopt (пусто)
        std::vector<std::vector<std::optional<Vertex>>> vectors;
        size_t max_degree = 0;        
    };
}