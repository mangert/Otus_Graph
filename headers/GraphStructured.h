#pragma once
#include <vector>
#include <map>
#include <list>
#include <string>
#include "IGraph.h"

namespace graph {
    template <Comparable Vertex, bool Directed = false>
    class GraphStructured : public IGraph<Vertex, bool> {
    private:
        // ≈диный линейный список всех ребер
        std::list<Vertex> edges_;

        // ќглавление: дл€ каждой вершины храним итератор на начало еЄ списка и размер
        // ≈сли список пуст, итератор = edges_.end(), размер = 0
        std::map<Vertex, std::pair<typename std::list<Vertex>::iterator, size_t>> headers_;

    public:
        bool addVertex(const Vertex& v) override {
            if (headers_.count(v)) return false;  // вершина уже есть

            // Ќова€ вершина с пустым списком
            headers_[v] = { edges_.end(), 0 };
            return true;
        }

        bool addEdge(const Vertex& from, const Vertex& to, const bool& info = true) override {
            auto it_from = headers_.find(from);
            auto it_to = headers_.find(to);
            if (it_from == headers_.end() || it_to == headers_.end()) {
                return false;  // одна из вершин не существует
            }

            if (hasEdge(from, to)) return false;  // ребро уже есть

            auto& [from_iter, from_size] = it_from->second;

            // Ќаходим позицию дл€ вставки (сохран€ем сортировку списков)
            auto pos = from_iter;
            for (size_t i = 0; i < from_size; ++i) {
                if (pos == edges_.end() || *pos > to) break;
                if (*pos == to) return false;  // дополнительна€ проверка
                ++pos;
            }

            // ¬ставл€ем ребро
            auto inserted = edges_.insert(pos, to);

            // ≈сли это было первое ребро, обновл€ем итератор начала
            if (from_size == 0) {
                from_iter = inserted;
            }

            // ”величиваем размер списка
            ++from_size;

            if constexpr (!Directed) {
                auto& [to_iter, to_size] = it_to->second;

                // јналогично дл€ обратного направлени€
                pos = to_iter;
                for (size_t i = 0; i < to_size; ++i) {
                    if (pos == edges_.end() || *pos > from) break;
                    if (*pos == from) {
                        // “акого не должно быть из-за проверки hasEdge, но на вс€кий случай
                        --from_size;  // откатываем
                        edges_.erase(inserted);
                        return false;
                    }
                    ++pos;
                }

                inserted = edges_.insert(pos, from);

                if (to_size == 0) {
                    to_iter = inserted;
                }

                ++to_size;
            }

            return true;
        }

