#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "IGraph.h"

template <typename Vertex, typename EdgeInfo = bool>
class GraphEnumeration : public IGraph<Vertex, EdgeInfo> {
private:
    std::set<Vertex> vertices_;
    // Для ребер нужно хранить: (from, to) -> info
    std::map<std::pair<Vertex, Vertex>, EdgeInfo> edges_;

public:
    bool addVertex(const Vertex& v) override {
        return vertices_.insert(v).second;
    }

    bool removeVertex(const Vertex& v) override {
        if (!vertices_.erase(v)) return false;

        // Удаляем все ребра, связанные с этой вершиной
        auto it = edges_.begin();
        while (it != edges_.end()) {
            if (it->first.first == v || it->first.second == v) {
                it = edges_.erase(it);
            }
            else {
                ++it;
            }
        }
        return true;
    }

    bool hasVertex(const Vertex& v) const override {
        return vertices_.count(v) > 0;
    }

    std::vector<Vertex> getVertices() const override {
        return { vertices_.begin(), vertices_.end() };
    }

    size_t vertexCount() const override {
        return vertices_.size();
    }

    bool addEdge(const Vertex& from, const Vertex& to, const EdgeInfo& info = EdgeInfo()) override {
        // Проверяем, что вершины существуют
        if (!hasVertex(from) || !hasVertex(to)) return false;

        auto edge = std::make_pair(from, to);
        return edges_.emplace(edge, info).second;
    }

    bool removeEdge(const Vertex& from, const Vertex& to) override {
        return edges_.erase({ from, to }) > 0;
    }

    bool hasEdge(const Vertex& from, const Vertex& to) const override {
        return edges_.count({ from, to }) > 0;
    }

    EdgeInfo getEdgeInfo(const Vertex& from, const Vertex& to) const override {
        auto it = edges_.find({ from, to });
        if (it != edges_.end()) {
            return it->second;
        }
        return EdgeInfo(); // Возвращаем значение по умолчанию
    }

    std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> getEdges() const override {
        std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> result;
        for (const auto& [edge, info] : edges_) {
            result.emplace_back(edge.first, edge.second, info);
        }
        return result;
    }

    size_t edgeCount() const override {
        return edges_.size();
    }

    std::vector<Vertex> getNeighbors(const Vertex& v) const override {
        std::vector<Vertex> neighbors;
        for (const auto& [edge, _] : edges_) {
            if (edge.first == v) {
                neighbors.push_back(edge.second);
            }
            // Для неориентированного графа добавляем и обратные ребра
            if (edge.second == v) {
                neighbors.push_back(edge.first);
            }
        }
        return neighbors;
    }

    int degree(const Vertex& v) const override {
        int deg = 0;
        for (const auto& [edge, _] : edges_) {
            if (edge.first == v || edge.second == v) {
                deg++;
            }
        }
        return deg;
    }
};