#pragma once

// graph_algorithms_common.h
// Содержит общую для разных алгоритмов структуру вывода результата
namespace graph_algorithms {	
    
    template<typename Vertex>
    //структура для представления ребра
    struct Edge {
        Vertex v1;
        Vertex v2;
        Edge() = default;
        Edge(const Vertex& u, const Vertex& v) : v1(u), v2(v) {}
    };

}//namespace graph_algorithms