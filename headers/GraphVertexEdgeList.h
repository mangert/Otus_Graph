#pragma once
#include <vector>
#include <map>
#include <list>
#include <string>
#include "IGraph.h"

namespace graph {
    template <Comparable Vertex, typename EdgeInfo, bool Directed = false>
    class GraphVertexEdgeList : public IGraph<Vertex, EdgeInfo> {
    
    public:
        //---------- Констукторы ---------------//
        GraphVertexEdgeList() = default;
        GraphVertexEdgeList(const GraphVertexEdgeList&) = default;
        GraphVertexEdgeList(GraphVertexEdgeList&&) = default;

        GraphVertexEdgeList& operator=(const GraphVertexEdgeList&) = default;
        GraphVertexEdgeList& operator=(GraphVertexEdgeList&&) = default;
        ~ GraphVertexEdgeList() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            if (vertex_index.count(v)) return false;

            // Добавляем вершину в список
            vertices.emplace_back(v);

            // Сохраняем итератор на неё
            auto it = vertices.end();
            --it;  // итератор на только что добавленный элемент
            vertex_index[v] = it;

            return true;
        }

        bool removeVertex(const Vertex& v) override {
            auto it_idx = vertex_index.find(v);
            if (it_idx == vertex_index.end()) return false;

            auto vertex_it = it_idx->second;

            // Удаляем все ребра, связанные с v (из других вершин)
            for (auto& node : vertices) {
                if (node.vertex == v) continue;  // пропускаем саму удаляемую вершину

                auto& edges = node.edges;
                for (auto it = edges.begin(); it != edges.end(); ) {
                    if (it->to == v) {
                        it = edges.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }

            // Удаляем вершину
            vertices.erase(vertex_it);
            vertex_index.erase(v);

            return true;
        }

        bool hasVertex(const Vertex& v) const override {
            return vertex_index.count(v) > 0;
        }

        std::vector<Vertex> getVertices() const override {
            std::vector<Vertex> result;
            for (const auto& node : vertices) {
                result.push_back(node.vertex);
            }
            return result;
        }

        size_t vertexCount() const override {
            return vertices.size();
        }

        //--------- Работа с ребрами --------------//
        bool addEdge(const Vertex& from, const Vertex& to, const EdgeInfo& info = EdgeInfo()) override {
            auto it_from = vertex_index.find(from);
            auto it_to = vertex_index.find(to);

            if (it_from == vertex_index.end() || it_to == vertex_index.end()) {
                return false;
            }

            if (hasEdge(from, to)) return false;

            // Добавляем ребро
            it_from->second->edges.push_back({ info, to });

            if constexpr (!Directed) {
                it_to->second->edges.push_back({ info, from });
            }

            return true;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            auto it_from = vertex_index.find(from);
            if (it_from == vertex_index.end()) return false;

            auto& edges_from = it_from->second->edges;

            for (auto it = edges_from.begin(); it != edges_from.end(); ++it) {
                if (it->to == to) {
                    edges_from.erase(it);

                    if constexpr (!Directed) {
                        auto it_to = vertex_index.find(to);
                        if (it_to != vertex_index.end()) {
                            auto& edges_to = it_to->second->edges;
                            for (auto it2 = edges_to.begin(); it2 != edges_to.end(); ++it2) {
                                if (it2->to == from) {
                                    edges_to.erase(it2);
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
            auto it_from = vertex_index.find(from);
            if (it_from == vertex_index.end()) return false;

            for (const auto& e : it_from->second->edges) {
                if (e.to == to) return true;
            }

            return false;
        }

        std::optional<EdgeInfo> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            auto it_from = vertex_index.find(from);
            if (it_from == vertex_index.end()) return std::nullopt;

            for (const auto& e : it_from->second->edges) {
                if (e.to == to) return e.info;
            }

            return std::nullopt;
        }

        std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> result;

            for (const auto& node : vertices) {
                for (const auto& e : node.edges) {
                    
                    if constexpr (Directed) {
                        result.emplace_back(node.vertex, e.to, e.info);
                    }
                    else {// Для неориентированных добавляем только если from <= to (петли) 
                        if (node.vertex < e.to || node.vertex == e.to) {
                            result.emplace_back(node.vertex, e.to, e.info);
                        }
                    }
                }
            }

            return result;
        }

        size_t edgeCount() const override {
            size_t count = 0;
            for (const auto& node : vertices) {
                count += node.edges.size();
            }

            if constexpr (!Directed) {
                count /= 2;
            }

            return count;
        }

        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            auto it = vertex_index.find(v);
            if (it == vertex_index.end()) return {};

            std::vector<Vertex> neighbors;
            for (const auto& e : it->second->edges) {
                neighbors.push_back(e.to);
            }

            return neighbors;
        }

        size_t degree(const Vertex& v) const override {
            auto it = vertex_index.find(v);
            if (it == vertex_index.end()) return 0;

            size_t deg = it->second->edges.size();

            if constexpr (Directed) {
                // Добавляем входящие
                for (const auto& node : vertices) {
                    if (node.vertex == v) continue;
                    for (const auto& e : node.edges) {
                        if (e.to == v) ++deg;
                    }
                }
            }
            return deg;
        }

        size_t outDegree(const Vertex& v) const requires (Directed) {
            auto it = vertex_index.find(v);
            if (it == vertex_index.end()) return 0;
            
            return it->second->edges.size();
        }

        size_t inDegree(const Vertex& v) const requires (Directed) {
            size_t deg = 0;
            for (const auto& node : vertices) {
                if (node.vertex == v) continue;
                for (const auto& e : node.edges) {
                    if (e.to == v) ++deg;
                }
            }
            return deg;
        }    
    
    private:
        //структруры узлов
        struct Edge {
            EdgeInfo info;
            Vertex to;
        };
        struct VertexNode {
            Vertex vertex;
            std::list<Edge> edges;
        };
        //собственно список вершин
        std::list<VertexNode> vertices;
        //индекс вершин
        std::map<Vertex, typename std::list<VertexNode>::iterator> vertex_index;
    };
}//namespace graph