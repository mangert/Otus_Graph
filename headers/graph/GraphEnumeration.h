#pragma once
#include <vector>
#include <set>
#include <map>
#include <optional>
#include <string>
#include "IGraph.h"

namespace graph {
    
    template <Comparable Vertex, typename EdgeInfo = bool, bool Directed = false>
    class GraphEnumeration : public IGraph<Vertex, EdgeInfo> {

    public:
        //---------- Констукторы ---------------//
        GraphEnumeration() = default;
        GraphEnumeration(const GraphEnumeration&) = default;
        GraphEnumeration(GraphEnumeration&&) = default;

        GraphEnumeration& operator=(const GraphEnumeration&) = default;
        GraphEnumeration& operator=(GraphEnumeration&&) = default;
        ~GraphEnumeration() = default;

        //--------- Работа с вершинами --------------//
        bool addVertex(const Vertex& v) override {
            return vertices.insert(v).second;
        }

        bool removeVertex(const Vertex& v) override {
            if (!vertices.erase(v)) return false;

            // Удаляем все ребра, связанные с этой вершиной
            auto it = edges.begin();
            while (it != edges.end()) {
                if (it->first.first == v || it->first.second == v) {
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
            // Проверяем, что вершины существуют
            if (!hasVertex(from) || !hasVertex(to)) return false;

            auto edge = std::make_pair(from, to);
            return edges.emplace(edge, info).second;
        }

        bool removeEdge(const Vertex& from, const Vertex& to) override {
            return edges.erase({ from, to }) > 0;
        }

        bool hasEdge(const Vertex& from, const Vertex& to) const override {
            //прямое направление
            if (edges.count({ from, to }) > 0)
                return true;
            
            // Для неориентированного пробуем обратное
            if constexpr (!Directed) {
                return edges.count({ to, from }) > 0;
            }
        }

        std::optional<EdgeInfo> getEdgeInfo(const Vertex& from, const Vertex& to) const override {
            auto it = edges.find({ from, to });
            if (it != edges.end()) {

                return it->second;
            }
            return std::nullopt; // нет такого ребра
        }

        std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> getEdges() const override {
            std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> result;
            for (const auto& [edge, info] : edges) {
                result.emplace_back(edge.first, edge.second, info);
            }
            return result;
        }

        size_t edgeCount() const override {
            return edges.size();
        }

        //--------- Другие операции --------------//

        std::vector<Vertex> getNeighbors(const Vertex& v) const override {
            std::vector<Vertex> neighbors;
            for (const auto& [edge, _] : edges) {
                if (edge.first == v) {
                    neighbors.emplace_back(edge.second);
                }
                // Для неориентированного графа добавляем и обратные ребра
                if constexpr (!Directed) {
                    if (edge.second == v) {
                        neighbors.emplace_back(edge.first);
                    }
                }

            }
            return neighbors;
        }

        size_t outDegree(const Vertex& v) const
            requires (Directed) {

            return countOut(v);
        }

        size_t inDegree(const Vertex& v) const
            requires (Directed) {

            return countIn(v);
        }

        //эти методы доступны только для ориентированных графов
        size_t degree(const Vertex& v) const override {

            return countOut(v) + countIn(v);
        }
    private:

        size_t countOut(const Vertex& v) const {
            size_t deg = 0;
            for (const auto& [edge, _] : edges) {
                if (edge.first == v) {
                    ++deg;
                }
            }
            return deg;
        }

        size_t countIn(const Vertex& v) const {
            size_t deg = 0;
            for (const auto& [edge, _] : edges) {
                if (edge.second == v) {
                    ++deg;
                }
            }
            return deg;
        }

    private:

        std::set<Vertex> vertices;
        std::map<std::pair<Vertex, Vertex>, EdgeInfo> edges;

    };
}//namespace graph