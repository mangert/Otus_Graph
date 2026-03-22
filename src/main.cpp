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
	auto result = graph_algorithms::demukron(graph);
	if (result.has_value()) {
		for (auto& row : result.value()) {
			for (auto& item : row) {
				std::cout << item << "  ";
			}
			std::cout << std::endl;
		}
	
	}

	/*auto result = GraphAlgorithm::demukron2(graph);
	if (result.has_value()) {
		for (auto& x : result.value()) {
			std::cout << x << "  ";
		}
	}*/
	std::cout << std::endl;
	
	return 0;
}