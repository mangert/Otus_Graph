#pragma once
#include "List.h"  
#include <stdexcept>

template <typename T>
class Stack : private List<T> {
public:
    // ---------- Конструкторы ----------
    Stack() = default;
    
    // Конструктор копирования
    Stack(const Stack& other) : List<T>(other) {}

    // Конструктор перемещения
    Stack(Stack&& other) noexcept : List<T>(std::move(other)) {}

    ~Stack() = default;

    // ---------- Операторы присваивания ----------
    Stack& operator=(const Stack& other) {
        List<T>::operator=(other);
        return *this;
    }

    Stack& operator=(Stack&& other) noexcept {
        List<T>::operator=(std::move(other));
        return *this;
    }

    // ---------- Основные методы очереди ----------

    // Добавление элемента в стек
    void push(const T& value) {
        List<T>::push_front(value);
    }

    void push(T&& value) {
        List<T>::push_front(std::move(value));
    }

    // Получение элемента из стека
    void pop() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        List<T>::del(0);
    }

    // Доступ к первому элементу
    T& top() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return (*this)[0];  
    }

    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
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