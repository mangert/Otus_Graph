#include <iostream>
#include <vector>
#include "GraphEnumeration.h"
#include "GraphAdjMatrix.h"
#include "GraphIncMatrix.h"
#include "graph_factory.h"
#include "GraphEdgeList.h"


int main() {
	using namespace graph;
	std::cout << "hello" << std::endl;
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

	std::vector<int> vertices { 1, 2, 3 };
	std::tuple<int, int, bool> b { 1, 2, true };
	std::vector<std::tuple<int, int, bool>> edges {b};
	auto g1 = graph::make_graph<GraphAdjMatrix>(vertices, edges);  // Directed = false
	auto g2 = graph::make_graph<GraphEdgeList, true>(vertices, edges);  // Directed = true

	std::cout << g2.outDegree(1); // 2 (→2, →3)
	std::cout << g2.inDegree(1);  // 1 (2→1)
	std::cout << g2.degree(1);     // 3 (2+1)
		
	/*std::cout << directed.outDegree(1); // 2 (→2, →3)
	std::cout << directed.inDegree(1);  // 1 (2→1)
	std::cout << directed.degree(1);     // 3 (2+1)*/
	return 0;
}