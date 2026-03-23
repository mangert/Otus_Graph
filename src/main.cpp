#include <iostream>
#include <vector>
#include "test/test.cpp"
#include "test/bipartiteGraph.cpp" 
#include "graph/GraphAdjMatrix.h"
#include "graph/graph_factory.h"
#include <optional>
#include "algorithms/graph_algorithms.h"
#include "data_structures/Queue.h"

int main() {
	using namespace graph;
	setlocale(LC_ALL, "russian");
	
	//Часть 1 - граф из ДЗ - А(3,4) c пятью ребрами - двудольный
	/*testAllRepresentations();
	 
	//Часть 2 - тестирование функционала
	std::cout << "=== TESTING UNDIRECTED GRAPHS ===\n";
	run_all_tests<false>();

	std::cout << "\n\n=== TESTING DIRECTED GRAPHS ===\n";
	run_all_tests<true>();*/
	using edge_data = std::tuple<int, int, bool>;	
	
	std::vector<int> vertices = { 1, 2, 3, 4, 5, 6 };

	std::vector<edge_data> edges = {
		{1, 2, true},  
		{1, 4, true},  
		{3, 2, true},  
		{2, 6, true},  
		{4, 6, true},
		{6, 5, true}
	};
	auto graph = make_graph<GraphAdjMatrix, true>(vertices, edges);
	std::cout << graph.vertexCount() << " ----------" << std::endl;
	auto result = graph_algorithms::demukron(graph);
	if (result.has_value()) {
		for (auto& row : result.value()) {
			for (auto& item : row) {
				std::cout << item << "  ";
			}
			std::cout << std::endl;
		}
	
	}
	std::cout << std::endl;
	/*auto result = GraphAlgorithm::demukron2(graph);
	if (result.has_value()) {
		for (auto& x : result.value()) {
			std::cout << x << "  ";
		}
	}*/
	// Создаем граф: 0 -> 1 -> 2 -> 3
	graph::GraphAdjVectors<int, true> graph2;
	for (int i = 0; i < 4; ++i) graph2.addVertex(i);
	graph2.addEdge(0, 1);
	graph2.addEdge(1, 2);
	graph2.addEdge(2, 3);

	auto result2 = graph_algorithms::tarjan(graph);
	size_t size = result2.value().size();
	std::cout << "Size " << size << std::endl;

	if (result2) {
		std::cout << "Топологический порядок: ";
		for (auto& v : result2.value()) {
			std::cout << v << " ";
		}
		// Ожидаемый вывод: 0 1 2 3 (или 3 2 1 0? зависит от обхода)
	}
	else {
		std::cout << "Граф содержит цикл!";
	}
	
	std::cout << std::endl;
	
	return 0;
}