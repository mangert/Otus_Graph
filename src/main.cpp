#include <iostream>
#include <vector>
#include "GraphEnumeration.h"
#include "GraphAdjMatrix.h"
#include "GraphIncMatrix.h"
#include "graph_factory.h"
#include "GraphEdgeList.h"
#include "GraphAdjVectors.h"
#include "GraphAdjArray.h"
#include "GraphAdjList.h"
#include "GraphStructured.h"
#include "GraphVertexEdgeList.h"
#include "test.cpp"



int main() {
	using namespace graph;
	
	std::cout << "=== TESTING UNDIRECTED GRAPHS ===\n";
	run_all_tests<true>();

	std::cout << "\n\n=== TESTING DIRECTED GRAPHS ===\n";
	run_all_tests<false>();

	//и так 9 раз
	//а потом еще 9 раз - для ориентированных
	
	/*std::cout << "hello" << std::endl;
	GraphIncMatrix<int, int> G;
	G.addVertex(1);
	G.addVertex(2);
	G.addEdge(1, 2, 3);
	auto v = G.getVertices();
	for (auto& ver : v)
		std::cout << ver << std::endl;
	auto edge = G.getEdgeInfo(1,2);
	std::cout << edge.value() << std::endl;
	std::cout << "--------------" << std::endl;
	// Неориентированный граф
	GraphAdjMatrix<int, bool, false> undirected;
	//undirected.inDegree(1); // ОШИБКА КОМПИЛЯЦИИ! requires не выполняется

	// Ориентированный граф
	graph::GraphEnumeration<int, bool, true> directed;
	/*directed.addVertex(1);
	directed.addVertex(2);
	directed.addVertex(3);
	directed.addEdge(1, 2);
	directed.addEdge(2, 1);
	directed.addEdge(1, 3);*/
	/*
	std::vector<int> vertices { 1, 2, 3 };
	std::tuple<int, int, bool> b { 1, 2, true };
	std::vector<std::tuple<int, int, bool>> edges {b};
	GraphAdjVectors<int,true> g0;
	g0.addVertex(1);
	g0.addVertex(2);
	g0.addVertex(3);
	g0.addEdge(1, 2);
	g0.addEdge(2, 1);
	g0.addEdge(1, 3);
	std::cout << g0.outDegree(1); // 2 (→2, →3)
	std::cout << g0.inDegree(1);  // 1 (2→1)
	std::cout << g0.degree(1);     // 3 (2+1)*/

	// Неориентированный граф (по умолчанию)
	/*auto g1 = graph::make_graph<GraphAdjVectors>(vertices, edges);

	// Ориентированный граф
	auto g2 = graph::make_graph<graph::GraphAdjVectors, true>(vertices, edges);
	std::cout << g2.outDegree(1); // 2 (→2, →3)
	std::cout << g2.inDegree(1);  // 1 (2→1)
	std::cout << g2.degree(1);     // 3 (2+1)*/

	// Проверка, что это действительно работает для GraphAdjVectors
	/*static_assert(std::is_same_v<
		decltype(make_graph<graph::GraphAdjVectors>(std::vector<int>{},
			std::vector<std::tuple<int, int, bool>>{})),
		graph::GraphAdjVectors<int, false >> );
	std::cout << std::endl;
	auto g3 = graph::make_graph<graph::GraphVertexEdgeList, true>(vertices, edges);
	std::cout << g3.outDegree(1); // 2 (→2, →3)
	std::cout << g3.inDegree(1);  // 1 (2→1)
	std::cout << g3.degree(1);     // 3 (2+1)*/
		
	/*std::cout << directed.outDegree(1); // 2 (→2, →3)
	std::cout << directed.inDegree(1);  // 1 (2→1)
	std::cout << directed.degree(1);     // 3 (2+1)*/
	return 0;
}