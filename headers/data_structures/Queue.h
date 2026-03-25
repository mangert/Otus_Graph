#pragma once
#include "List.h"  
#include <stdexcept>

template <typename T>
class Queue : private List<T> {
public:
    // ---------- Конструкторы ----------
    Queue() = default;
    
    // Конструктор копирования
    Queue(const Queue& other) : List<T>(other) {}

    // Конструктор перемещения
    Queue(Queue&& other) noexcept : List<T>(std::move(other)) {}

    ~Queue() = default;

    // ---------- Операторы присваивания ----------
    Queue& operator=(const Queue& other) {
        List<T>::operator=(other);
        return *this;
    }

    Queue& operator=(Queue&& other) noexcept {
        List<T>::operator=(std::move(other));
        return *this;
    }

    // ---------- Основные методы очереди ----------

    // Добавление элемента в конец очереди (enqueue)
    void push(const T& value) {
        List<T>::push_back(value);
    }

    void push(T&& value) {
        List<T>::push_back(std::move(value));
    }

    // Удаление элемента из начала очереди (dequeue)
    void pop() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        List<T>::del(0);
    }

    // Доступ к первому элементу
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return (*this)[0];  
    }

    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return (*this)[0];
    }

    // ---------- Вспомогательные методы ----------
    bool empty() const {
        return List<T>::is_empty();
    }

    size_t size() const {
        return List<T>::size();
    }

    void clear() {
        List<T>::clear();
    }    
};