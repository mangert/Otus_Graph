#pragma once
#include <vector>


template<typename T, typename Compare = std::less<T>>
    requires requires(Compare cmp, const T& a, const T& b) {
        { cmp(a, b) } -> std::convertible_to<bool>; 
    }
class PriorityQueue {
    
public:
    //конструкторы
    PriorityQueue() = default;
    explicit PriorityQueue(const Compare& cmp) : comp(cmp) {}
    PriorityQueue(const PriorityQueue&) = default;
    PriorityQueue(PriorityQueue&&) = default;
    ~PriorityQueue() = default;
    
    PriorityQueue& operator=(const PriorityQueue&) = default;
    PriorityQueue& operator=(PriorityQueue&&) = default;

    //основные методы
    void push(const T& value) {
        data.push_back(value);
        heapify_up(data.size() - 1);
    }

    void push(T&& value) {
        data.push_back(std::move(value));
        heapify_up(data.size() - 1);
    }

    void pop() {
        if (empty()) return;
        data[0] = std::move(data.back());
        data.pop_back();
        if (!empty()) {
            heapify_down(0);
        }
    }

    const T& top() const {
        return data[0];
    }

    bool empty() const {        
        return data.empty();
    }

    size_t size() const {
        return data.size();
    }

    void clear() {
        data.clear();
    }

private:
    void heapify_up(size_t index) {
        while (index > 0 && comp(data[index], data[parent(index)])) {
            std::swap(data[index], data[parent(index)]);
            index = parent(index);
        }
    }

    void heapify_down(size_t index) {
        size_t smallest = index;
        size_t l = left(index);
        size_t r = right(index);

        if (l < data.size() && comp(data[l], data[smallest])) {
            smallest = l;
        }
        if (r < data.size() && comp(data[r], data[smallest])) {
            smallest = r;
        }

        if (smallest != index) {
            std::swap(data[index], data[smallest]);
            heapify_down(smallest);
        }
    }


    size_t parent(size_t i) const { return (i - 1) / 2; }
    size_t left(size_t i) const { return 2 * i + 1; }
    size_t right(size_t i) const { return 2 * i + 2; }

private:
    std::vector<T> data;
    [[no_unique_address]] Compare comp; //компаратор

};