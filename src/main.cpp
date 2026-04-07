#include <iostream>
#include <vector>
//#include "test/test.cpp"
//#include "test/bipartiteGraph.cpp"
//#include "test/test_algorithms.cpp"
//#include "test/test_mst.cpp"
//#include "test/test_shortest_path.cpp"
#include "algorithms/graph_algorithms_shortest_path.h"

void testAllAlgorithms();
void testAllMSTAlgorithms();
void testAllShortestPathAlgorithms();

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

	testAllShortestPathAlgorithms();
	
	return 0;
}