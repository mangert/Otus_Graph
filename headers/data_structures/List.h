#pragma once

template <typename T>
class List {
public:
	struct Node {
		T data; //данные
		Node* next;
		//конструкторы
		Node() : next(nullptr) {};
		Node(T data) : data(data), next(nullptr) {};
		Node(const Node& other) = delete;
		Node(Node&& other) = delete;
		Node& operator=(Node&&) = delete;
		Node& operator=(const Node&) = delete;

		~Node() = default;
	};

	// Конструкторы
	List() = default;  // пустой список

	// Из initializer_list
	List(std::initializer_list<T> init) {
		for (const auto& item : init) {
			push_back(item);
		}
	};

	// Из количества с default значениями
	List(size_t count, const T& value = T{}) {
		for (size_t i = 0; i < count; ++i) {
			push_back(value);
		}
	};

	// Конструктор копирования
	List(const List& other) {
		for (const auto& item : other) {
			push_back(item);
		}
	};

	// Конструктор перемещения
	List(List&& other) noexcept
		: head(other.head), tail(other.tail), size_(other.size_) {
		other.head = nullptr;
		other.tail = nullptr;
		other.size_ = 0;
	}		

	virtual ~List() {
		clear();
	};	

	// Оператор копирующего присваивания
	List& operator=(const List& other) {
		clear();
		for (const auto& item : other) {
			push_back(item);
		}
		return *this;
	};
	
	// Оператор перемещающего присваивания
	List& operator=(List&& other) noexcept {
		clear();
		head = other.head;
		tail = other.tail;
		size_ = other.size_;
		other.head = nullptr;
		other.tail = nullptr;
		other.size_ = 0;
		return *this;
	};

	//получение информации о списке
	size_t size() const {
		return size_;
	}

	bool is_empty() const {
		return head == nullptr;
	}

	//функции для работы со списком
	
	//oчистка списка
	void clear() {
		Node* current = head;
		while (current != nullptr) {
			Node* next = current->next;
			delete current;
			current = next;
		}
		head = nullptr;
		tail = nullptr;
		size_ = 0;
	}	
	//доступ к данным
	T& operator[] (size_t idx) {
		if (idx >= size_) throw std::invalid_argument("Index out of range");
		Node* item = head;
		for (size_t counter = 0; counter < idx; ++counter) {
			item = item->next;
		};		
		return item->data;
	}

	const T& operator[] (size_t idx) const {
		if (idx >= size_) throw std::invalid_argument("Index out of range");
		Node* item = head;
		for (size_t counter = 0; counter < idx; ++counter) {
			item = item->next;
		};		
		return item->data;
	}

	//добавление в конец
	template<typename U>
	void push_back(U&& value) {

		//проверяем, что тип списка и тип добавляемого элемента совпадают
		static_assert(std::is_same_v<std::decay_t<U>, T>,
			"Exact type match required");

		Node* new_node = new Node(std::forward<U>(value));

		if (tail == nullptr) {
			head = new_node;
			tail = new_node;
		}
		else {
			tail->next = new_node;
			tail = new_node;
		}
		++size_;
	};

	//добавление в начало
	template<typename U>
	void push_front(U&& value) {

		//проверяем, что тип списка и тип добавляемого элемента совпадают
		static_assert(std::is_same_v<std::decay_t<U>, T>,
			"Exact type match required");

		Node* new_node = new Node(std::forward<U>(value));

		if (head == nullptr) {
			head = new_node;
			tail = new_node;
		}
		else {
			new_node->next = head;
			head = new_node;			
		}
		++size_;
	}

	//вставка на позицию
	template<typename U>
	void push(U&& value, size_t idx) {

		//проверяем, что тип списка и тип добавляемого элемента совпадают
		static_assert(std::is_same_v<std::decay_t<U>, T>,
			"Exact type match required");
		//проверяем индекс
		if (idx > size_) throw std::invalid_argument("Index out of range");

		if (idx == size_) {
			push_back(std::forward<U>(value));
			return;
		};
		if (0 == idx) {
			push_front(std::forward<U>(value));
			return;
		}

		Node* new_node = new Node(std::forward<U>(value));
		Node* position = head;
		//доходим до нужного элемента
		for (size_t counter = 1; counter < idx; ++counter) {
			position = position->next;
		};
		//добавляем
		new_node->next = position->next;
		position->next = new_node;		
		++size_;
	}

	//удаление элемента
	T del(size_t idx) {
		if (idx >= size_) throw std::invalid_argument("Index out of range");

		Node* to_delete = nullptr;
		T data;

		if (idx == 0) {
			to_delete = head;
			data = head->data;
			head = head->next;
			if (head == nullptr) tail = nullptr;
		}
		else {
			Node* prev = head;
			for (size_t i = 1; i < idx; ++i) {
				prev = prev->next;
			}
			to_delete = prev->next;
			data = to_delete->data;
			prev->next = to_delete->next;

			if (to_delete == tail) {
				tail = prev;
			}
		}

		delete to_delete;
		--size_;
		return data;
	}

//итератор для работы с нашим списком
public:
	class ListIterator {
	public:
		//для совместимости с STL
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		ListIterator() : node_ptr(nullptr) {}
		ListIterator(Node* node) : node_ptr(node) {}
		ListIterator(const Node* node) : node_ptr(const_cast<Node*>(node)) {}
		ListIterator(const ListIterator& other) : node_ptr(other.node_ptr) {}		

		T& operator*() { return node_ptr->data; }
		const T& operator*() const { return node_ptr->data; }

		T* operator->() { return &(node_ptr->data); }
		const T* operator->() const { return &(node_ptr->data); }

		ListIterator& operator++() {
			node_ptr = node_ptr->next;
			return *this;
		}

		ListIterator operator++(int) {
			ListIterator tmp = *this;
			node_ptr = node_ptr->next;
			return tmp;
		}

		bool operator==(const ListIterator& other) const {
			return node_ptr == other.node_ptr;
		}
		bool operator!=(const ListIterator& other) const {
			return node_ptr != other.node_ptr;
		}

	private:
		Node* node_ptr;
	};
	// Итераторы
	ListIterator begin() { return ListIterator(head); }
	ListIterator end() { return ListIterator(); }

	ListIterator begin() const { return ListIterator(head); }
	ListIterator end() const { return ListIterator(); }

private:
	Node* head = nullptr;
	Node* tail = nullptr;
	size_t size_ = 0;
};