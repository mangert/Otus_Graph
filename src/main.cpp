#include <iostream>
#include "test.cpp"
#include "bipartiteGraph.cpp" 

int main() {
	using namespace graph;
	setlocale(LC_ALL, "russian");
	
	//Часть 1 - граф из ДЗ - А(3,4) c пятью ребрами - двудольный
	testAllRepresentations();
	 
	//Часть 2 - тестирование функционала
	std::cout << "=== TESTING UNDIRECTED GRAPHS ===\n";
	run_all_tests<false>();

	std::cout << "\n\n=== TESTING DIRECTED GRAPHS ===\n";
	run_all_tests<true>();

	
	return 0;
}