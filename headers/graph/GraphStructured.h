#pragma once
#include <vector>
#include <map>
#include <list>
#include <string>
#include "IGraph.h"

namespace graph {
    template <Comparable Vertex, bool Directed = false>
    class GraphStructured : public IGraph<Vertex, bool> {
    
    public:
        //---------- Констукторы ---------------//
        GraphStructured() = default;
        GraphStructured(const GraphStructured&) = default;
        GraphStructured(GraphStructured&&) = default;

        GraphStructured& operator=(const GraphStructured&) = default;
        GraphStructured& operator=(GraphStructured&&) = default;
        ~GraphStructured() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            if (headers.count(v)) return false;  // вершина уже есть

            // Новая вершина с пустым списком
            headers[v] = { edges.end(), 0 };
            return true;
        }

        bool removeVertex(const Vertex& v) override {
            auto it_v = headers.find(v);
            if (it_v == headers.end()) return false;

            auto& [iter, size] = it_v->second;

            // 1. Удаляем ВСЕ ребра, где v участвует
            // Сначала собираем все вершины, с которыми v связано (в любую сторону)
            std::set<Vertex> all_neighbors;

            // Исходящие ребра (из v)
            auto it = iter;
            for (size_t i = 0; i < size; ++i) {
                if (it == edges.end()) break;
                all_neighbors.insert(*it);
                ++it;
            }

            // Входящие ребра (в v) - нужно пройти по всем спискам
            for (const auto& [u, data] : headers) {
                if (u == v) continue;
                const auto& [u_iter, u_size] = data;
                auto it_u = u_iter;
                for (size_t i = 0; i < u_size; ++i) {
                    if (it_u == edges.end()) break;
                    if (*it_u == v) {
                        all_neighbors.insert(u);
                        break;
                    }
                    ++it_u;
                }
            }

            // 2. Удаляем все ребра с этими соседями (в обе стороны)
            for (const auto& neighbor : all_neighbors) {
                removeEdge(v, neighbor);    // исходящее (если есть)
                removeEdge(neighbor, v);    // входящее (если есть)
            }

            // 3. Удаляем вершину из оглавления
            headers.erase(v);

            return true;
        }

        bool hasVertex(const Vertex& v) const {            
            return headers.count(v) > 0;
        }
        
        std::vector<Vertex> getVertices() const override {
            std::vector<Vertex> result;
            for (const auto& [v, _] : headers) {
                result.push_back(v);
            }
            return result;
        }

        size_t vertexCount() const override {
            return headers.size();
        }

        //--------- Работа с ребрами --------------//
        bool addEdge(const Vertex& from, const Vertex& to, const bool& info = true) override {
            auto it_from = headers.find(from);
            auto it_to = headers.find(to);
            if (it_from == headers.end() || it_to == headers.end()) {
                return false;  // одна из вершин не существует
            }

            if (hasEdge(from, to)) return false;  // ребро уже есть

            auto& [from_iter, from_size] = it_from->second;

            // Находим позицию для вставки (сохраняем сортировку списков)
            auto pos = from_iter;
            for (size_t i = 0; i != from_size; ++i) {
                if (pos == edges.end() || *pos > to) break;                
                ++pos;
            }

            // Вставляем ребро
            auto inserted = edges.insert(pos, to);

            // Если это было первое ребро, обновляем итератор начала
            if (from_size == 0) {
                from_iter = inserted;
            }

            // Увеличиваем размер списка
            ++from_size;

            if constexpr (!Directed) {
                auto& [to_iter, to_size] = it_to->second;

                // Аналогично для обратного направления
                pos = to_iter;
                for (size_t i = 0; i < to_size; ++i) {
                    if (pos == edges.end() || *pos > from) break;                    
                    ++pos;
                }
                
                inserted = edges.insert(pos, from);

                if (to_size == 0) {
                    to_iter = inserted;
                }

                ++to_size;
            }

            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it_from = headers.find(from);
            if (it_from == headers.end()) return false;

            auto& [from_iter, from_size] = it_from->second;

            // Ищем ребро в списке from
            auto it = from_iter;
            auto prev = it;
            for (size_t i = 0; i != from_size; ++i) {
                if (it == edges.end()) break;
                if (*it == to) {
                    // Удаляем
                    --from_size;
                    if (it == from_iter) {
                        // Удаляем первый элемент - нужно обновить итератор начала
                        auto next = std::next(it);
                        from_iter = (!from_size) ? edges.end() : next;                        
                    }
                    edges.erase(it);                    

                    if constexpr (!Directed) {
                        // Удаляем обратное направление
                        auto it_to = headers.find(to);
                        if (it_to != headers.end()) {
                            auto& [to_iter, to_size] = it_to->second;

                            auto it2 = to_iter;
                            for (size_t j = 0; j != to_size; ++j) {
                                if (it2 == edges.end()) break;                                
                                if (*it2 == from) {
                                    --to_size;
                                    if (it2 == to_iter) {
                                        auto next = std::next(it2);
                                        to_iter = (!to_size) ? edges.end() : next;                                        
                                    }
                                    edges.erase(it2);
                                    
                                    break;
                                }
                                ++it2;
                            }
                        }
                    }

                    return true;
                }
                if (*it > to) break;  // дальше искать не имеет смысла
                prev = it;
                ++it;
            }

            return false;
        }

