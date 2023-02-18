// Arseny Savchenko

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <utility>

namespace dsa_hw {
	template <typename T> struct extendable {
		virtual void add(const T& elem) noexcept = 0;
		virtual void add(T&& elem) noexcept = 0;

		template <typename ForwardIterator> void extend(
				const ForwardIterator begin,
				const ForwardIterator end
		) noexcept {
			for (auto it = begin; it != end; ++it)
				add(*it);
		}
	};

	template <typename T> struct list : public extendable<T> {
		virtual void push_back(const T& elem) noexcept = 0;
		virtual void push_back(T&& elem) noexcept = 0;

		inline void add(const T& elem) noexcept override { push_back(elem); }
		inline void add(T&& elem) noexcept override { push_back(elem); }

		virtual T& pop_back() noexcept = 0;
		virtual void clear() noexcept = 0;

		[[nodiscard]] virtual constexpr std::size_t get_size() const noexcept = 0;
		[[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }
		[[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return get_size() != 0; }

		[[nodiscard]] virtual constexpr T& back() noexcept = 0;
		[[nodiscard]] virtual constexpr const T& back() const noexcept = 0;
		[[nodiscard]] virtual constexpr T& front() noexcept = 0;
		[[nodiscard]] virtual constexpr const T& front() const noexcept = 0;
	};

	template <typename T, typename A = std::allocator<T>> class vec : public list<T> {
		A allocator;
		using type_traits = std::allocator_traits<A>;

		T* data;

		std::size_t capacity = 16;
		std::size_t size = 0;

		inline static std::size_t get_init_capacity(const std::size_t the_size) noexcept {
			for (int i = 4; i < 64; ++i) {
				const std::size_t cap = 1LL << i;

				if (cap >= the_size)
					return cap;
			}

			return 1LL << 63;
		}

		inline void init_capacity() noexcept { capacity = get_init_capacity(size); }

		inline T* realloc(const std::size_t new_size) noexcept {
			const auto old_capacity = capacity;
			capacity = get_init_capacity(new_size);
			size = std::min(new_size, size);

			auto* new_buffer = type_traits::allocate(allocator, capacity);
			type_traits::construct(allocator, new_buffer);

			for (auto* data_ptr = data, *buf_ptr = new_buffer; data_ptr != data + size; ++data_ptr, ++buf_ptr)
				std::move(data_ptr, data_ptr + 1, buf_ptr);

			type_traits::deallocate(allocator, data, old_capacity);
			return new_buffer;
		}

		[[nodiscard]] constexpr inline bool is_capacity_reached() const noexcept {
			return size >= capacity;
		}

		inline void resize_if_capacity_reached() noexcept {
			if (is_capacity_reached())
				data = realloc(size);
		}

		[[nodiscard]] constexpr inline bool is_size_less_than_capacity_four_times() const noexcept {
			return size * 4 <= capacity;
		}

		inline void resize_if_size_less_than_capacity_four_times() noexcept {
			if (is_size_less_than_capacity_four_times()) {
				const auto old_capacity = capacity;
				capacity >>= 1;

				auto* new_buffer = type_traits::allocate(allocator, capacity);
				type_traits::construct(allocator, new_buffer);

				for (auto* data_ptr = data, *buf_ptr = new_buffer; data_ptr != data + size; ++data_ptr, ++buf_ptr)
					std::move(data_ptr, data_ptr + 1, buf_ptr);

				type_traits::deallocate(allocator, data, old_capacity);
				data = new_buffer;
			}
		}

		inline void init_data() noexcept {
			data = type_traits::allocate(allocator, capacity);
		}

	public:
		using value_type = T;

		class const_iterator;

		class iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = value_type*;
			using reference = value_type&;

		private:
			friend class vec<T>;
			pointer ptr;

			/**
			 * Creates iterator from given pointer
			 * @param ptr pointer to element
			 */

			explicit iterator(pointer ptr) noexcept : ptr(ptr) {}

		public:
			~iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			inline reference operator*() const noexcept { return *ptr; }
			inline pointer operator->() const noexcept { return ptr; }

			// --------------- Comparison operators ---------------

			constexpr inline bool operator==(const iterator& other) const noexcept { return ptr == other.ptr; };
			constexpr inline bool operator!=(const iterator& other) const noexcept { return ptr != other.ptr; };

			constexpr inline bool operator==(const const_iterator& other) const noexcept { return ptr == other.ptr; };
			constexpr inline bool operator!=(const const_iterator& other) const noexcept { return ptr != other.ptr; };

			// --------------- Movement operators ---------------

			inline iterator operator++() noexcept { ++ptr; return *this; }
			inline iterator operator--() noexcept { --ptr; return *this; }
			constexpr inline iterator operator+(const std::size_t move) const noexcept { return iterator(ptr + move); }
			constexpr inline iterator operator-(const std::size_t move) const noexcept { return iterator(ptr - move); }
		};

		class const_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = value_type* const;
			using reference = const value_type&;

		private:
			friend class vec<T>;
			pointer ptr;

			/**
			 * Creates iterator from given pointer
			 * @param ptr pointer to element
			 */

			explicit const_iterator(const pointer ptr) noexcept : ptr(ptr) {}

		public:
			~const_iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			inline reference operator*() const noexcept { return *ptr; }
			inline pointer operator->() const noexcept { return ptr; }

			// --------------- Comparison operators ---------------

			constexpr inline bool operator==(const iterator& other) const noexcept { return ptr == other.ptr; };
			constexpr inline bool operator!=(const iterator& other) const noexcept { return ptr != other.ptr; };

			constexpr inline bool operator==(const const_iterator& other) const noexcept { return ptr == other.ptr; };
			constexpr inline bool operator!=(const const_iterator& other) const noexcept { return ptr != other.ptr; };

			// --------------- Movement operators ---------------

			inline const_iterator operator++() noexcept { ++ptr; return *this; }
			inline const_iterator operator--() noexcept { --ptr; return *this; }
			constexpr inline const_iterator operator+(const std::size_t move) const noexcept { return iterator(ptr + move); }
			constexpr inline const_iterator operator-(const std::size_t move) const noexcept { return iterator(ptr - move); }
		};

		vec() noexcept {
			capacity = 16;
			init_data();
		}

		explicit vec(const std::size_t initial_size) noexcept {
			size = initial_size;
			init_capacity();
			init_data();
			T();

			for (auto* p = data; p != data + size; ++p)
				type_traits::construct(allocator, p);
		}

		vec(const std::size_t initial_size, const T& init) noexcept {
			size = initial_size;
			init_capacity();
			init_data();

			for (auto* p = data; p != data + size; ++p)
				*p = init;
		}

		vec(const std::size_t initial_size, const std::function<T()> init) noexcept {
			size = initial_size;
			init_capacity();
			init_data();

			for (auto* p = data; p != data + size; ++p)
				*p = init();
		}

		vec(const std::initializer_list<T>& elems) noexcept {
			size = elems.size();
			init_capacity();
			init_data();

			for (int i = 0; i < elems.size(); ++i)
				data[i] = elems[i];
		}

		vec(std::initializer_list<T>&& elems) noexcept {
			size = elems.size();
			init_capacity();
			init_data();

			for (int i = 0; i < elems.size(); ++i)
				data[i] = elems[i];
		}

		~vec() noexcept {
			for (auto* p = data; p != data + size; ++p)
				type_traits::destroy(allocator, p);

			type_traits::deallocate(allocator, data, capacity);
		}

		inline void push_back(const T& elem) noexcept override {
			++size;
			resize_if_capacity_reached();
			data[size - 1] = elem;
		}

		inline void push_back(T&& elem) noexcept override {
			++size;
			resize_if_capacity_reached();
			data[size - 1] = elem;
		}

		inline void insert(const const_iterator position, const T& elem) noexcept {
			push_back(elem);
			const auto ind = position.ptr - data;
			auto* tmp = allocator.allocate(1);

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp); 		// tmp = data[i]
				std::move(data + i - 1, data + i, data + i); 	// data[i] = data[i - 1]
				std::move(tmp, tmp + 1, data - 1); 				// data[i - 1] = tmp == data[i]
			}

