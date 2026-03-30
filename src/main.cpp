#include <iostream>
#include <vector>
#include "test/test.cpp"
#include "test/bipartiteGraph.cpp"
#include <optional>
#include "data_structures/UnionFind.h"
#include "algorithms/graph_algorithms2.h"
#include "graph/GraphIncMatrix.h"
#include "graph/graph_factory.h"
#include "graph/GraphEdgeList.h"

void testAllAlgorithms();

void test_graph(const auto& graph, const std::string& name) {
	std::cout << "\n=== " << name << " ===" << std::endl;
	std::cout << "Vertices " << graph.vertexCount() << std::endl;
	// Проверяем соседей
	for (int v : {1, 2, 3, 4, 5, 6}) {
		std::cout << "Neighbors of " << v << ": ";
		auto neighbors = graph.getNeighbors(v);
		for (auto n : neighbors) {
			std::cout << n << " ";
		}
		std::cout << std::endl;
	}

	// Проверяем ребра
	std::cout << "Edges: ";
	auto edges = graph.getEdges();
	for (const auto& [from, to, weight] : edges) {
		std::cout << "(" << from << "-" << to << ":" << weight << ") ";
	}
	std::cout << std::endl;
}

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
	/*UnionFind<int> uf;

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
	graph::GraphIncMatrix<int, bool, true> g;*/

	using edge_data = std::tuple<int, int, double>;
	std::vector<int> vertices_dag_complex = { 1, 2, 3, 4, 5, 6 };
	std::vector<edge_data> edges_dag_complex = {
		{1, 2, 1.5},
		{1, 4, 2},
		{3, 2, 0.5},
		{2, 6, 3},
		{4, 6, 6},
		{6, 5, 2}
	};
	std::cout << "***********************" << std::endl;
	//auto graph = graph::make_graph<GraphIncMatrix, false>(vertices_dag_complex, edges_dag_complex);
	/*auto path = graph_algorithms::kruskal(graph);
	for (auto& item : path) {
		std::cout << item.v1 << " -> " << item.v2 << std::endl;
	}*/

	using edge_data = std::tuple<int, int, double>;
	std::vector<int> vertices = { 1, 2, 3, 4, 5, 6 };
	std::vector<edge_data> edges = {
		{1, 2, 1.5},
		{1, 4, 2.0},
		{3, 2, 0.5},
		{2, 6, 3.0},
		{4, 6, 6.0},
		{6, 5, 2.0}
	};

	// Используем подходящее представление графа с весами
	auto graph = graph::make_graph<graph::GraphIncMatrix, false>(
		vertices, edges);

	test_graph(graph, "GraphIncMatrix");
	std::cout << "--------------------------\n";

	auto result = graph_algorithms::prim(graph, 1);

	if (result.has_value()) {
		std::cout << "MST (Prim):" << std::endl;
		for (const auto& edge : result.value()) {
			std::cout << edge.v1 << " -- " << edge.v2 << std::endl;
		}
	}
	else {
		std::cout << "Граф несвязный" << std::endl;
	}
	std::cout << "Краскал" << std::endl;
	auto path = graph_algorithms::kruskal(graph);
	for (auto& item : path) {
		std::cout << item.v1 << " -> " << item.v2 << std::endl;
	}
	return 0;
}