        bool hasEdge(const Vertex& from, const Vertex& to) const override {
            auto it_from = headers_.find(from);
            if (it_from == headers_.end()) return false;

            const auto& [iter, size] = it_from->second;

            auto it = iter;
            for (size_t i = 0; i < size; ++i) {
                if (it == edges_.end()) break;
                if (*it == to) return true;
                if (*it > to) break;  // список отсортирован
                ++it;
            }

            return false;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it_from = headers_.find(from);
            if (it_from == headers_.end()) return false;

            auto& [from_iter, from_size] = it_from->second;

            // »щем ребро в списке from
            auto it = from_iter;
            auto prev = it;
            for (size_t i = 0; i < from_size; ++i) {
                if (it == edges_.end()) break;
                if (*it == to) {
                    // ”дал€ем
                    if (it == from_iter) {
                        // ”дал€ем первый элемент - нужно обновить итератор начала
                        auto next = std::next(it);
                        from_iter = next;
                    }
                    edges_.erase(it);
                    --from_size;

                    if constexpr (!Directed) {
                        // ”дал€ем обратное направление
                        auto it_to = headers_.find(to);
                        if (it_to != headers_.end()) {
                            auto& [to_iter, to_size] = it_to->second;

                            auto it2 = to_iter;
                            for (size_t j = 0; j < to_size; ++j) {
                                if (it2 == edges_.end()) break;
                                if (*it2 == from) {
                                    if (it2 == to_iter) {
                                        auto next = std::next(it2);
                                        to_iter = next;
                                    }
                                    edges_.erase(it2);
                                    --to_size;
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

        bool removeVertex(const Vertex& v) override {
            auto it_v = headers_.find(v);
            if (it_v == headers_.end()) return false;

            const auto& [iter, size] = it_v->second;

            // ”дал€ем все ребра, св€занные с v
            if (size > 0) {
                // —обираем все вершины, смежные с v
                std::vector<Vertex> neighbors;
                auto it = iter;
                for (size_t i = 0; i < size; ++i) {
                    if (it == edges_.end()) break;
                    neighbors.push_back(*it);
                    ++it;
                }

                // ”дал€ем ребра в обратную сторону
                for (const auto& to : neighbors) {
                    removeEdge(to, v);  // удал€ем обратные ребра
                }

                // ”дал€ем ребра из v
                it = iter;
                for (size_t i = 0; i < size; ++i) {
                    auto next = std::next(it);
                    edges_.erase(it);
                    it = next;
                }
            }

            // ”дал€ем вершину из оглавлени€
            headers_.erase(v);

            return true;
        }

        std::vector<Vertex> getVertices() const override {
            std::vector<Vertex> result;
            for (const auto& [v, _] : headers_) {
                result.push_back(v);
            }
            return result;
        }

        size_t vertexCount() const override {
            return headers_.size();
        }

        std::vector<std::tuple<Vertex, Vertex, bool>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, bool>> result;

            for (const auto& [v, data] : headers_) {
                const auto& [iter, size] = data;
                auto it = iter;
                for (size_t i = 0; i < size; ++i) {
                    if (it == edges_.end()) break;

                    // ƒл€ неориентированных графов добавл€ем только if v < *it
                    if constexpr (Directed) {
                        result.emplace_back(v, *it, true);
                    }
                    else {
                        if (v < *it) {  // нужно operator< дл€ Vertex
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
                return edges_.size();
            }
            else {
                return edges_.size() / 2;
            }
        }

        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            auto it_v = headers_.find(v);
            if (it_v == headers_.end()) return {};

            const auto& [iter, size] = it_v->second;

            std::vector<Vertex> neighbors;
            auto it = iter;
            for (size_t i = 0; i < size; ++i) {
                if (it == edges_.end()) break;
                neighbors.push_back(*it);
                ++it;
            }

            return neighbors;
        }

        size_t degree(const Vertex& v) const override {
            auto it_v = headers_.find(v);
            if (it_v == headers_.end()) return 0;

            const auto& [_, size] = it_v->second;

            if constexpr (!Directed) {
                return size;
            }
            else {
                // ƒл€ ориентированных нужно учесть вход€щие
                size_t in_deg = 0;
                for (const auto& [u, data] : headers_) {
                    if (u == v) continue;
                    const auto& [iter, sz] = data;
                    auto it = iter;
                    for (size_t i = 0; i < sz; ++i) {
                        if (it == edges_.end()) break;
                        if (*it == v) ++in_deg;
                        ++it;
                    }
                }
                return size + in_deg;
            }
        }

        size_t outDegree(const Vertex& v) const requires (Directed) {
            auto it_v = headers_.find(v);
            if (it_v == headers_.end()) return 0;
            return it_v->second.second;
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            size_t deg = 0;
            for (const auto& [u, data] : headers_) {
                if (u == v) continue;
                const auto& [iter, sz] = data;
                auto it = iter;
                for (size_t i = 0; i < sz; ++i) {
                    if (it == edges_.end()) break;
                    if (*it == v) ++deg;
                    ++it;
                }
            }
            return deg;
        }

        bool hasVertex(const Vertex& v) const override {
            return headers_.count(v) > 0;
        }

        std::optional<bool> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            if (hasEdge(from, to)) {
                return true;
            }
            return std::nullopt;
        }
    };
}//namespace graph