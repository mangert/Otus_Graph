#pragma once
#include <vector>
#include <map>
#include <stdexcept>

template <typename T>
class UnionFind {
public:
    // Конструкторы
    UnionFind() = default;    
    
    explicit UnionFind(size_t expected_size) {
        nodes.reserve(expected_size);
        rank.reserve(expected_size);
    }
    UnionFind(std::initializer_list<T> init) {                
        size_t size = init.size();
        nodes.reserve(size);
        rank.reserve(size);
        for (const auto& item : init) {
            makeSet(item);
        }
    }
    UnionFind(const std::vector<T>& data) {
        nodes.reserve(data.size());
        rank.reserve(data.size());
        for (const auto& item : data) {
            makeSet(item);
        }
    }
    
    // Копирование и перемещение
    UnionFind(const UnionFind&) = default;
    UnionFind(UnionFind&&) = default;
    ~UnionFind() = default;
    
    // Операторы присваивания
    UnionFind& operator=(const UnionFind&) = default;    
    UnionFind& operator=(UnionFind&&) = default;    
  
    // Добавление нового множества с элементом data
    void makeSet(const T& data) {
        size_t idx = nodes.size();
        nodes.push_back(idx);          
        rank.push_back(0);

        data_to_index[data] = idx;
        index_to_data[idx] = data;
    }

    // Поиск корня по индексу (с сжатием пути)
    size_t root(size_t idx) {
        // Сжатие пути: рекурсивно находим корень и обновляем ссылки
        if (idx != nodes[idx]) {
            nodes[idx] = root(nodes[idx]);
        }
        return nodes[idx];
    }

    // Поиск корня по значению элемента
    size_t root(const T& data) {
        auto it = data_to_index.find(data);
        if (it == data_to_index.end()) {
            throw std::runtime_error("Data not found in UnionFind");
        }
        return root(it->second);
    }

    // Проверка связности по индексам
    bool isConnected(size_t idx0, size_t idx1) {
        return root(idx0) == root(idx1);
    }

    // Проверка связности по значениям элементов
    bool isConnected(const T& x, const T& y) {
        return root(x) == root(y);
    }

    // Объединение множеств по индексам
    void unite(size_t idx0, size_t idx1) {
        size_t root0 = root(idx0);
        size_t root1 = root(idx1);

        if (root0 == root1)
            return;

        // Объединяем по рангу (высоте дерева)
        if (rank[root0] < rank[root1]) {
            nodes[root0] = root1;
        }
        else if (rank[root0] > rank[root1]) {
            nodes[root1] = root0;
        }
        else {
            // Если высоты равны, делаем один корнем другого и увеличиваем ранг
            nodes[root1] = root0;
            ++rank[root0];
        }
    }

    // Объединение множеств по значениям элементов
    void unite(const T& x, const T& y) {
        unite(root(x), root(y));
    }

    // Получить количество элементов
    size_t size() const {
        return nodes.size();
    }

    // Проверить, существует ли вершина в структуре
    bool contains(const T& data) const {
        return data_to_index.find(data) != data_to_index.end();
    }

private:
    std::vector<size_t> nodes;      // родитель каждого элемента
    std::vector<size_t> rank;       // высота дерева (ранг)

    std::map<T, size_t> data_to_index;
    std::map<size_t, T> index_to_data;
};