			allocator.deallocate(tmp, 1);
		}

		inline void insert(const const_iterator position, T&& elem) noexcept {
			push_back(elem);
			const auto ind = position.ptr - data;
			auto* tmp = allocator.allocate(1);

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp); 		// tmp = data[i]
				std::move(data + i - 1, data + i, data + i); 	// data[i] = data[i - 1]
				std::move(tmp, tmp + 1, data - 1); 				// data[i - 1] = tmp == data[i]
			}

			allocator.deallocate(tmp, 1);
		}

		inline void insert(const iterator position, const T& elem) noexcept {
			push_back(elem);
			const auto ind = position.ptr - data;
			auto* tmp = allocator.allocate(1);

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp); 		// tmp = data[i]
				std::move(data + i - 1, data + i, data + i); 	// data[i] = data[i - 1]
				std::move(tmp, tmp + 1, data - 1); 				// data[i - 1] = tmp == data[i]
			}

			allocator.deallocate(tmp, 1);
		}

		inline void insert(const iterator position, T&& elem) noexcept {
			push_back(elem);
			const auto ind = position.ptr - data;
			auto* tmp = allocator.allocate(1);

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp); 		// tmp = data[i]
				std::move(data + i - 1, data + i, data + i); 	// data[i] = data[i - 1]
				std::move(tmp, tmp + 1, data - 1); 				// data[i - 1] = tmp == data[i]
			}

			allocator.deallocate(tmp, 1);
		}

		inline T& pop_back() noexcept override {
			if (size == 0)
				throw std::out_of_range("vec is empty");

			resize_if_size_less_than_capacity_four_times();

			auto& back = data[size - 1];
			--size;
			return back;
		}

		inline void erase(const const_iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("vec is empty");

			resize_if_size_less_than_capacity_four_times();

			const auto ind = position.ptr - data;
			auto* tmp = allocator.allocate(1);

			for (int i = ind + 1; i < size; ++i) {
				std::move(data + i, data + i + 1, tmp); 		// tmp = data[i]
				std::move(data + i - 1, data + i, data + i); 	// data[i] = data[i - 1]
				std::move(tmp, tmp + 1, data - 1); 				// data[i - 1] = tmp == data[i]
			}

			pop_back();
			allocator.deallocate(tmp, 1);
		}

		inline void erase(const iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("vec is empty");

			resize_if_size_less_than_capacity_four_times();

			const auto ind = position.ptr - data;
			auto* tmp = allocator.allocate(1);

			for (int i = ind + 1; i < size; ++i) {
				std::move(data + i, data + i + 1, tmp); 		// tmp = data[i]
				std::move(data + i - 1, data + i, data + i); 	// data[i] = data[i - 1]
				std::move(tmp, tmp + 1, data - 1); 				// data[i - 1] = tmp == data[i]
			}

			pop_back();
			allocator.deallocate(tmp, 1);
		}

		inline void clear() noexcept override {
			for (auto* p = data; p != data + size; ++p)
				type_traits::destroy(allocator, p);

			allocator.deallocate(data, capacity);
			size = 0;
			capacity = 16;
			init_data();
		}

		inline void resize(const std::size_t new_size) noexcept {
			data = realloc(new_size);
		}

		inline void reserve(const std::size_t new_size) noexcept {
			if (new_size > capacity)
				resize(new_size);
		}

		inline void shrink_to_fit() noexcept {
			const auto old_capacity = capacity;
			capacity = size;
			auto* new_buffer = allocator.allocate(capacity);

			for (auto* data_ptr = data, *buf_ptr = new_buffer; data_ptr != data + size; ++data_ptr, ++buf_ptr)
				std::move(data_ptr, data_ptr + 1, buf_ptr);

			for (auto* p = data; p != data + size; ++p)
				type_traits::destroy(allocator, p);

			allocator.deallocate(data, old_capacity);
			data = new_buffer;
		}

		[[nodiscard]] constexpr inline T& get_unchecked(const std::size_t ind) noexcept { return data[ind]; }
		[[nodiscard]] constexpr inline const T& get_unchecked(const std::size_t ind) const noexcept { return data[ind]; }

		[[nodiscard]] inline T& operator[](const std::size_t ind) noexcept {
			if (ind >= size)
				throw std::out_of_range("Vec's index out of range");

			return get_unchecked(ind);
		}

		[[nodiscard]] inline const T& operator[](const std::size_t ind) const noexcept {
			if (ind >= size)
				throw std::out_of_range("Vec's index out of range");

			return get_unchecked(ind);
		}

		[[nodiscard]] constexpr inline iterator begin() noexcept { return iterator(data); }
		[[nodiscard]] constexpr inline const_iterator begin() const noexcept { return const_iterator(data); }

		[[nodiscard]] constexpr inline T& front() noexcept override { return *begin(); }
		[[nodiscard]] constexpr inline const T& front() const noexcept override { return *begin(); }

		[[nodiscard]] constexpr inline iterator end() noexcept { return iterator(data + size); }
		[[nodiscard]] constexpr inline const_iterator end() const noexcept { return const_iterator(data + size); }

		[[nodiscard]] constexpr inline T& back() noexcept override { return data[size - 1]; }
		[[nodiscard]] constexpr inline const T& back() const noexcept override { return data[size - 1]; }

		[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return size; }
	};

	template <typename T> class linked_list : public list<T> {
		struct node : std::enable_shared_from_this<node> {
			std::shared_ptr<T> value = std::shared_ptr<T>(nullptr);
			std::shared_ptr<node> previous_node = std::shared_ptr<node>(nullptr);
			std::weak_ptr<node> next_node = std::weak_ptr<node>();

			node() noexcept = default;

			explicit node(const T& value) noexcept { this->value = std::make_shared<T>(value); }
			explicit node(T&& value) noexcept { this->value = std::make_shared<T>(value); }

			node(const T& value, const std::shared_ptr<node> previous_node, const std::weak_ptr<node> next_node) noexcept {
				this->value = std::make_shared(value);
				this->previous_node = previous_node;
				this->next_node = next_node;
			}

			node(T&& value, const std::shared_ptr<node> previous_node, const std::weak_ptr<node> next_node) noexcept {
				this->value = std::make_shared(value);
				this->previous_node = previous_node;
				this->next_node = next_node;
			}

			~node() noexcept = default;
		};

		std::shared_ptr<node> front_node = std::shared_ptr<node>(nullptr);
		std::shared_ptr<node> back_node = std::shared_ptr<node>(nullptr);
		std::size_t size = 0;

	public:
		class const_iterator;

		class iterator {
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = std::shared_ptr<value_type>;
			using reference = value_type&;

		private:
			friend class linked_list<T>;
			std::shared_ptr<node> node_ptr;

			/**
			 * Creates const iterator from given pointer
			 * @param node_ptr pointer to element
			 */

			explicit iterator(std::shared_ptr<node> node_ptr) noexcept : node_ptr(std::move(node_ptr)) {}

		public:
			~iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			inline reference operator*() const noexcept { return *node_ptr->value; }
			inline pointer operator->() const noexcept { return node_ptr->value; }

			// --------------- Comparison operators ---------------

			constexpr inline bool operator==(const iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			constexpr inline bool operator!=(const iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			constexpr inline bool operator==(const const_iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			constexpr inline bool operator!=(const const_iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			// --------------- Movement operators ---------------

			inline iterator operator++() noexcept { node_ptr = node_ptr->next_node.lock(); return *this; }
			inline iterator operator--() noexcept { node_ptr = node_ptr->previous_node; return *this; }

			[[nodiscard]] constexpr inline bool has_value() const noexcept {
				return node_ptr.get() != nullptr && node_ptr->value.get() != nullptr;
			}
		};

		class const_iterator {
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = std::shared_ptr<value_type>;
			using reference = const value_type&;

		private:
			friend class linked_list<T>;
			std::shared_ptr<node> node_ptr;

			/**
			 * Creates iterator from given pointer
			 * @param ptr pointer to element
			 */

			explicit const_iterator(const std::shared_ptr<node> node_ptr) noexcept : node_ptr(node_ptr) {}

		public:
			~const_iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			inline reference operator*() const noexcept { return *node_ptr->value; }
			inline pointer operator->() const noexcept { return node_ptr->value; }

			// --------------- Comparison operators ---------------

			constexpr inline bool operator==(const iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			constexpr inline bool operator!=(const iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			constexpr inline bool operator==(const const_iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			constexpr inline bool operator!=(const const_iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			// --------------- Movement operators ---------------

			inline const_iterator operator++() noexcept { node_ptr = node_ptr->next_node.lock(); return *this; }
			inline const_iterator operator--() noexcept { node_ptr = node_ptr->previous_node; return *this; }

			[[nodiscard]] constexpr inline bool has_value() const noexcept {
				return node_ptr.get() != nullptr && node_ptr->value.get() != nullptr;
			}
		};

		linked_list() noexcept = default;

		linked_list(std::initializer_list<T> elems) noexcept {
			std::for_each(elems.begin(), elems.end(), [this](const T& elem) { push_back(elem); });
		}

		~linked_list() = default;

		inline void push_back(const T& elem) noexcept override {
			auto new_node = std::make_shared<node>(node(elem));
			++size;

			if (size == 1) {
				front_node = new_node;
				back_node = new_node;
			} else {
				new_node->previous_node = back_node;
				back_node->next_node = new_node;
				back_node = new_node;
			}
		}

		inline void push_back(T&& elem) noexcept override {
			auto new_node = std::make_shared<node>(node(elem));
			++size;

			if (size == 1) {
				front_node = new_node;
				back_node = new_node;
			} else {
				new_node->previous_node = back_node;
				back_node->next_node = new_node;
				back_node = new_node;
			}
		}

		inline void push_front(const T& elem) noexcept {
			auto new_node = std::make_shared<node>(node(elem));
			++size;

			if (size == 1) {
				front_node = new_node;
				back_node = new_node;
			} else {
				new_node->next_node = front_node;
				front_node->previous_node = new_node;
				front_node = new_node;
			}
		}

		inline void push_front(T&& elem) noexcept {
			auto new_node = std::make_shared<node>(node(elem));
			++size;

			if (size == 1) {
				front_node = new_node;
				back_node = new_node;
			} else {
				new_node->next_node = front_node;
				front_node->previous_node = new_node;
				front_node = new_node;
			}
		}

		inline void insert(iterator position, const T& elem) noexcept {
			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			auto new_node = std::make_shared<node>(node(elem, prev_node, next_node));
			next_node.lock()->previous_node = new_node;
			prev_node->next_node = new_node;
		}

		inline void insert(const_iterator position, const T& elem) noexcept {
			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			auto new_node = std::make_shared<node>(node(elem, prev_node, next_node));
			next_node.lock()->previous_node = new_node;
			prev_node->next_node = new_node;
		}

		inline T& pop_back() noexcept override {
			if (size == 0)
				throw std::out_of_range("list is empty");

			auto& elem = *back_node->value;
			auto removed_node = back_node;

			back_node = back_node->previous_node;
			removed_node->previous_node.reset();
			--size;

			return elem;
		}

		inline T& pop_front() noexcept {
			if (size == 0)
				throw std::out_of_range("list is empty");

			auto& elem = *front_node->value;
			auto removed_node = front_node;

			front_node = front_node->next_node.lock();
			removed_node->next_node.reset();
			--size;

			return elem;
		}

		inline void erase(iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("list is empty");

			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			next_node.lock()->previous_node = prev_node;
			prev_node->next_node = next_node;
		}

		inline void erase(const_iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("list is empty");

			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			next_node.lock()->previous_node = prev_node;
			prev_node->next_node = next_node;
		}

		inline void clear() noexcept override {
			const auto sz = size;
			for (int i = 0; i < sz; ++i) pop_back();
		}

		[[nodiscard]] constexpr inline iterator begin() noexcept { return iterator(front_node); }
		[[nodiscard]] constexpr inline const_iterator begin() const noexcept { return const_iterator(front_node); }

		[[nodiscard]] constexpr inline T& front() noexcept override { return *front_node->value; }
		[[nodiscard]] constexpr inline const T& front() const noexcept override { return *front_node->value; }

		[[nodiscard]] constexpr inline iterator end() noexcept {
			return iterator(back_node.get() == nullptr ? back_node : back_node->next_node.lock());
		}

		[[nodiscard]] constexpr inline const_iterator end() const noexcept {
			return iterator(back_node.get() == nullptr ? back_node : back_node->next_node.lock());
		}

		[[nodiscard]] constexpr inline T& back() noexcept override { return *back_node->value; }
		[[nodiscard]] constexpr inline const T& back() const noexcept override { return *back_node->value; }

		[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return size; }
	};

	template <typename T> class linked_stack {
		linked_list<T> wrapper;

	public:
		linked_stack() = default;
		~linked_stack() = default;

		inline void push(const T& elem) noexcept { wrapper.push_back(elem); }
		inline void push(T&& elem) noexcept { wrapper.push_back(elem); }

		inline T& pop() noexcept { return wrapper.pop_back(); }
		inline void clear() noexcept { wrapper.clear(); }

		[[nodiscard]] constexpr inline T& top() noexcept { return wrapper.back(); }
		[[nodiscard]] constexpr inline const T& top() const noexcept { return wrapper.back(); }

		[[nodiscard]] constexpr inline std::size_t get_size() const noexcept { return wrapper.get_size(); }
		[[nodiscard]] constexpr inline bool is_empty() const noexcept { return wrapper.is_empty(); }
		[[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return wrapper.is_not_empty(); }
	};

    namespace hash {
 
        // ############################ Map ADL ############################
 
        /**
         * Map ADL with basic insertion and remove strategy
         * @param K key type
         * @param V value type
         */
 
        template <typename K, typename V> struct map {
 
            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             *
             * @param key entry's key to insert
             * @param value value to insert / update
             */
 
            virtual void insert(const K& key, const V& value) noexcept = 0;
 
            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             *
             * @param key entry's key to insert
             * @param value value to insert / update
             */
 
            virtual void insert(K&& key, V&& value) noexcept = 0;
 
            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             * @param entry key and value to insert
             */
 
            virtual void insert(const std::pair<K, V>& entry) noexcept = 0;
 
            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             * @param entry key and value to insert
             */
 
            virtual void insert(std::pair<K, V>&& entry) noexcept = 0;
 
            /**
             * Gets value by its key.
             * If key is not present in the map,
             * creates default value with default constructor
             * @param key entry's key to access value
             */
 
            virtual V& operator[] (const K& key) noexcept = 0;
 
            /**
             * Removes value by its key, if present.
             * @param key entry's key to access value
             * @return true if value was removed, otherwise false
             */
 
            virtual bool remove(const K& key) noexcept = 0;
 
            /** @return Number of inserted elements */
 
            [[nodiscard]] virtual constexpr std::size_t get_size() const noexcept = 0;
 
            /**
             * Are there any elements?
             * @return true if size is zero
             */
 
            [[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }
 
            /**
             * Are there any elements?
             * @return false if size is zero
             */
 
            [[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return !is_empty(); }
        };

		template <typename T> struct set {
			virtual void insert(const T& value) noexcept = 0;
			virtual void insert(T&& value) noexcept = 0;

			virtual bool remove(const T& value) noexcept = 0;

			[[nodiscard]] virtual constexpr std::size_t get_size() const noexcept = 0;
			[[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }
			[[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return get_size() != 0; }
		};
 
        // ############################ Hash Map ############################
 
        /**
         * Hash map implementation with side-chaining (buckets).
         * By default, the number of buckets is 16; Load factor is 75%.
         * By default, std::hash<K> is used to hash keys. To provide
         * your personal hasher, you have to override std::size_t operator()
         * with your hashing strategy. You can use Boost's hash_combine() method
         * for your data structure.
         *
         * @param K key's type
         * @param V value's type
         * @param H hasher class with overridden std::size_t operator(). By default std::hash<K> is used
         * @param KEq key's equality checker. By default std::equal_to<K> is used
         */
 
        template <typename K, typename V, typename H = std::hash<K>, typename KEq = std::equal_to<K>> class hash_map : public map<K, V> {
            constexpr static const float LOAD_FACTOR = 0.75F;
 
            H hasher = H();
            KEq key_equality_checker = KEq();
 
            std::size_t elems = 0;
            vec<linked_list<std::pair<K, V>>> buckets = vec<linked_list<std::pair<K, V>>>(16);
 
            class const_bucket_iterator;
 
            // ############################ Bucket Iterator ############################
 
            /** Random access iterator to iterate over the buckets */
 
            class bucket_iterator {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = linked_list<std::pair<K, V>>;
                using pointer = linked_list<std::pair<K, V>>*;
                using reference = value_type&;
 
            private:
                /** Index of current bucket */
                std::size_t index;
 
                /** Begin iterator over the buckets (first element's iterator) */
                vec<linked_list<std::pair<K, V>>>::iterator begin;
 
            public:
 
                /**
                 * Creates bucket iterator from bucket's index and begin iterator
                 * @param index index of current bucket
                 * @param begin iterator to the beginning of buckets
                 */
 
                bucket_iterator(
                        const std::size_t index,
                        vec<linked_list<std::pair<K, V>>>::iterator begin
                ) noexcept : index(index), begin(begin) {}
 
                ~bucket_iterator() noexcept = default;
 
                // --------------- Dereference operators ---------------
 
                inline reference operator*() const noexcept { return *(begin + index); }
                inline pointer operator->() const noexcept { return &*(begin + index); }
 
                // --------------- Comparison operators ---------------
 
                constexpr inline bool operator==(const bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const bucket_iterator& other) const noexcept { return index != other.index; };
 
                constexpr inline bool operator==(const const_bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const const_bucket_iterator& other) const noexcept { return index != other.index; };
 
                // --------------- Movement operators ---------------
 
                inline bucket_iterator operator++() noexcept { ++index; return *this; }
                inline bucket_iterator operator--() noexcept { --index; return *this; }
                constexpr inline bucket_iterator operator+(const std::size_t move) const noexcept { return bucket_iterator(index + move, begin); }
                constexpr inline bucket_iterator operator-(const std::size_t move) const noexcept { return bucket_iterator(index - move, begin); }
            };
 
            [[nodiscard]] constexpr inline bucket_iterator get_bucket_begin_it() const noexcept {
                return bucket_iterator(0, buckets.begin());
            }
 
            [[nodiscard]] constexpr inline bucket_iterator get_bucket_end_it() const noexcept {
                return bucket_iterator(buckets.size(), buckets.begin());
            }
 
            // ############################ Const Bucket Iterator ############################
 
            /**
             * Random access iterator to iterate over the buckets.
             * Not able to modify its value
             */
 
            class const_bucket_iterator {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = linked_list<std::pair<K, V>>;
                using pointer = const linked_list<std::pair<K, V>>*;
                using reference = const value_type&;
 
            private:
                /** Index of current bucket */
                std::size_t index;
 
                /** Begin iterator over the buckets (first element's iterator) */
                vec<linked_list<std::pair<K, V>>>::const_iterator begin;
 
            public:
 
                /**
                 * Creates bucket iterator from bucket's index and begin iterator
                 * @param index index of current bucket
                 * @param begin iterator to the beginning of buckets
                 */
 
                const_bucket_iterator(
                        const std::size_t index,
                        const vec<linked_list<std::pair<K, V>>>::const_iterator begin
                ) noexcept : index(index), begin(begin) {}
 
                ~const_bucket_iterator() noexcept = default;
 
                // --------------- Dereference operators ---------------
 
                inline reference operator*() const noexcept { return *(begin + index); }
                inline pointer operator->() const noexcept { return &*(begin + index); }
 
                // --------------- Comparison operators ---------------
 
                constexpr inline bool operator==(const bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const bucket_iterator& other) const noexcept { return index != other.index; };
 
                constexpr inline bool operator==(const const_bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const const_bucket_iterator& other) const noexcept { return index != other.index; };
 
                // --------------- Movement operators ---------------
 
                inline const_bucket_iterator operator++() noexcept { ++index; return *this; }
                inline const_bucket_iterator operator--() noexcept { --index; return *this; }
                constexpr inline const_bucket_iterator operator+(const std::size_t move) const noexcept { return const_bucket_iterator(index + move, begin); }
                constexpr inline const_bucket_iterator operator-(const std::size_t move) const noexcept { return const_bucket_iterator(index - move, begin); }
            };
 
			// --------------- Resize strategies ---------------
 
            /** Checks if number of elements greater than load factor */
 
            [[nodiscard]] constexpr inline bool is_load_factor_reached() const noexcept {
                return 1.0F * elems / buckets.get_size() > LOAD_FACTOR;
            }
 
			/**
			 * If load factor is reached, doubles the number of buckets
			 * and reinserts all elements from buffer.
			 *
			 * Takes O(n) to buffer values with move semantics +
			 * O(n + hash strategy) to insert new values in average case
			 *
			 * @return true if load factor was reached and
			 * the number of buckets was increased
			 */
 
            inline bool resize_if_load_factor_reached() noexcept {
                if (is_load_factor_reached()) {
                    vec<std::pair<K, V>> store;
                    std::move(begin(), end(), std::back_inserter(store));
 
                    const std::size_t new_buckets_number = buckets.get_size() << 1;
 
					// Trying to save as much memory as possible
 
                    elems = 0;
                    buckets.clear();
                    buckets.resize(new_buckets_number);
                    buckets.shrink_to_fit();
 
                    std::for_each(store.begin(), store.end(), [this](auto&& p) { this->insert_after_resize(p.first, p.second); });
                    return true;
                }
 
                return false;
            }
 
			// --------------- Search key in bucket ---------------
 
			/**
			 * Searches entry by given key with O(n) in worst case.
			 * @param bucket bucket to search in
			 * @param key entry's key
			 * @return entry's iterator or bucket's end if it was not found
			 */
 
			[[nodiscard]] inline linked_list<std::pair<K, V>>::iterator find_by_key(
					linked_list<std::pair<K, V>>& bucket,
					const K& key
			) noexcept {
                return std::find_if(
                        bucket.begin(),
                        bucket.end(),
                        [this, &key](const std::pair<K, V>& p) { return key_equality_checker(p.first, key); }
                );
            }
 
			/**
			 * Searches entry by given key with O(n) in worst case.
			 * @param bucket bucket to search in
			 * @param key entry's key
			 * @return entry's iterator or bucket's end if it was not found
			 */
 
            [[nodiscard]] constexpr inline linked_list<std::pair<K, V>>::const_iterator find_by_key(
					const linked_list<std::pair<K, V>>& bucket,
					const K& key
			) const noexcept {
                return std::find_if(
                        bucket.begin(),
                        bucket.end(),
                        [this, &key](const std::pair<K, V>& p) { return key_equality_checker(p.first, key); }
                );
            }
 
        public:
            class const_iterator;
 
			// ############################ Iterator ############################
 
			/** Bidirectional iterator to iterate over map's entries */
 
            class iterator {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::pair<K, V>;
                using pointer = value_type*;
                using reference = value_type&;
 
            private:
				friend class hash_map<K, V, H, KEq>;
 
				/** Iterator over current bucket */
                linked_list<std::pair<K, V>>::iterator it;
 
				/** Iterator to current bucket */
                bucket_iterator bucket_it;
 
				/** Iterator to first bucket */
                bucket_iterator first_bucket_it;
 
				/** Iterator to buckets' end */
                bucket_iterator end_buckets_it;
 
				/**
				 * Creates new iterator
				 * @param it current bucket's iterator
				 * @param bucket_it iterator to current bucket
				 * @param first_bucket_it iterator to first bucket
				 * @param end_buckets_it iterator to buckets' end
				 */
 
				iterator(
						const linked_list<std::pair<K, V>>::iterator it,
						const bucket_iterator bucket_it,
						const bucket_iterator first_bucket_it,
						const bucket_iterator end_buckets_it
				) noexcept : it(it), bucket_it(bucket_it), first_bucket_it(first_bucket_it), end_buckets_it(end_buckets_it) {};
 
			public:
                ~iterator() noexcept = default;
 
				// --------------- Dereference operators ---------------
 
                inline reference operator*() const noexcept { return *it; }
                inline pointer operator->() const noexcept { return &*it; }
 
				// --------------- Validation operators ---------------
 
				constexpr inline explicit operator bool() const noexcept { return it != nullptr && it->has_value(); }
                constexpr inline bool operator!() const noexcept { return it == nullptr || !it->has_value(); }
 
				// --------------- Comparison operators ---------------
 
                constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };
 
                constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };
 
				// --------------- Movement operators ---------------
 
				/**
				 * Searches for the next entry and moves iterator to it.
				 * Firstly, checks if there is next entry to move in bucket (not the end of bucket).
				 * If so, it will return iterator on it. In other case, starts searching for the next
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Implementation does not check if the next entry exists! In other words, if you are out of bounds,
				 * the exception will not be thrown and you will have segmentation fault. It is up to you
				 * to check bounds.
				 * @return iterator to the next entry
				 */
 
                inline iterator operator++() noexcept {
                    if (std::next(it) == bucket_it->end()) {
                        while (bucket_it + 1 != end_buckets_it && (bucket_it + 1)->is_empty())
                            ++bucket_it;
 
                        it = (bucket_it + 1 == end_buckets_it) ? bucket_it->end() : (++bucket_it)->begin();
                    } else { ++it; }
 
                    return *this;
                }
 
				/**
				 * Searches for the previous entry and moves iterator to it.
				 * Firstly, checks if there is previous entry to move in bucket.
				 * If so, it will return iterator on it. In other case, starts searching for the previous
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Iterator before bucket's end will be returned.
				 * Implementation does not check if the previous entry exists!
				 * In other words, if you are out of bounds, the exception will not be thrown
				 * and you will have segmentation fault. It is up to you to check bounds.
				 * @return iterator to the previous entry
				 */
 
                inline iterator operator--() noexcept {
                    if (it == bucket_it->begin()) {
                        while (bucket_it != first_bucket_it && (bucket_it - 1)->is_empty())
                            --bucket_it;
 
                        if (bucket_it != first_bucket_it) {
                            it = std::prev((--bucket_it)->end());
                        } else if (it != first_bucket_it->begin()) {
                            it = std::prev(bucket_it->end());
                        }
                    } else { --it; }
 
                    return *this;
                }
            };
 
        private:
 
			/**
			 * Simplified way to create iterator
			 * @param it current bucket's iterator
			 * @param hashed_ind key's hashed index or bucket's index
			 */
 
            [[nodiscard]] constexpr inline iterator iter(
                    linked_list<std::pair<K, V>>::iterator it,
                    const std::size_t hashed_ind
            ) noexcept {
                return iterator(
                        it,
                        bucket_iterator(hashed_ind, buckets.begin()),
                        bucket_iterator(0, buckets.begin()),
                        bucket_iterator(buckets.get_size(), buckets.end())
                );
            }
 
        public:
 
			// ############################ Const Iterator ############################
 
			/**
			 * Bidirectional iterator to iterate over map's entries.
			 * Not able to modify its value
			 */
 
            class const_iterator {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::pair<K, V>;
                using pointer = const value_type*;
                using reference = const value_type&;
 
            private:
				friend class hash_map<K, V, H, KEq>;
 
				/** Iterator over current bucket */
                linked_list<std::pair<K, V>>::const_iterator it;
 
				/** Iterator to current bucket */
                const_bucket_iterator bucket_it;
 
				/** Iterator to first bucket */
                const const_bucket_iterator first_bucket_it;
 
				/** Iterator to buckets' end */
                const const_bucket_iterator end_buckets_it;
 
				/**
				 * Creates new iterator
				 * @param it current bucket's iterator
				 * @param bucket_it iterator to current bucket
				 * @param first_bucket_it iterator to first bucket
				 * @param end_buckets_it iterator to buckets' end
				 */
 
				const_iterator(
						const linked_list<std::pair<K, V>>::const_iterator it,
						const const_bucket_iterator bucket_it,
						const const_bucket_iterator first_bucket_it,
						const const_bucket_iterator end_buckets_it
				) noexcept : it(it), bucket_it(bucket_it), first_bucket_it(first_bucket_it), end_buckets_it(end_buckets_it) {};
 
			public:
                ~const_iterator() noexcept = default;
 
				// --------------- Dereference operators ---------------
 
                inline reference operator*() const noexcept { return *it; }
                inline pointer operator->() const noexcept { return &*it; }
 
				// --------------- Validation operators ---------------
 
				constexpr inline explicit operator bool() const noexcept { return it != nullptr && it->has_value(); }
                constexpr inline bool operator!() const noexcept { return it == nullptr || !it->has_value(); }
 
				// --------------- Comparison operators ---------------
 
                constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };
 
                constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };
 
				// --------------- Movement operators ---------------
 
				/**
				 * Searches for the next entry and moves iterator to it.
				 * Firstly, checks if there is next entry to move in bucket (not the end of bucket).
				 * If so, it will return iterator on it. In other case, starts searching for the next
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Implementation does not check if the next entry exists! In other words, if you are out of bounds,
				 * the exception will not be thrown and you will have segmentation fault. It is up to you
				 * to check bounds.
				 * @return iterator to the next entry
				 */
 
                inline const_iterator operator++() noexcept {
                    if (std::next(it) == bucket_it->end()) {
                        while (bucket_it + 1 != end_buckets_it && (bucket_it + 1)->is_empty())
                            ++bucket_it;
 
                        it = (bucket_it + 1 == end_buckets_it) ? bucket_it->end() : (++bucket_it)->begin();
                    } else { ++it; }
 
                    return *this;
                }
 
				/**
				 * Searches for the previous entry and moves iterator to it.
				 * Firstly, checks if there is previous entry to move in bucket.
				 * If so, it will return iterator on it. In other case, starts searching for the previous
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Iterator before bucket's end will be returned.
				 * Implementation does not check if the previous entry exists!
				 * In other words, if you are out of bounds, the exception will not be thrown
				 * and you will have segmentation fault. It is up to you to check bounds.
				 * @return iterator to the previous entry
				 */
 
                inline const_iterator operator--() noexcept {
                    if (it == bucket_it->begin()) {
                        while (bucket_it != first_bucket_it && (bucket_it - 1)->is_empty())
                            --bucket_it;
 
                        if (bucket_it != first_bucket_it)
                            it = std::prev((--bucket_it)->end());
                        else if (it != first_bucket_it->begin())
                            it = std::prev(bucket_it->end());
                    } else { --it; }
 
                    return *this;
                }
            };
 
        private:
 
			/**
			 * Simplified way to create constant iterator
			 * @param it current bucket's iterator
			 * @param hashed_ind key's hashed index or bucket's index
			 */
 
            [[nodiscard]] constexpr inline const_iterator const_iter(
                    linked_list<std::pair<K, V>>::const_iterator it,
                    const std::size_t hashed_ind
            ) const noexcept {
                return const_iterator(
                        it,
                        const_bucket_iterator(hashed_ind, buckets.begin()),
                        const_bucket_iterator(0, buckets.begin()),
                        const_bucket_iterator(buckets.get_size(), buckets.end())
                );
            }
 
			// --------------- Unsafe insertions ---------------
 
			/**
			 * Version of insertion that does not checks if value was already
			 * present in the map. However, it checks if load factor is reached and
			 * resizes map, if needed. In total O(hash strategy) with insertion only,
			 * and O(hash strategy + n) with resizing
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 * @return iterator to inserted entry
			 */
 
            inline iterator insert_unchecked(const K& key, const V& value) noexcept {
				++elems;
				const auto is_resized = resize_if_load_factor_reached();
 
                const auto hashed_ind = hasher(key) % buckets.get_size();
                auto& bucket = buckets[hashed_ind];
 
                bucket.push_back(std::make_pair(key, value));
                if (is_resized) ++elems;
 
                return iter(std::prev(bucket.end()), hashed_ind);
            }
 
			/**
			 * Version of insertion that does not checks if value was already
			 * present in the map. However, it checks if load factor is reached and
			 * resizes map, if needed. In total O(hash strategy) with insertion only,
			 * and O(hash strategy + n) with resizing
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 * @return iterator to inserted entry
			 */
 
            inline iterator insert_unchecked(K&& key, V&& value) noexcept {
				++elems;
				const auto is_resized = resize_if_load_factor_reached();
 
				const auto hashed_ind = hasher(key) % buckets.get_size();
				auto& bucket = buckets[hashed_ind];
 
				bucket.push_back(std::make_pair(key, value));
				if (is_resized) ++elems;
 
				return iter(std::prev(bucket.end()), hashed_ind);
            }
 
			/**
			 * Version of insertion that assumes that function was called after
			 * resizing when load factor was reached. No checks (load factor or presence)
			 * are done. Function must be called only after resizing! In total O(hashing strategy)
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */
 
            inline void insert_after_resize(const K& key, const V& value) noexcept {
                const auto hashed_ind = hasher(key) % buckets.get_size();
                auto& bucket = buckets[hashed_ind];
                bucket.push_back(std::make_pair(key, value));
                ++elems;
            }
 
			/**
			 * Version of insertion that assumes that function was called after
			 * resizing when load factor was reached. No checks (load factor or presence)
			 * are done. Function must be called only after resizing! In total O(hashing strategy)
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */
 
            inline void insert_after_resize(K&& key, V&& value) noexcept {
                const auto hashed_ind = hasher(key) % buckets.get_size();
                auto& bucket = buckets[hashed_ind];
                bucket.push_back(std::make_pair(key, value));
                ++elems;
            }
 
        public:
 
			// --------------- Constructors ---------------
 
			/** Creates hash_map with initial number of buckets */
 
            explicit hash_map(const std::size_t initial_number_of_buckets) noexcept {
                buckets = vec<linked_list<std::pair<K, V>>>(initial_number_of_buckets);
                buckets.shrink_to_fit();
            }
 
			/** Creates hash_map with 16 buckets */
 
            hash_map() noexcept = default;

            ~hash_map() noexcept = default;
 
			// --------------- Insertions ---------------
 
			/**
			 * Inserts element or updates present value with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */
 
            inline void insert(const K& key, const V& value) noexcept override {
                const auto hashed_ind = hasher(key) % buckets.get_size();
                linked_list<std::pair<K, V>>& bucket = buckets[hashed_ind];
                auto it = find_by_key(bucket, key);
 
                if (it == bucket.end()) {
					++elems;
 
					if (resize_if_load_factor_reached())
						insert_after_resize(key, value);
					else
						bucket.push_back(std::make_pair(key, value));
                } else {
                    it->second = value;
                }
            }
 
			/**
			 * Inserts element or updates present value with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */
 
            inline void insert(K&& key, V&& value) noexcept override {
				const auto hashed_ind = hasher(key) % buckets.get_size();
				auto& bucket = buckets[hashed_ind];
				auto it = find_by_key(bucket, key);
 
				if (it == bucket.end()) {
					++elems;
 
					if (resize_if_load_factor_reached())
						insert_after_resize(key, value);
					else
						bucket.push_back(std::make_pair(key, value));
				} else {
					it->second = value;
				}
            }
 
			/**
			 * Inserts element or updates present entry with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param entry key and value to insert
			 */
 
            inline void insert(const std::pair<K, V>& entry) noexcept override {
				const auto& key = entry.first;
				const auto& value = entry.second;

				const auto hashed_ind = hasher(key) % buckets.get_size();
				auto& bucket = buckets[hashed_ind];
				auto it = find_by_key(bucket, key);

				if (it == bucket.end()) {
					++elems;

					if (resize_if_load_factor_reached())
						insert_after_resize(key, value);
					else
						bucket.push_back(std::make_pair(key, value));
				} else {
					it->second = value;
				}
            }
 
			/**
			 * Inserts element or updates present entry with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param entry key and value to insert
			 */
 
            inline void insert(std::pair<K, V>&& entry) noexcept override {
				const auto& key = entry.first;
				const auto& value = entry.second;

				const auto hashed_ind = hasher(key) % buckets.get_size();
				auto& bucket = buckets[hashed_ind];
				auto it = find_by_key(bucket, key);

				if (it == bucket.end()) {
					++elems;

					if (resize_if_load_factor_reached())
						insert_after_resize(key, value);
					else
						bucket.push_back(std::make_pair(key, value));
				} else {
					it->second = value;
				}
            }
 
			// --------------- Searching ---------------
 
			/**
			 * Searches for entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return iterator to entry or end iterator if not present
			 */
 
			[[nodiscard]] inline iterator find(const K& key) noexcept {
                const auto hashed_ind = hasher(key) % buckets.get_size();
                auto& bucket = buckets[hashed_ind];
                const auto it = find_by_key(bucket, key);
                return it == bucket.end() ? end() : iter(it, hashed_ind);
            }
 
			/**
			 * Searches for entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return iterator to entry or end iterator if not present
			 */
 
            [[nodiscard]] inline const_iterator find(const K& key) const noexcept {
                const auto hashed_ind = hasher(key) % buckets.get_size();
                const auto& bucket = buckets[hashed_ind];
                const auto it = find_by_key(bucket, key);
                return it == bucket.end() ? end() : const_iter(it, hashed_ind);
            }
 
			/**
			 * Gets entry with given key or creates
			 * with default key if not present.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return reference to value
			 */
 
            inline V& operator[] (const K& key) noexcept override {
                auto it = find(key);
 
                if (it == end())
                    it = insert_unchecked(key, V());
 
                return it->second;
            }
 
			// --------------- Removing ---------------
 
			/**
			 * Removes entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return true if value was removed
			 */
 
            inline bool remove(const K& key) noexcept override {
                const auto iter = find(key);
 
                if (iter == end())
                    return false;
 
                remove(iter);
                return true;
            }
 
			/**
			 * Removes entry by iterator.
			 * In total O(1) in all cases
			 * @param iter entry's iterator
			 */
 
            inline void remove(const iterator iter) noexcept {
                iter.bucket_it->erase(iter.it);
                --elems;
            }
 
			// --------------- Iterators ---------------
 
			/**
			 * Gets iterator to the first entry, if present.
			 * In worst case O(k), where k is the current number of buckets.
			 * @return iterator to the first entry or end iterator
			 * if there are no elements in map
			 */
 
			[[nodiscard]] inline iterator begin() noexcept {
				if (elems == 0)
					return end();
 
                auto try_begin_it = iter(buckets[0].begin(), 0);
 
                if (buckets[0].is_empty())
                    ++try_begin_it;
 
                return try_begin_it;
            }
 
			/**
			 * Gets iterator to the first entry, if present.
			 * In worst case O(k), where k is the current number of buckets.
			 * @return iterator to the first entry or end iterator
			 * if there are no elements in map
			 */
 
            [[nodiscard]] inline const_iterator begin() const noexcept {
				if (elems == 0)
					return end();
 
                auto try_begin_it = const_iter(buckets[0].begin(), 0);
 
                if (buckets[0].is_empty())
                    ++try_begin_it;
 
                return try_begin_it;
            }
 
			/**
			 * Gets last iterator that is out of bounds
			 * and does not have a value. Equality to this iterator
			 * means end of map.
			 * @return out of bounds iterator
			 */
 
			[[nodiscard]] constexpr inline iterator end() noexcept {
                return iter(buckets[buckets.get_size() - 1].end(), buckets.get_size() - 1);
            }
 
			/**
			 * Gets last iterator that is out of bounds
			 * and does not have a value. Equality to this iterator
			 * means end of map.
			 * @return out of bounds iterator
			 */
 
			[[nodiscard]] constexpr inline const_iterator end() const noexcept {
                return const_iter(buckets[buckets.get_size() - 1].end(), buckets.get_size() - 1);
            }
 
			/** Gets current number of elements in map */
 
			[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return elems; }
        };

		template <typename T, typename H = std::hash<T>, typename KEq = std::equal_to<T>> class hash_set :
				public set<T>, public extendable<T> {
			struct none {};

			using map_wrapper = hash_map<T, none, H, KEq>;
			map_wrapper wrapper;

		public:
			class const_iterator;

			// ############################ Iterator ############################

			/** Bidirectional iterator to iterate over map's entries */

			class iterator {
			public:
				using iterator_category = std::bidirectional_iterator_tag;
				using difference_type = std::ptrdiff_t;
				using value_type = T;
				using pointer = value_type*;
				using reference = value_type&;

			private:
				friend class hash_set<T, H, KEq>;

				/** Map's iterator */
				map_wrapper::iterator it;

				/**
				 * Creates new iterator from map's iterator
				 * @param it current bucket's iterator
				 */

				explicit iterator(map_wrapper::iterator it) noexcept : it(it) {};

			public:
				~iterator() noexcept = default;

				// --------------- Dereference operators ---------------

				inline reference operator*() const noexcept { return *it; }
				inline pointer operator->() const noexcept { return &*it; }

				// --------------- Validation operators ---------------

				constexpr inline explicit operator bool() const noexcept { return it; }
				constexpr inline bool operator!() const noexcept { return !it; }

				// --------------- Comparison operators ---------------

				constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
				constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };

				constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
				constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };

				// --------------- Movement operators ---------------

				/**
				 * Searches for the next entry and moves iterator to it.
				 * Firstly, checks if there is next entry to move in bucket (not the end of bucket).
				 * If so, it will return iterator on it. In other case, starts searching for the next
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Implementation does not check if the next entry exists! In other words, if you are out of bounds,
				 * the exception will not be thrown and you will have segmentation fault. It is up to you
				 * to check bounds.
				 * @return iterator to the next entry
				 */

				inline iterator operator++() noexcept {
					++it;
					return *this;
				}

				/**
				 * Searches for the previous entry and moves iterator to it.
				 * Firstly, checks if there is previous entry to move in bucket.
				 * If so, it will return iterator on it. In other case, starts searching for the previous
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Iterator before bucket's end will be returned.
				 * Implementation does not check if the previous entry exists!
				 * In other words, if you are out of bounds, the exception will not be thrown
				 * and you will have segmentation fault. It is up to you to check bounds.
				 * @return iterator to the previous entry
				 */

				inline iterator operator--() noexcept {
					--it;
					return *this;
				}
			};

		public:

			// ############################ Const Iterator ############################

			/**
			 * Bidirectional iterator to iterate over map's entries.
			 * Not able to modify its value
			 */

			class const_iterator {
			public:
				using iterator_category = std::bidirectional_iterator_tag;
				using difference_type = std::ptrdiff_t;
				using value_type = T;
				using pointer = const value_type*;
				using reference = const value_type&;

			private:
				friend class hash_set<T, H, KEq>;

				/** Map's iterator */
				map_wrapper::const_iterator it;

				/**
				 * Creates new iterator from map's iterator
				 * @param it current bucket's iterator
				 */

				explicit const_iterator(map_wrapper::const_iterator it) noexcept : it(it) {};

			public:
				~const_iterator() noexcept = default;

				// --------------- Dereference operators ---------------

				inline reference operator*() const noexcept { return *it; }
				inline pointer operator->() const noexcept { return &*it; }

				// --------------- Validation operators ---------------

				constexpr inline explicit operator bool() const noexcept { return it; }
				constexpr inline bool operator!() const noexcept { return !it; }

				// --------------- Comparison operators ---------------

				constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
				constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };

				constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
				constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };

				// --------------- Movement operators ---------------

				/**
				 * Searches for the next entry and moves iterator to it.
				 * Firstly, checks if there is next entry to move in bucket (not the end of bucket).
				 * If so, it will return iterator on it. In other case, starts searching for the next
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Implementation does not check if the next entry exists! In other words, if you are out of bounds,
				 * the exception will not be thrown and you will have segmentation fault. It is up to you
				 * to check bounds.
				 * @return iterator to the next entry
				 */

				inline const_iterator operator++() noexcept {
					++it;
					return *this;
				}

				/**
				 * Searches for the previous entry and moves iterator to it.
				 * Firstly, checks if there is previous entry to move in bucket.
				 * If so, it will return iterator on it. In other case, starts searching for the previous
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Iterator before bucket's end will be returned.
				 * Implementation does not check if the previous entry exists!
				 * In other words, if you are out of bounds, the exception will not be thrown
				 * and you will have segmentation fault. It is up to you to check bounds.
				 * @return iterator to the previous entry
				 */

				inline const_iterator operator--() noexcept {
					--it;
					return *this;
				}
			};

		public:

			// --------------- Constructors ---------------

			/** Creates hash_set with initial number of buckets */

			explicit hash_set(const std::size_t initial_number_of_buckets) noexcept {
				wrapper = map_wrapper(initial_number_of_buckets);
			}

			/** Creates hash_set with 16 buckets */

			hash_set() noexcept = default;
			~hash_set() noexcept = default;

			// --------------- Insertions ---------------

			/**
			 * Inserts element to the wrapper.
			 * Checks if load factor is reached and resizes wrapper, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param value value to insert
			 */

			inline void insert(const T& value) noexcept override {
				wrapper.insert(value, none());
			}

			/**
			 * Inserts element to the wrapper.
			 * Checks if load factor is reached and resizes wrapper, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param value value to insert
			 */

			inline void insert(T&& value) noexcept override {
				wrapper.insert(std::make_pair(value, none()));
			}

			/**
			 * Inserts element to the wrapper.
			 * Checks if load factor is reached and resizes wrapper, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param value value to insert
			 */

			inline void add(const T& value) noexcept override { insert(value); }

			/**
			 * Inserts element to the wrapper.
			 * Checks if load factor is reached and resizes wrapper, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param value value to insert
			 */

			inline void add(T&& value) noexcept override { insert(value); }

			/**
			 * Inserts elements to the wrapper.
			 * Checks if load factor is reached and resizes wrapper, if needed.
			 * In total O((hash strategy + n) * k) in all cases,
			 * where n is current size and k is number of values.
			 * @param value value to insert
			 */

			inline void insert(const std::initializer_list<T>& values) noexcept {
				this->extend(values.begin(), values.end());
			}

			/**
			 * Inserts elements to the wrapper.
			 * Checks if load factor is reached and resizes wrapper, if needed.
			 * In total O((hash strategy + n) * k) in all cases,
			 * where n is current size and k is number of values.
			 * @param value value to insert
			 */

			inline void insert(std::initializer_list<T>&& values) noexcept {
				this->extend(values.begin(), values.end());
			}

			// --------------- Searching ---------------

			/**
			 * Searches for entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param value value to search
			 * @return iterator to entry or end iterator if not present
			 */

			[[nodiscard]] inline iterator find(const T& value) noexcept {
				return iterator(wrapper.find(value));
			}

			/**
			 * Searches for entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param value value to search
			 * @return iterator to entry or end iterator if not present
			 */

			[[nodiscard]] inline const_iterator find(const T& value) const noexcept {
				return const_iterator(wrapper.find(value));
			}

			// --------------- Removing ---------------

			/**
			 * Removes entry with given value.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param value entry's value to search
			 * @return true if value was removed
			 */

			inline bool remove(const T& value) noexcept override {
				return wrapper.remove(value);
			}

			/**
			 * Removes entry by iterator.
			 * In total O(1) in all cases
			 * @param iter entry's iterator
			 */

			inline void remove(const iterator iter) noexcept {
				wrapper.remove(iter.it);
			}

			// --------------- Iterators ---------------

			/**
			 * Gets iterator to the first entry, if present.
			 * In worst case O(k), where k is the current number of buckets.
			 * @return iterator to the first entry or end iterator
			 * if there are no elements in set
			 */

			[[nodiscard]] inline iterator begin() noexcept {
				return iterator(wrapper.begin());
			}

			/**
			 * Gets iterator to the first entry, if present.
			 * In worst case O(k), where k is the current number of buckets.
			 * @return iterator to the first entry or end iterator
			 * if there are no elements in set
			 */

			[[nodiscard]] inline const_iterator begin() const noexcept {
				return const_iterator(wrapper.begin());
			}

			/**
			 * Gets last iterator that is out of bounds and does not have a value.
			 * Equality to this iterator means end of set.
			 * @return out of bounds iterator
			 */

			[[nodiscard]] constexpr inline iterator end() noexcept {
				return iterator(wrapper.end());
			}

			/**
			 * Gets last iterator that is out of bounds and does not have a value.
			 * Equality to this iterator means end of set.
			 * @return out of bounds iterator
			 */

			[[nodiscard]] constexpr inline const_iterator end() const noexcept {
				return const_iterator(wrapper.end());
			}

			/** Gets current number of elements in set */

			[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return wrapper.get_size(); }
		};
    }
 
    namespace utils {
		/**
		 * Converts `$number` string to long double
		 * @param cost_str string to convert
		 * @return parsed cost
		 */
 
        inline long double parse_cost(const std::string& cost_str) noexcept {
            std::string clone(cost_str.size(), '0');
            std::copy(cost_str.begin(), cost_str.end(), clone.begin());
            clone.replace(clone.find('$'), 1, "");
            return std::stold(clone);
        }
 
		/**
		 * Converts float128 to string with at least 1 number in mantissa part.
		 * Example: assert(to_string_with_mantissa(45.0) = std::string("45.0"))
		 * @param ld float128 to convert to string
		 */
 
        inline std::string to_string_with_mantissa(const long double ld) noexcept {
            std::stringstream out;
            out << ld;
 
            const auto& str = out.str();
 
            if (str.find('.') == std::string::npos)
                out << ".0";
 
            return out.str();
        }
    }
}

int main() {
	int n = 0, k = 0;
	std::cin >> n >> k;

	dsa_hw::hash::hash_set<std::string> delimiters;
	dsa_hw::hash::hash_map<std::string, std::string> open_to_close_delimiters;

	for (int i = 0; i < n; ++i) {
		std::string start, end;
		std::cin >> start >> end;
		delimiters.insert({start, end});
		open_to_close_delimiters.insert(std::make_pair(start, end));
		open_to_close_delimiters[start] = end;
	}

	dsa_hw::linked_stack<std::string> open_delimiters_stack;
	std::size_t column = 0;

	const auto is_simple_word = [&delimiters](const std::string& word) {
		return delimiters.find(word) == delimiters.end();
	};

	const auto is_close_delimiter = [&open_to_close_delimiters](const std::string& delim) {
		return open_to_close_delimiters.find(delim) == open_to_close_delimiters.end();
	};

	for (int i = 1; i <= k; ++i) {
		std::string input, word;
		std::getline(std::cin >> std::ws, input);
		std::stringstream line;
		line << input;

		for (column = 1; line >> word; column += word.length() + 1) {
			if (is_simple_word(word))
				continue;

			if (open_delimiters_stack.is_empty()) {

				// if close delimiter and empty stack, error
				if (is_close_delimiter(word)) {
					std::printf(
							"Error in line %d, column %zu: unexpected closing token %s.",
							i,
							column,
							word.c_str()
					);
					return 0;
				}

				// if open, add to stack
				open_delimiters_stack.push(word);
				continue;
			}

			// stack is not empty
			// if close delimiter, check if matches with stack open one

			if (is_close_delimiter(word)) {
				const auto& close_delim = open_to_close_delimiters[open_delimiters_stack.top()];

				// not matches with open delimiter in stack
				if (close_delim != word) {
					std::printf(
							"Error in line %d, column %zu: expected %s but got %s.",
							i,
							column,
							close_delim.c_str(),
							word.c_str()
					);
					return 0;
				}

				// matches with open delimiter in stack
				// remove open one from stack
				open_delimiters_stack.pop();
				continue;
			}

			// open delimiter, add to stack
			open_delimiters_stack.push(word);
		}
	}

	// if there are some unclosed delimiters
	if (!open_delimiters_stack.is_empty()) {
		std::printf(
				"Error in line %d, column %zu: expected %s but got end of input.",
				k,
				column,
				open_to_close_delimiters[open_delimiters_stack.pop()].c_str()
		);
		return 0;
	}

	std::puts("The input is properly balanced.");
	return 0;
}
