#pragma once
#include <vector>
#include <list>
#include <map>
#include <string>
#include "IGraph.h"

namespace graph {
    template <Comparable Vertex, bool Directed = false>
    class GraphAdjList : public IGraph<Vertex, bool> { //информация о ребрах в этом представлении не сохраняется

    public:
        //---------- Констукторы ---------------//
        GraphAdjList() = default;
        GraphAdjList(const GraphAdjList&) = default;
        GraphAdjList(GraphAdjList&&) = default;

        GraphAdjList& operator=(const GraphAdjList&) = default;
        GraphAdjList& operator=(GraphAdjList&&) = default;
        ~GraphAdjList() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            if (vertex_to_index.count(v)) return false;

            size_t new_idx = lists.size();
            vertex_to_index[v] = new_idx;
            index_to_vertex.push_back(v);
            lists.emplace_back();  // пустой список

            return true;
        }

        bool removeVertex(const Vertex& v) override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return false;

            size_t idx = it->second;
            size_t last_idx = lists.size() - 1;

            // Удаляем все вхождения v из других списков
            for (auto& lst : lists) {
                lst.remove(v);  // list имеет метод remove!
            }

            // Перестановка при удалении не последней вершины
            if (idx != last_idx) {
                Vertex last_v = index_to_vertex[last_idx];
                vertex_to_index[last_v] = idx;
                index_to_vertex[idx] = std::move(index_to_vertex[last_idx]);
                lists[idx].swap(lists[last_idx]);  // эффективный обмен списками
            }

            vertex_to_index.erase(v);
            index_to_vertex.pop_back();
            lists.pop_back();

            return true;
        }

        bool hasVertex(const Vertex& v) const override {
            return vertex_to_index.count(v) > 0;
        }
        virtual std::vector<Vertex> getVertices() const override {
            return { index_to_vertex.begin(), index_to_vertex.end() };
        }
        virtual size_t vertexCount() const override {
            return lists.size();
        }

        //--------- Работа с ребрами --------------//
        bool addEdge(const Vertex& from, const Vertex& to, const bool& info = true) override {
            auto it_from = vertex_to_index.find(from);
            auto it_to = vertex_to_index.find(to);
            if (it_from == vertex_to_index.end() || it_to == vertex_to_index.end()) {
                return false;
            }

            if (hasEdge(from, to)) return false;

            lists[it_from->second].push_back(to);

            if constexpr (!Directed) {
                lists[it_to->second].push_back(from);
            }

            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it_from = vertex_to_index.find(from);
            if (it_from == vertex_to_index.end()) return false;

            auto& lst_from = lists[it_from->second];
            
            for (auto it = lst_from.begin(); it != lst_from.end(); ++it) {
                if (*it == to) {
                    lst_from.erase(it);

                    if constexpr (!Directed) {
                        auto it_to = vertex_to_index.find(to);
                        if (it_to != vertex_to_index.end()) {
                            auto& lst_to = lists[it_to->second];
                            for (auto it2 = lst_to.begin(); it2 != lst_to.end(); ++it2) {
                                if (*it2 == from) {
                                    lst_to.erase(it2);
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
            auto it_from = vertex_to_index.find(from);
            if (it_from == vertex_to_index.end()) return false;

            const auto& lst = lists[it_from->second];
            return std::find(lst.begin(), lst.end(), to) != lst.end();
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

            for (size_t i = 0; i < lists.size(); ++i) {
                for (const auto& to : lists[i]) {
                    // Чтобы не дублировать для неориентированных,
                    // добавляем только если from < to или граф ориентированный
                    if constexpr (Directed) {
                        result.emplace_back(index_to_vertex[i], to, true);
                    }
                    else {
                        if (index_to_vertex[i] < to) {
                            result.emplace_back(index_to_vertex[i], to, true);
                        }
                    }
                }
            }
            return result;
        }

        size_t edgeCount() const override {
            size_t count = 0;
            for (const auto& lst : lists) {
                count += lst.size();
            }
            if constexpr (!Directed) {
                count /= 2;
            }
            return count;
        }
        
        //--------- Другие операции --------------//    
        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return {};

            const auto& lst = lists[it->second];
            return { lst.begin(), lst.end() };
        }

        size_t degree(const Vertex& v) const override {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;

            size_t deg = lists[it->second].size();

            if constexpr (Directed) {
                // Добавляем входящие
                for (const auto& lst : lists) {
                    deg += std::count(lst.begin(), lst.end(), v);
                }
            }
            return deg;
        }

        size_t outDegree(const Vertex& v) const requires (Directed) {
            auto it = vertex_to_index.find(v);
            if (it == vertex_to_index.end()) return 0;
            return lists[it->second].size();
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            size_t deg = 0;
            for (const auto& lst : lists) {
                deg += std::count(lst.begin(), lst.end(), v);
            }
            return deg;
        }

    private:
        //вспомогательные индексы для вершин
        std::map<Vertex, size_t> vertex_to_index;
        std::vector<Vertex> index_to_vertex;

        // Собственно граф
        std::vector<std::list<Vertex>> lists;

    };
}

