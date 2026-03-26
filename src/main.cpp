#include <iostream>
#include <vector>
#include "test/test.cpp"
#include "test/bipartiteGraph.cpp" 
#include <optional>
#include "data_structures/UnionFind.h"

void testAllAlgorithms();

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

	//Часть 3 - тестирование алгоритмов
	//testAllAlgorithms();

	// Пример использования
	UnionFind<int> uf;

	// Добавляем вершины
	uf.makeSet(0);
	uf.makeSet(1);
	uf.makeSet(2);
	uf.makeSet(3);

	// Объединяем множества
	uf.unite(0, 1);
	uf.unite(2, 3);

	// Проверяем связность
	bool connected1 = uf.isConnected(0, 1);  // true
	bool connected2 = uf.isConnected(0, 2);  // false
	std::cout << connected1 << " ** " << connected2 << std::endl;

	// Объединяем два компонента
	uf.unite(1, 2);

	// Теперь все связаны
	bool connected3 = uf.isConnected(0, 3);  // true
	std::cout << connected3 << std::endl;
	std::cout << uf.size() << std::endl;

	return 0;
}