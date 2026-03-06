#pragma once
#include <vector>

// Базовый интерфейс 
template <typename Vertex, typename EdgeInfo>
class IGraph {
public:
    
    virtual ~IGraph() = default;

    // Работа с вершинами
    virtual bool addVertex(const Vertex& v) = 0;
    virtual bool removeVertex(const Vertex& v) = 0;
    virtual bool hasVertex(const Vertex& v) const = 0;
    virtual std::vector<Vertex> getVertices() const = 0;
    virtual size_t vertexCount() const = 0;

    // Работа с ребрами
    virtual bool addEdge(const Vertex& from, const Vertex& to, const EdgeInfo& info = EdgeInfo()) = 0;
    virtual bool removeEdge(const Vertex& from, const Vertex& to) = 0;
    virtual bool hasEdge(const Vertex& from, const Vertex& to) const = 0;
    virtual EdgeInfo getEdgeInfo(const Vertex& from, const Vertex& to) const = 0;
    virtual std::vector<std::tuple<Vertex, Vertex, EdgeInfo>> getEdges() const = 0;
    virtual size_t edgeCount() const = 0;

    // Для алгоритмов (пригодятся в следующих работах)
    virtual std::vector<Vertex> getNeighbors(const Vertex& v) const = 0;
    virtual int degree(const Vertex& v) const = 0;
};