        bool hasEdge(const Vertex& from, const Vertex& to) const override {
            auto it_from = headers.find(from);
            if (it_from == headers.end()) return false;

            const auto& [iter, size] = it_from->second;

            auto it = iter;
            for (size_t i = 0; i < size; ++i) {
                if (it == edges.end()) break;
                if (*it == to) return true;
                if (*it > to) break;  // список отсортирован
                ++it;
            }

            return false;
        }

        std::optional<bool> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            if (hasEdge(from, to)) {
                return true;
            }
            return std::nullopt;
        }

        std::vector<std::tuple<Vertex, Vertex, bool>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, bool>> result;

            for (const auto& [v, data] : headers) {
                const auto& [iter, size] = data;
                auto it = iter;
                for (size_t i = 0; i < size; ++i) {
                    if (it == edges.end()) break;

                    // Для неориентированных графов добавляем только if v < *it
                    if constexpr (Directed) {
                        result.emplace_back(v, *it, true);
                    }
                    else {
                        if (v < *it || v == *it) {
                            result.emplace_back(v, *it, true);
                        }
                    }
                    ++it;
                }
            }

            return result;
        }

        size_t edgeCount() const override {
            if constexpr (Directed) {
                return edges.size();
            }
            else {
                return edges.size() / 2;
            }
        }

        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            auto it_v = headers.find(v);
            if (it_v == headers.end()) return {};

            const auto& [iter, size] = it_v->second;

            std::vector<Vertex> neighbors;
            auto it = iter;
            for (size_t i = 0; i < size; ++i) {
                if (it == edges.end()) break;
                neighbors.push_back(*it);
                ++it;
            }

            return neighbors;
        }

        size_t degree(const Vertex& v) const override {
            auto it_v = headers.find(v);
            if (it_v == headers.end()) return 0;

            const auto& [_, size] = it_v->second;

            if constexpr (!Directed) {
                return size;
            }
            else {
                // Для ориентированных нужно учесть входящие
                size_t in_deg = 0;
                for (const auto& [u, data] : headers) {
                    if (u == v) continue;
                    const auto& [iter, sz] = data;
                    auto it = iter;
                    for (size_t i = 0; i < sz; ++i) {
                        if (it == edges.end()) break;
                        if (*it == v) ++in_deg;
                        ++it;
                    }
                }
                return size + in_deg;
            }
        }

        size_t outDegree(const Vertex& v) const requires (Directed) {
            auto it_v = headers.find(v);
            if (it_v == headers.end()) return 0;
            return it_v->second.second;
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            size_t deg = 0;
            for (const auto& [u, data] : headers) {
                if (u == v) continue;
                const auto& [iter, sz] = data;
                auto it = iter;
                for (size_t i = 0; i < sz; ++i) {
                    if (it == edges.end()) break;
                    if (*it == v) ++deg;
                    ++it;
                }
            }
            return deg;
        }       

    private:
        // Единый линейный список всех ребер
        std::list<Vertex> edges;

        // Оглавление: для каждой вершины храним итератор на начало её списка и размер
        // Если список пуст, итератор = edges_.end(), размер = 0
        std::map<Vertex, std::pair<typename std::list<Vertex>::iterator, size_t>> headers;
    };
}//namespace graph