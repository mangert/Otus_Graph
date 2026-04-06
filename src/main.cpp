#include <iostream>
#include <vector>
#include "test/test.cpp"
#include "test/bipartiteGraph.cpp"
#include "test/test_algorithms.cpp"
#include "test/test_mst.cpp"
#include "algorithms/graph_algorithms.h"
#include "graph/IGraph.h"
#include "graph/GraphAdjMatrix.h"
#include "graph/graph_factory.h"

void testAllAlgorithms();
void testAllMSTAlgorithms();

int main() {
	using namespace graph;
	setlocale(LC_ALL, "russian");
	/*
	//Часть 1 - граф из ДЗ - А(3,4) c пятью ребрами - двудольный
	testAllRepresentations();
	
	//Часть 2 - тестирование функционала
	std::cout << "=== TESTING UNDIRECTED GRAPHS ===\n";
	run_all_tests<false>();
	
	std::cout << "\n\n=== TESTING DIRECTED GRAPHS ===\n";
	run_all_tests<true>();
	
	//Часть 3 - тестирование алгоритмов
	testAllAlgorithms();*/
	
	//testAllMSTAlgorithms();
	using weighted_edge = std::tuple<int, int, double>;
	std::vector<int> vertices_example = { 1, 2, 3, 4, 5, 6 };
	std::vector<weighted_edge> edges_example = {
		{1, 2, 7},
		{1, 3, 9},
		{2, 3, 10},
		{2, 4, 15},
		{3, 4, 11},
		{3, 6, 2},
		{4, 5, 6},
		{6, 5, 9}
	};

	auto graph = graph::make_graph<graph::GraphAdjMatrix, false>(
		vertices_example, edges_example);
	auto result = graph_algorithms::deikstra(graph, 1).value();
	for (auto& item : result) {
		std::cout << item.v1 << " - " << item.v2 << "," << std::endl;
	}

	return 0;
}