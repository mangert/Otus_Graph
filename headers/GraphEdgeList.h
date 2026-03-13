#pragma once
#include <vector>
#include <map>
#include "IGraph.h"

namespace graph {
    
    template <typename Vertex, typename EdgeInfo, bool Directed = false>
    class GraphEdgeList : public IGraph<Vertex, EdgeInfo> {    

    public:
        //---------- Констукторы ---------------//
        GraphEdgeList() = default;
        GraphEdgeList(const GraphEdgeList&) = default;
        GraphEdgeList(GraphEdgeList&&) = default;

        GraphEdgeList& operator= (const GraphEdgeList&) = default;
        GraphEdgeList& operator= (GraphEdgeList&&) = default;
        ~GraphEdgeList() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            return vertices.insert(v).second;
        }

        bool removeVertex(const Vertex& v) override {
            if (!vertices.erase(v)) return false;

            // Удаляем все ребра с этой вершиной
            auto it = edges.begin();
            while (it != edges.end()) {
                if (it->from == v || it->to == v) {
                    edge_index.erase({ it->from, it->to });
                    if constexpr (!Directed) {
                        edge_index.erase({ it->to, it->from });
                    }
                    it = edges.erase(it);
                }
                else {
                    ++it;
                }
            }
            return true;
        }
        bool hasVertex(const Vertex& v) const override {
            return vertices.count(v) > 0;
        }
        std::vector<Vertex> getVertices() const override {
            return { vertices.begin(), vertices.end() };
        }
        size_t vertexCount() const override {
            return vertices.size();
        }

        //--------- Работа с ребрами --------------//
        bool addEdge(const Vertex& from, const Vertex& to, const EdgeInfo& info = EdgeInfo()) override {
            if (!hasVertex(from) || !hasVertex(to)) return false;

            // Проверка на существование ребра
            if (hasEdge(from, to)) return false;

            edge_index[{from, to}] = edges.size();
            if constexpr (!Directed) {
                // Для неориентированных храним оба направления в индексе
                edge_index[{to, from}] = edges.size();
            }

            edges.push_back({ from, to, info });
            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it = edge_index.find({ from, to });
            if (it == edge_index.end()) {
                if constexpr (!Directed) {
                    it = edge_index.find({ to, from });
                    if (it == edge_index.end()) return false;
                }
                else {
                    return false;
                }
            }

            size_t idx = it->second;
            size_t last_idx = edges.size() - 1;

            // Если удаляем не последнее ребро - переставляем
            if (idx != last_idx) {
                const auto& last_edge = edges[last_idx];
                edges[idx] = last_edge;

                // Обновляем индексы для перемещенного ребра
                edge_index[{last_edge.from, last_edge.to}] = idx;
                if constexpr (!Directed) {
                    edge_index[{last_edge.to, last_edge.from}] = idx;
                }
            }

            // Удаляем старое ребро из индекса
            edge_index.erase({ from, to });
            if constexpr (!Directed) {
                edge_index.erase({ to, from });
            }

            edges.pop_back();
            return true;
        }

        bool hasEdge(const Vertex& from, const Vertex& to) const override {
            if constexpr (Directed) {
                return edge_index.count({ from, to }) > 0;
            }
            else {
                return edge_index.count({ from, to }) > 0 ||
                    edge_index.count({ to, from }) > 0;
            }
        }

        std::optional<EdgeInfo> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            auto it = edge_index.find({ from, to });
            if (it != edge_index.end()) {
                return edges[it->second].info;
            }

            if constexpr (!Directed) {
                it = edge_index.find({ to, from });
                if (it != edge_index.end()) {
                    return edges[it->second].info;
                }
            }

            return std::nullopt;
        }

        std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> result;
            for (const auto& e : edges) {
                result.emplace_back(e.from, e.to, e.info);
            }
            return result;
        }
        
        size_t edgeCount() const override {
            return edges.size();
        }

        //--------- Другие операции --------------//
        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            if (!hasVertex(v)) return {};

            std::set<Vertex> neighbors;
            for (const auto& e : edges) {
                if constexpr (Directed) {
                    if (e.from == v) neighbors.insert(e.to);                    
                }
                else {
                    if (e.from == v) neighbors.insert(e.to);
                    if (e.to == v) neighbors.insert(e.from);
                }
            }

            return { neighbors.begin(), neighbors.end() };
        }

        size_t degree(const Vertex& v) const override {
            if (!hasVertex(v)) return 0;

            size_t deg = 0;
            for (const auto& e : edges) {
                if constexpr (Directed) {
                    if (e.from == v) ++deg;
                    if (e.to == v) ++deg; // для полной степени
                }
                else {
                    if (e.from == v) ++deg;
                    if (e.to == v) ++deg;
                }
            }
            return deg;
        }

        // Для ориентированных графов
        size_t outDegree(const Vertex& v) const requires (Directed) {
            if (!hasVertex(v)) return 0;

            size_t deg = 0;
            for (const auto& e : edges) {
                if (e.from == v) ++deg;
            }
            return deg;
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            if (!hasVertex(v)) return 0;

            size_t deg = 0;
            for (const auto& e : edges) {
                if (e.to == v) ++deg;
            }
            return deg;
        }    

    private:
        std::set<Vertex> vertices;

        struct Edge {
            Vertex from;
            Vertex to;
            EdgeInfo info;
        };
        std::vector<Edge> edges;

        // Для быстрого поиска
        std::map<std::pair<Vertex, Vertex>, size_t> edge_index;
    };

}//namespace graph