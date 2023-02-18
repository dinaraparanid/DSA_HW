// Arseny Savchenko

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <utility>

/**
 * To Alaa or one who is checking the homework:
 *
 * In this homework, I tried to implement all the structures
 * that have been studied in the course (except for the queue)
 * and also learn something new for myself. I haven't written in C++ for 2 years,
 * so my code style may be rusty. This is far from my main languages with functional style.
 * It was quite informative to come back after 2 years of active development
 * experience in tons of technologies and languages.
 *
 * For the most part, I am satisfied with what I wrote, but, unfortunately,
 * I am running out of time (I wrote it in 2 days) for a really high-quality implementation
 * (I have not implemented many things (such as reverse iterators),
 * many of them also had not been tested).
 *
 * I'm not chasing marks, besides, I absolutely don't care how it's going to be graded,
 * because the experience I've gained is worth a lot more than `titles` and stupid grades to me.
 *
 * I hope you will like some aspects of the implementation, because most of them were designed
 * and implemented exclusively by me. There are also a some good-to-known things in the code,
 * which I found during the process of solving problems with the recycling of structures.
 *
 * In any case, thanks to the DSA team for the opportunity to implement everything as planned!
 * And I apologize for stealing such a valuable time...
 */

namespace dsa_hw {

	// ############################ Extendable ############################

	/** Interface to add multiple values */

	template <typename T> struct extendable {
		/**
		 * Adds element to the structure.
		 * See implementation details of your DS for complexity.
		 * @param elem element to insert
		 */

		virtual void add(const T& elem) noexcept = 0;

		/**
		 * Adds element to the structure.
		 * See implementation details of your DS for complexity.
		 * @param elem element to insert
		 */

		virtual void add(T&& elem) noexcept = 0;

		/**
		 * Adds multiple elements to the structure.
		 * See implementation details of your DS for complexity.
		 * @param begin begin iterator of DS
		 * @param end end iterator of DS (out of bounds iterator)
		 */

		template <typename ForwardIterator> void extend(
				const ForwardIterator begin,
				const ForwardIterator end
		) noexcept {
			for (auto it = begin; it != end; ++it)
				add(*it);
		}
	};

	// ############################ List ADL ############################

	/**
	 * List is an ADL that can add elements to the end
	 * or remove them from the end. Implements extendable,
	 * so it is allowed to push multiple elements to the end.
	 * Also it provides basic information with size and back / front elements.
	 * See implementations of the List ADL (vec and linked_list) for more details.
	 */

	template <typename T> struct list : public extendable<T> {

		// ------------------ Inserting ------------------

		/**
		 * Add element to the end.
		 * @param elem element to insert
		 */

		virtual void push_back(const T& elem) noexcept = 0;

		/**
		 * Add element to the end.
		 * @param elem element to insert
		 */

		virtual void push_back(T&& elem) noexcept = 0;

		/**
		 * Add element to the end by calling push_back().
		 * @param elem element to insert
		 */

		inline void add(const T& elem) noexcept override { push_back(elem); }

		/**
		 * Add element to the end by calling push_back().
		 * @param elem element to insert
		 */

		inline void add(T&& elem) noexcept override { push_back(elem); }

		// ------------------ Removing ------------------

		/**
		 * Removes element from the end.
		 * @throw std::out_of_range() if list was empty
		 * @return reference to the removed element
		 */

		virtual T& pop_back() noexcept = 0;

		/** Removes all elements from the list */

		virtual void clear() noexcept = 0;

		/** @return number of elements in the list */

		// ------------------ Size Checking ------------------

		[[nodiscard]] virtual constexpr std::size_t get_size() const noexcept = 0;

		/**
		 * Is list empty?
		 * @return true if there are no elements in the list
		 */

		[[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }

		/**
		 * Is list empty?
		 * @return true if there at least one element in the list
		 */

		[[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return get_size() != 0; }

		// ------------------ Elements Accessing ------------------

		/** @return reference to the last element of list */

		[[nodiscard]] virtual constexpr T& back() noexcept = 0;

		/** @return reference to the last element of list */

		[[nodiscard]] virtual constexpr const T& back() const noexcept = 0;

		/** @return reference to the first element of list */

		[[nodiscard]] virtual constexpr T& front() noexcept = 0;

		/** @return reference to the first element of list */

		[[nodiscard]] virtual constexpr const T& front() const noexcept = 0;
	};

	// ############################ Vector ############################

	/**
	 * Vector is an implementation of dynamic array
	 * with all List ADL operations (adding, removing, front-back accessing, etc.).
	 * It dynamically allocates memory and reallocates of it is reached.
	 * It has parameter `capacity` which represents the maximum number of elements
	 * that can vector can have before reallocation (default is 16). It should be always
	 * power of 2 (except if shrink_to_fit() was called)
	 * In case of insertion of new element, if capacity is reached,
	 * new buffer with doubled capacity is created and used.
	 * In case of remove, if current size is 4 times less
	 * (or even more less) than current capacity, current buffer is replaced
	 * with new buffer with capacity 2 times less.
	 *
	 * @param T type of element. Must have default constructor (with no fields)
	 * @param A allocator that is used to create space for new element
	 * and creation of default values. By default, std::allocator<T> is used
	 *
	 * -------------- Complexity --------------
	 * |  Method   | Average case | Worst case |
	 * |push_back()|    O(1)      |    O(n)    |
	 * |insert(pos)|    O(n-pos)  |    O(n)    |
	 * |pop_back() |    O(1)      |    O(n)    |
	 * |erase(pos) |    O(n-pos)  |    O(n)    |
	 * | clear()   |    O(n)      |    O(n)    |
	 * | get(ind)  |    O(1)      |    O(1)    |
	 * | begin()   |    O(1)      |    O(1)    |
	 * |  end()    |    O(1)      |    O(1)    |
	 */

	template <typename T, typename A = std::allocator<T>> class vec : public list<T> {

		// ------------------ Fields ------------------

		A allocator;
		using type_traits = std::allocator_traits<A>;

		/** Actual buffer that is used for elements */
		T* data;

		/** Actual allocated space for the buffer */
		std::size_t capacity = 16;

		/** Number of elements in vector */
		std::size_t size = 0;

		// ------------------ Capacity Utils ------------------

		/**
		 * Calculates next power of 2 for the given number for O(1).
		 * Implementation from https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
		 * @param value value for which next power of 2 should be calculated
		 * @return 2^k s.t. it >= value
		 */

		[[nodiscard]] static constexpr inline std::size_t get_upper_power_of_two(std::size_t value) noexcept {

			// Example for 8 bit number:
			// 0. 00100100 <- initial number
			// 1. 00100011 <- --value
			// 1. 00010001 <- value >> 1
			// 2. 00110011 <- value |= value >> 1
			// 3. 00001100 <- value >> 2
			// 4. 00111111 <- value |= value >> 2
			// 5. 00000011 <- value >> 4
			// 6. 00111111 <- value |= value >> 4
			// 7. 01000000 <- ++value

			--value;
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			value |= value >> 32;
			++value;

			return value;
		}

		/**
		 * Calculates new capacity for the given size.
		 * Complexity is O(1).
		 * @param size new size from which capacity is calculated
		 * @return 2^k s.t. it >= size
		 */

		[[nodiscard]] static constexpr inline std::size_t get_capacity_for(const std::size_t size) noexcept {
			return get_upper_power_of_two(size);
		}

		/**
		 * Initializes capacity with get_capacity_for(size).
		 * Complexity is O(1).
		 */

		inline void init_capacity() noexcept { capacity = get_capacity_for(size); }

		// ------------------ Resize Utils ------------------

		/**
		 * Reallocates buffer with new desired capacity.
		 * In case if desired_capacity is less or equal to the current size,
		 * copies only first desired_capacity elements to new buffer
		 * and set size = desired_capacity.
		 * In case if desired_capacity is greater, does not initialize new memory
		 * and clones only ones that were present.
		 * Size (number of elements) is not changed.
		 * Complexity is O(min(desired_capacity, size))
		 * @param desired_capacity capacity that will be used to reallocate elements.
		 * In case if this capacity is less than size, size becomes equal to it.
		 */

		inline void realloc(const std::size_t desired_capacity) noexcept {
			const auto old_capacity = capacity;
			capacity = get_capacity_for(desired_capacity);
			size = std::min(desired_capacity, size);

			auto* const new_buffer = type_traits::allocate(allocator, capacity);

			for (auto* data_ptr = data, *buf_ptr = new_buffer; data_ptr != data + size; ++data_ptr, ++buf_ptr) {
				// default constructor is used to create objects
				type_traits::construct(allocator, buf_ptr);
				std::move(data_ptr, data_ptr + 1, buf_ptr);
			}

			// old buffer is deallocated
			// no destruction is required because of std::move()
			type_traits::deallocate(allocator, data, old_capacity);
			data = new_buffer;
		}

		/** Checks if size is greater or equal to capacity */

		[[nodiscard]] constexpr inline bool is_capacity_reached() const noexcept {
			return size >= capacity;
		}

		/**
		 * Reallocates buffer when size >= capacity.
		 * Complexity is O(min(desired_capacity, size))
		 */

		inline void resize_if_capacity_reached() noexcept {
			if (is_capacity_reached())
				realloc(size);
		}

		/** Checks if size * 4 <= capacity */

		[[nodiscard]] constexpr inline bool is_size_less_than_capacity_four_times() const noexcept {
			return size * 4 <= capacity;
		}

		/**
		 * Makes buffer 2 times less if 3/4 of it is not used.
		 * Complexity is O(n).
		 */

		inline void resize_if_size_less_than_capacity_four_times() noexcept {
			if (is_size_less_than_capacity_four_times()) {
				const auto old_capacity = capacity;
				capacity >>= 1;

				auto* new_buffer = type_traits::allocate(allocator, capacity);

				for (auto* data_ptr = data, *buf_ptr = new_buffer; data_ptr != data + size; ++data_ptr, ++buf_ptr) {
					// default constructor is used to create objects
					type_traits::construct(allocator, buf_ptr);
					std::move(data_ptr, data_ptr + 1, buf_ptr);
				}

				// old buffer is deallocated
				// no destruction is required because of std::move()
				type_traits::deallocate(allocator, data, old_capacity);
				data = new_buffer;
			}
		}

		/** Allocates memory for the buffer */

		inline void init_data() noexcept {
			data = type_traits::allocate(allocator, capacity);
		}

	public:
		using value_type = T;

		// ------------------ Iterators ------------------

		class const_iterator;

		// ############################ Iterator ############################

		/** Random access iterator for vector that allows to modify elements */

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

			/** Initializes iterator with pointer to the element */
			explicit iterator(pointer ptr) noexcept : ptr(ptr) {}

		public:
			~iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			inline reference operator*() const noexcept { return *ptr; }
			inline pointer operator->() const noexcept { return ptr; }

			// --------------- Validation operators ---------------

			[[nodiscard]] constexpr inline explicit operator bool() const noexcept { return ptr; }
			[[nodiscard]] constexpr inline bool operator!() const noexcept { return !ptr; }

			// --------------- Comparison operators ---------------

			[[nodiscard]] constexpr inline bool operator==(const iterator& other) const noexcept { return ptr == other.ptr; };
			[[nodiscard]] constexpr inline bool operator!=(const iterator& other) const noexcept { return ptr != other.ptr; };

			[[nodiscard]] constexpr inline bool operator==(const const_iterator& other) const noexcept { return ptr == other.ptr; };
			[[nodiscard]] constexpr inline bool operator!=(const const_iterator& other) const noexcept { return ptr != other.ptr; };

			// --------------- Movement operators ---------------

			inline iterator operator++() noexcept { ++ptr; return *this; }
			inline iterator operator--() noexcept { --ptr; return *this; }

			[[nodiscard]] constexpr inline iterator operator+(const std::size_t move) const noexcept { return iterator(ptr + move); }
			[[nodiscard]] constexpr inline iterator operator-(const std::size_t move) const noexcept { return iterator(ptr - move); }
		};

		// ############################ Const Iterator ############################

		/** Random access iterator for vector that does not allow to modify elements */

		class const_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = value_type*;
			using reference = const value_type&;

		private:
			friend class vec<T>;
			pointer const ptr;

			/** Initializes iterator with pointer to the element */
			explicit const_iterator(pointer const ptr) noexcept : ptr(ptr) {}

		public:
			~const_iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			inline reference operator*() const noexcept { return *ptr; }
			inline pointer operator->() const noexcept { return ptr; }

			// --------------- Validation operators ---------------

			[[nodiscard]] constexpr inline explicit operator bool() const noexcept { return ptr; }
			[[nodiscard]] constexpr inline bool operator!() const noexcept { return !ptr; }

			// --------------- Comparison operators ---------------

			[[nodiscard]] constexpr inline bool operator==(const iterator& other) const noexcept { return ptr == other.ptr; };
			[[nodiscard]] constexpr inline bool operator!=(const iterator& other) const noexcept { return ptr != other.ptr; };

			[[nodiscard]] constexpr inline bool operator==(const const_iterator& other) const noexcept { return ptr == other.ptr; };
			[[nodiscard]] constexpr inline bool operator!=(const const_iterator& other) const noexcept { return ptr != other.ptr; };

			// --------------- Movement operators ---------------

			inline const_iterator operator++() noexcept { ++ptr; return *this; }
			inline const_iterator operator--() noexcept { --ptr; return *this; }

			[[nodiscard]] constexpr inline const_iterator operator+(const std::size_t move) const noexcept { return iterator(ptr + move); }
			[[nodiscard]] constexpr inline const_iterator operator-(const std::size_t move) const noexcept { return iterator(ptr - move); }
		};

		// --------------- Constructors ---------------

		/** Creates vector with default capacity (16) */

		vec() noexcept {
			capacity = 16;
			init_data();
		}

		/**
		 * Creates vector with given number of elements.
		 * All elements are constructed with default constructors
		 * @param initial_size initial number of elements
		 */

		explicit vec(const std::size_t initial_size) noexcept {
			size = initial_size;
			init_capacity();
			init_data();

			for (auto* p = data; p != data + size; ++p)
				type_traits::construct(allocator, p);
		}

		/**
		 * Creates vector with given number of elements.
		 * All elements are copied from given init element.
		 * @param initial_size initial number of elements
		 * @param init element that will be used to instantiate elements.
		 */

		vec(const std::size_t initial_size, const T& init) noexcept {
			size = initial_size;
			init_capacity();
			init_data();

			for (auto* p = data; p != data + size; ++p)
				*p = init;
		}

		/**
		 * Creates vector with given number of elements.
		 * All elements are constructed with given init function.
		 * @param initial_size initial number of elements
		 * @param init function to construct new elements.
		 * As an optional argument, uses index of an element
		 */

		vec(const std::size_t initial_size, const std::function<T&(std::size_t)> init) noexcept {
			size = initial_size;
			init_capacity();
			init_data();

			for (auto* p = data; p != data + size; ++p)
				*p = init(p - data);
		}

		/**
		 * Creates vector from given elements.
		 * Complexity is O(n) for n assignments.
		 * @param initial_size initial number of elements
		 * @param elems elements to add to vector.
		 */

		vec(const std::initializer_list<T>& elems) noexcept {
			size = elems.size();
			init_capacity();
			init_data();

			for (int i = 0; i < elems.size(); ++i)
				data[i] = elems[i];
		}

		/**
		 * Creates vector from given elements.
		 * Complexity is O(n) for n assignments.
		 * @param initial_size initial number of elements
		 * @param elems elements to add to vector.
		 */

		vec(std::initializer_list<T>&& elems) noexcept {
			size = elems.size();
			init_capacity();
			init_data();

			for (int i = 0; i < elems.size(); ++i)
				data[i] = elems[i];
		}

		~vec() noexcept {
			// Destroys all left elements in vector

			for (auto* p = data; p != data + size; ++p)
				type_traits::destroy(allocator, p);

			// Deallocates buffer
			type_traits::deallocate(allocator, data, capacity);
		}

		// --------------- Inserting ---------------

		/**
		 * Adds element to the end of vector.
		 * If capacity is reached, resizes whole buffer
		 * by all copping elements to the new one.
		 * Complexity is O(1) in average case
		 * and O(n) in worst case
		 * @param elem element to insert to the end of vector
		 */

		inline void push_back(const T& elem) noexcept override {
			++size;
			resize_if_capacity_reached();
			data[size - 1] = elem;
		}

		/**
		 * Adds element to the end of vector.
		 * If capacity is reached, resizes whole buffer
		 * by all copping elements to the new one.
		 * Complexity is O(1) in average case
		 * and O(n) in worst case
		 * @param elem element to insert to the end of vector
		 */

		inline void push_back(T&& elem) noexcept override {
			++size;
			resize_if_capacity_reached();
			data[size - 1] = elem;
		}

		/**
		 * Inserts element to the given position, pointed by iterator.
		 * If capacity is reached, resizes whole buffer
		 * by all copping elements to the new one.
		 * Complexity is O(n - pos) in average case
		 * and O(n + n - pos) in worst case
		 * @param elem element to insert to the vector
		 */

		inline void insert(const const_iterator position, const T& elem) noexcept {
			// Adds to the end and resizes, if necessary
			push_back(elem);

			const auto ind = position.ptr - data;

			// Creates buffer for the element
			auto* tmp = type_traits::allocate(allocator, 1);
			type_traits::construct(allocator, tmp);

			// Swaps until insertion position is reached

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp);
				std::move(data + i - 1, data + i, data + i);
				std::move(tmp, tmp + 1, data - 1);
			}

			// Deallocates temporary element buffer
			allocator.deallocate(tmp, 1);
		}

		/**
		 * Inserts element to the given position, pointed by iterator.
		 * If capacity is reached, resizes whole buffer
		 * by all copping elements to the new one.
		 * Complexity is O(n - pos) in average case
		 * and O(n + n - pos) in worst case
		 * @param elem element to insert to the vector
		 */

		inline void insert(const const_iterator position, T&& elem) noexcept {
			// Adds to the end and resizes, if necessary
			push_back(elem);

			const auto ind = position.ptr - data;

			// Creates buffer for the element
			auto* tmp = type_traits::allocate(allocator, 1);
			type_traits::construct(allocator, tmp);

			// Swaps until insertion position is reached

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp);
				std::move(data + i - 1, data + i, data + i);
				std::move(tmp, tmp + 1, data - 1);
			}

			// Deallocates temporary element buffer
			allocator.deallocate(tmp, 1);
		}

		/**
		 * Inserts element to the given position, pointed by iterator.
		 * If capacity is reached, resizes whole buffer
		 * by all copping elements to the new one.
		 * Complexity is O(n - pos) in average case
		 * and O(n + n - pos) in worst case
		 * @param elem element to insert to the vector
		 */

		inline void insert(const iterator position, const T& elem) noexcept {
			// Adds to the end and resizes, if necessary
			push_back(elem);

			const auto ind = position.ptr - data;

			// Creates buffer for the element
			auto* tmp = type_traits::allocate(allocator, 1);
			type_traits::construct(allocator, tmp);

			// Swaps until insertion position is reached

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp);
				std::move(data + i - 1, data + i, data + i);
				std::move(tmp, tmp + 1, data - 1);
			}

			// Deallocates temporary element buffer
			allocator.deallocate(tmp, 1);
		}

		/**
		 * Inserts element to the given position, pointed by iterator.
		 * If capacity is reached, resizes whole buffer
		 * by all copping elements to the new one.
		 * Complexity is O(n - pos) in average case
		 * and O(n + n - pos) in worst case
		 * @param elem element to insert to the vector
		 */

		inline void insert(const iterator position, T&& elem) noexcept {
			// Adds to the end and resizes, if necessary
			push_back(elem);

			const auto ind = position.ptr - data;

			// Creates buffer for the element
			auto* tmp = type_traits::allocate(allocator, 1);
			type_traits::construct(allocator, tmp);

			// Swaps until insertion position is reached

			for (int i = size - 1; i > ind; ++i) {
				std::move(data + i, data + i + 1, tmp);
				std::move(data + i - 1, data + i, data + i);
				std::move(tmp, tmp + 1, data - 1);
			}

			// Deallocates temporary element buffer
			allocator.deallocate(tmp, 1);
		}

		// --------------- Removing ---------------

		/**
		 * Removes last element from the vector.
		 * Makes buffer 2 times less if 3/4 of it is not used.
		 * Complexity is O(1) in average case,
		 * and O(n) in worst case
		 * @throw std::out_of_range() if vector is empty.
		 * @return reference to the removed element
		 */

		inline T& pop_back() noexcept override {
			if (size == 0)
				throw std::out_of_range("Vec is empty");

			resize_if_size_less_than_capacity_four_times();

			auto& back = data[size - 1];
			--size;
			return back;
		}

		/**
		 * Removes element from the vector by given position.
		 * Makes buffer 2 times less if 3/4 of it is not used.
		 * Complexity is O(n - k) in average case,
		 * and O(n + n - k) in worst case.
		 * @throw std::out_of_range() if vector is empty.
		 * @throw std::out_of_range() if index is out of range
		 * @return reference to the removed element
		 */

		inline void erase(const const_iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("Vec is empty");

			if (position.ptr >= data + size)
				throw std::out_of_range("Index out of range");

			resize_if_size_less_than_capacity_four_times();

			const auto ind = position.ptr - data;

			// Creates buffer for the element
			auto* tmp = type_traits::allocate(allocator, 1);
			type_traits::construct(allocator, tmp);

			// Swaps until last element is reached

			for (int i = ind + 1; i < size; ++i) {
				std::move(data + i, data + i + 1, tmp);
				std::move(data + i - 1, data + i, data + i);
				std::move(tmp, tmp + 1, data - 1);
			}

			// Removes element from the end
			pop_back();

			// Deallocates buffer for the element
			allocator.deallocate(tmp, 1);
		}

		/**
		 * Removes element from the vector by given position.
		 * Makes buffer 2 times less if 3/4 of it is not used.
		 * Complexity is O(n - k) in average case,
		 * and O(n + n - k) in worst case.
		 * @throw std::out_of_range() if vector is empty.
		 * @throw std::out_of_range() if index is out of range
		 * @return reference to the removed element
		 */

		inline void erase(const iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("vec is empty");

			resize_if_size_less_than_capacity_four_times();

			const auto ind = position.ptr - data;

			// Creates buffer for the element
			auto* tmp = type_traits::allocate(allocator, 1);
			type_traits::construct(allocator, tmp);

			// Swaps until last element is reached

			for (int i = ind + 1; i < size; ++i) {
				std::move(data + i, data + i + 1, tmp);
				std::move(data + i - 1, data + i, data + i);
				std::move(tmp, tmp + 1, data - 1);
			}

			// Removes element from the end
			pop_back();

			// Deallocates buffer for the element
			allocator.deallocate(tmp, 1);
		}

		/**
		 * Removes all elements from the vector.
		 * After that vector will take its default form (capacity is 16, size is 0).
		 * WARNING: destroying all elements
		 * means that all references / pointers
		 * to the elements of vector will become dangling (not valid).
		 * Complexity is O(n).
		 */

		inline void clear() noexcept override {
			for (auto* p = data; p != data + size; ++p)
				type_traits::destroy(allocator, p);

			allocator.deallocate(data, capacity);
			size = 0;
			capacity = 16;
			init_data();
		}

		// --------------- Reallocating ---------------

		/**
		 * Reallocates buffer with new desired capacity.
		 * In case if desired_capacity is less or equal to the current size,
		 * copies only first desired_capacity elements to new buffer
		 * and set size = desired_capacity.
		 * In case if desired_capacity is greater,
		 * clones only ones that were present
		 * and initializes new memory with default constructors.
		 * Size (number of elements) is reset to new_size.
		 * Complexity is O(max(new_size, size))
		 * @param new_size desired number of elements
		 */

		inline void resize(const std::size_t new_size) noexcept {
			realloc(new_size);

			if (size < new_size) {
				for (auto* p = data + size; p != data + new_size; ++p)
					type_traits::construct(allocator, p);

				size = new_size;
			}
		}

		/**
		 * Reallocates buffer with new desired size.
		 * In case if new_size is less or equal to the current capacity, does nothing.
		 * In case if desired_capacity is greater, does not initialize new memory
		 * and clones only ones that were present.
		 * Size (number of elements) is not changed.
		 * Complexity is O(min(capacity(new_size), size))
		 * @param new_size desired number of elements
		 */

		inline void reserve(const std::size_t new_size) noexcept {
			if (new_size > capacity)
				realloc(new_size);
		}

		/**
		 * Resizes vector to have buffer with
		 * capacity exactly equal to its size.
		 * Complexity is O(n).
		 */

		inline void shrink_to_fit() noexcept {
			const auto old_capacity = capacity;
			capacity = size;
			auto* new_buffer = type_traits::allocate(allocator, capacity);

			for (auto* data_ptr = data, *buf_ptr = new_buffer; data_ptr != data + size; ++data_ptr, ++buf_ptr) {
				type_traits::construct(allocator, buf_ptr);
				std::move(data_ptr, data_ptr + 1, buf_ptr);
			}

			allocator.deallocate(data, old_capacity);
			data = new_buffer;
		}

		// --------------- Element Accessing ---------------

		/**
		 * Gets element of the vector without index checking.
		 * Complexity is O(1).
		 *
		 * P.S. I DO HATE this c++ vector[] implementation
		 * with segfault by default. The design is completely awful,
		 * because many developers does not think about at() method and all that stuff.
		 * (however, once it saved my life at school olympiad, but that's a long story to tell...)
		 * In Rust language they have implemented safe version by default and
		 * unsafe for those who really wants it: https://doc.rust-lang.org/std/primitive.slice.html#method.get_unchecked
		 *
		 * @param ind index of element
		 * @return reference to the element
		 */

		[[nodiscard]] constexpr inline T& get_unchecked(const std::size_t ind) noexcept { return data[ind]; }

		/**
		 * Gets element of the vector without index checking.
		 * Complexity is O(1).
		 *
		 * P.S. I DO HATE this c++ vector[] implementation
		 * with segfault by default. The design is completely awful,
		 * because many developers does not think about at() method and all that stuff.
		 * (however, once it saved my life at school olympiad, but that's a long story to tell...)
		 * In Rust language they have implemented safe version by default and
		 * unsafe for those who really wants it: https://doc.rust-lang.org/std/primitive.slice.html#method.get_unchecked
		 *
		 * @param ind index of element
		 * @return constant reference to the element
		 */

		[[nodiscard]] constexpr inline const T& get_unchecked(const std::size_t ind) const noexcept { return data[ind]; }

		/**
		 * Gets element of the vector.
		 * Complexity is O(1).
		 * @param ind index of element
		 * @throw std::out_of_range if index is out of range.
		 * @return reference to the element
		 */

		[[nodiscard]] inline T& operator[](const std::size_t ind) noexcept {
			if (ind >= size)
				throw std::out_of_range("Vec's index out of range");

			return get_unchecked(ind);
		}

		/**
		 * Gets element of the vector.
		 * Complexity is O(1).
		 * @param ind index of element
		 * @throw std::out_of_range if index is out of range.
		 * @return constant reference to the element
		 */

		[[nodiscard]] inline const T& operator[](const std::size_t ind) const noexcept {
			if (ind >= size)
				throw std::out_of_range("Vec's index out of range");

			return get_unchecked(ind);
		}

		// --------------- Iterators ---------------

		/** @return iterator to the first element */
		[[nodiscard]] constexpr inline iterator begin() noexcept { return iterator(data); }

		/** @return constant iterator to the first element */
		[[nodiscard]] constexpr inline const_iterator begin() const noexcept { return const_iterator(data); }

		/**
		 * Gets first element if present.
		 * @throw std::out_of_range if vector is empty
		 * @return reference to the first element
		 */

		[[nodiscard]] constexpr inline T& front() noexcept override {
			if (size == 0) throw std::out_of_range("Vec is empty");
			return *begin();
		}

		/**
		 * Gets first element if present.
		 * @throw std::out_of_range if vector is empty
		 * @return constant reference to the first element
		 */

		[[nodiscard]] constexpr inline const T& front() const noexcept override {
			if (size == 0) throw std::out_of_range("Vec is empty");
			return *begin();
		}

		/** @return iterator to the out of bounds position of buffer */
		[[nodiscard]] constexpr inline iterator end() noexcept { return iterator(data + size); }

		/** @return constant iterator to the out of bounds position of buffer */
		[[nodiscard]] constexpr inline const_iterator end() const noexcept { return const_iterator(data + size); }

		/**
		 * Gets last element if present.
		 * @throw std::out_of_range if vector is empty
		 * @return reference to the last element
		 */

		[[nodiscard]] constexpr inline T& back() noexcept override {
			if (size == 0) throw std::out_of_range("Vec is empty");
			return *begin();
		}

		/**
		 * Gets last element if present.
		 * @throw std::out_of_range if vector is empty
		 * @return reference to the last element
		 */

		[[nodiscard]] constexpr inline const T& back() const noexcept override {
			if (size == 0) throw std::out_of_range("Vec is empty");
			return *begin();
		}

		/** @return number of elements in vector */
		[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return size; }
	};

	// ############################ Linked List ############################

	/**
	 * Double Linked List is an implementation of linked list with
	 * each node holding reference to both neighbours (previous and next node).
	 * Implements List ADL operations (adding, removing, front-back accessing, etc.).
	 * @param T type of element
	 *
	 * -------------- Complexity --------------
	 * |  Method   | Average case | Worst case |
	 * |push_back()|    O(1)      |   O(1)     |
	 * |insert(pos)|    O(n-pos)  |   O(n-pos) |
	 * |pop_back() |    O(1)      |   O(1)     |
	 * |erase(pos) |    O(n-pos)  |   O(n-pos) |
	 * | clear()   |    O(n)      |   O(n)     |
	 * | begin()   |    O(1)      |   O(1)     |
	 * |  end()    |    O(1)      |   O(1)     |
	 */

	template <typename T> class linked_list : public list<T> {

		// ############################ Linked List's Node ############################

		/**
		 * Node owns value (or null if not present) and contains
		 * reference to the previous node and
		 * reference to the next node.
		 *
		 * To resolve memory leak with cycling strong references,
		 * It was decided to make reference to next node weak (not owning).
		 *
		 * Node implements std::shared_from_this<node>, so it create
		 * std::shared_ptr<node> from itself to pass to another node.
		 */

		struct node : std::enable_shared_from_this<node> {
			std::shared_ptr<T> value = std::shared_ptr<T>(nullptr);
			std::shared_ptr<node> previous_node = std::shared_ptr<node>(nullptr);
			std::weak_ptr<node> next_node = std::weak_ptr<node>();

			/** Creates node with default parameters */
			node() noexcept = default;

			/**
			 * Creates node with given value
			 * @param value value to store in the node
			 */

			explicit node(const T& value) noexcept { this->value = std::make_shared<T>(value); }

			/**
			 * Creates node with given value
			 * @param value value to store in the node
			 */

			explicit node(T&& value) noexcept { this->value = std::make_shared<T>(value); }

			/**
			 * Creates node with given value and references to other nodes.
			 * @param value value to store in the node
			 * @param previous_node node that will be previous for this one
			 * @param next_node node that will be next for this one
			 */

			node(const T& value, const std::shared_ptr<node> previous_node, const std::weak_ptr<node> next_node) noexcept {
				this->value = std::make_shared(value);
				this->previous_node = previous_node;
				this->next_node = next_node;
			}

			/**
			 * Creates node with given value and references to other nodes.
			 * @param value value to store in the node
			 * @param previous_node node that will be previous for this one
			 * @param next_node node that will be next for this one
			 */

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

		// --------------- Iterators ---------------

		class const_iterator;

		// ############################ Iterator ############################

		/** Bidirectional iterator for linked list that allows to modify elements */

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

			/** Creates new iterator from node's reference */
			explicit iterator(std::shared_ptr<node> node_ptr) noexcept : node_ptr(std::move(node_ptr)) {}

		public:
			~iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			[[nodiscard]] inline reference operator*() const noexcept { return *node_ptr->value; }
			[[nodiscard]] inline pointer operator->() const noexcept { return node_ptr->value; }

			// --------------- Validation operators ---------------

			[[nodiscard]] constexpr inline bool has_value() const noexcept {
				return node_ptr.get() != nullptr && node_ptr->value.get() != nullptr;
			}

			/** @return true if node is not null and has value (not null) */
			[[nodiscard]] constexpr inline explicit operator bool() const noexcept { return has_value(); }

			/** @return true if either node is null or node's value is null */
			[[nodiscard]] constexpr inline bool operator!() const noexcept { return !has_value(); }

			// --------------- Comparison operators ---------------

			[[nodiscard]] constexpr inline bool operator==(const iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			[[nodiscard]] constexpr inline bool operator!=(const iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			[[nodiscard]] constexpr inline bool operator==(const const_iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			[[nodiscard]] constexpr inline bool operator!=(const const_iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			// --------------- Movement operators ---------------

			/** Moves to next node */
			inline iterator operator++() noexcept { node_ptr = node_ptr->next_node.lock(); return *this; }

			/** Moves to previous node */
			inline iterator operator--() noexcept { node_ptr = node_ptr->previous_node; return *this; }
		};

		// ############################ Constant Iterator ############################

		/** Bidirectional iterator for linked list that does not allow to modify elements */

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

			/** Creates new iterator from node's reference */
			explicit const_iterator(const std::shared_ptr<node> node_ptr) noexcept : node_ptr(node_ptr) {}

		public:
			~const_iterator() noexcept = default;

			// --------------- Dereference operators ---------------

			[[nodiscard]] inline reference operator*() const noexcept { return *node_ptr->value; }
			[[nodiscard]] inline pointer operator->() const noexcept { return node_ptr->value; }

			// --------------- Validation operators ---------------

			[[nodiscard]] constexpr inline bool has_value() const noexcept {
				return node_ptr.get() != nullptr && node_ptr->value.get() != nullptr;
			}

			/** @return true if node is not null and has value (not null) */
			[[nodiscard]] constexpr inline explicit operator bool() const noexcept { return has_value(); }

			/** @return true if either node is null or node's value is null */
			[[nodiscard]] constexpr inline bool operator!() const noexcept { return !has_value(); }

			// --------------- Comparison operators ---------------

			[[nodiscard]] constexpr inline bool operator==(const iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			[[nodiscard]] constexpr inline bool operator!=(const iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			[[nodiscard]] constexpr inline bool operator==(const const_iterator& other) const noexcept { return node_ptr.get() == other.node_ptr.get(); };
			[[nodiscard]] constexpr inline bool operator!=(const const_iterator& other) const noexcept { return node_ptr.get() != other.node_ptr.get(); };

			// --------------- Movement operators ---------------

			/** Moves to next node */
			inline iterator operator++() noexcept { node_ptr = node_ptr->next_node.lock(); return *this; }

			/** Moves to previous node */
			inline iterator operator--() noexcept { node_ptr = node_ptr->previous_node; return *this; }
		};

		// --------------- Constructors ---------------

		/** Creates new empty list */
		linked_list() noexcept = default;

		/**
		 * Creates new list with given elements.
		 * Complexity is O(n)
		 * @param elems to insert to the list
		 */

		linked_list(std::initializer_list<T> elems) noexcept {
			std::for_each(elems.begin(), elems.end(), [this](const T& elem) { push_back(elem); });
		}

		~linked_list() = default;

		// --------------- Inserting ---------------

		/**
		 * Creates new node and adds it to the end of the list.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

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

		/**
		 * Creates new node and adds it to the end of the list.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

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

		/**
		 * Creates new node and adds it to the start of the list.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

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

		/**
		 * Creates new node and adds it to the start of the list.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

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

		/**
		 * Creates new node and adds it
		 * to the given position of the list.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

		inline void insert(iterator position, const T& elem) noexcept {
			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			auto new_node = std::make_shared<node>(node(elem, prev_node, next_node));
			next_node.lock()->previous_node = new_node;
			prev_node->next_node = new_node;
		}

		/**
		 * Creates new node and adds it
		 * to the given position of the list.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

		inline void insert(const_iterator position, const T& elem) noexcept {
			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			auto new_node = std::make_shared<node>(node(elem, prev_node, next_node));
			next_node.lock()->previous_node = new_node;
			prev_node->next_node = new_node;
		}

		// --------------- Removing ---------------

		/**
		 * Removes element from the end of the list.
		 * Complexity is O(1).
		 * @return reference to the removed element
		 */

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

		/**
		 * Removes element from the start of the list.
		 * Complexity is O(1).
		 * @return reference to the removed element
		 */

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

		/**
		 * Removes element from the given position in the list.
		 * Complexity is O(1).
		 * @return reference to the removed element
		 */

		inline void erase(iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("list is empty");

			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			next_node.lock()->previous_node = prev_node;
			prev_node->next_node = next_node;
		}

		/**
		 * Removes element from the given position in the list.
		 * Complexity is O(1).
		 * @return reference to the removed element
		 */

		inline void erase(const_iterator position) noexcept {
			if (size == 0)
				throw std::out_of_range("list is empty");

			auto next_node = position.node_ptr->next_node;
			auto prev_node = position.node_ptr->previous_node;
			next_node.lock()->previous_node = prev_node;
			prev_node->next_node = next_node;
		}

		/**
		 * Removes all elements by
		 * removing all nodes in the list.
		 * Complexity is O(n)
		 */

		inline void clear() noexcept override {
			const auto sz = size;
			for (int i = 0; i < sz; ++i) pop_back();
		}

		// --------------- Iterators ---------------

		/** @return iterator to the beginning of the list */
		[[nodiscard]] constexpr inline iterator begin() noexcept { return iterator(front_node); }

		/** @return constant iterator to the beginning of the list */
		[[nodiscard]] constexpr inline const_iterator begin() const noexcept { return const_iterator(front_node); }

		/**
		 * Gets first element if present.
		 * @throw std::out_of_range if list is empty
		 * @return reference to the first element
		 */

		[[nodiscard]] constexpr inline T& front() noexcept override {
			if (size == 0) throw std::out_of_range("linked_list is empty");
			return *front_node->value;
		}

		/**
		 * Gets first element if present.
		 * @throw std::out_of_range if list is empty
		 * @return constant reference to the first element
		 */

		[[nodiscard]] constexpr inline const T& front() const noexcept override {
			if (size == 0) throw std::out_of_range("linked_list is empty");
			return *front_node->value;
		}

		/** @return iterator to the position out of the list */
		[[nodiscard]] constexpr inline iterator end() noexcept {
			return iterator(back_node.get() == nullptr ? back_node : back_node->next_node.lock());
		}

		/** @return const iterator to the position out of the list */
		[[nodiscard]] constexpr inline const_iterator end() const noexcept {
			return const_iterator(back_node.get() == nullptr ? back_node : back_node->next_node.lock());
		}

		/**
		 * Gets last element if present.
		 * @throw std::out_of_range if list is empty
		 * @return reference to the first element
		 */

		[[nodiscard]] constexpr inline T& back() noexcept override { return *back_node->value; }

		/**
		 * Gets first element if present.
		 * @throw std::out_of_range if list is empty
		 * @return constant reference to the first element
		 */

		[[nodiscard]] constexpr inline const T& back() const noexcept override { return *back_node->value; }

		/** @return number of elements in list */
		[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return size; }
	};

	// ############################ Stack ADL ############################

	/**
	 * Stack is an ADL that is capable of inserting elements
	 * to its end and removing elements from the end.
	 * Implements extendable, so it can add multiple values at the same time.
	 * See linked_stack implementation for more details
	 */

	template <typename T> struct stack : public extendable<T> {
		/**
		 * Gets last added element from list
		 * @throw std::out_of_range() if stack is empty
		 * @return reference to the last element
		 */

		[[nodiscard]] virtual T& top() noexcept = 0;

		/**
		 * Gets last added element from list
		 * @throw std::out_of_range() if stack is empty
		 * @return reference to the last element
		 */

		[[nodiscard]] virtual const T& top() const noexcept = 0;

		/** Adds element to stack */
		virtual void push(const T& elem) noexcept = 0;

		/** Adds element to stack */
		virtual void push(T&& elem) noexcept = 0;

		/** Adds element to stack with push() */
		inline void add(const T& elem) noexcept override { push(elem); }

		/** Adds element to stack with push() */
		inline void add(T&& elem) noexcept override { push(elem); }

		/**
		 * Removes element from the stack
		 * @throws std::out_of_range() if stack is empty
		 * @return reference to the removed element
		 */

		virtual T& pop() noexcept = 0;

		/** Removes all elements from the stack */
		virtual void clear() noexcept = 0;

		/** @return number of elements in stack */
		[[nodiscard]] virtual constexpr std::size_t get_size() const noexcept = 0;

		/**
		 * Is list empty?
		 * @return true if there are no elements in the list
		 */

		[[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }

		/**
		 * Is list empty?
		 * @return true if there at least one element in the list
		 */

		[[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return get_size() != 0; }
	};

	// ############################ Linked Stack ############################

	/**
	 * Linked Stack is an implementation of Stack ADL with basic stack operations
	 * (push(), pop(), top(), etc.). Implemented as wrapper over linked_list<T>.
	 * @param T type of element
	 *
	 * -------------- Complexity --------------
	 * |  Method   | Average case | Worst case |
	 * |  push()   |    O(1)      |   O(1)     |
	 * |  pop()    |    O(1)      |   O(1)     |
	 * |  top()    |    O(1)      |   O(1)     |
	 * | clear()   |    O(n)      |   O(n)     |
	 */

	template <typename T> class linked_stack : public stack<T> {
		linked_list<T> wrapper;

	public:

		// --------------- Constructors ---------------

		/** Creates empty stack */
		linked_stack() noexcept = default;

		/**
		 * Creates stack with given elems.
		 * Complexity is O(n)
		 */

		linked_stack(const std::initializer_list<T>& elems) noexcept {
			this->extend(elems.begin(), elems.end());
		}

		/**
		 * Creates stack with given elems.
		 * Complexity is O(n)
		 */

		linked_stack(std::initializer_list<T>&& elems) noexcept {
			this->extend(elems.begin(), elems.end());
		}

		~linked_stack() = default;

		// --------------- Inserting ---------------

		/**
		 * Adds element to the end of the stack.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

		inline void push(const T& elem) noexcept override { wrapper.push_back(elem); }

		/**
		 * Adds element to the end of the stack.
		 * Complexity is O(1).
		 * @param elem element to insert
		 */

		inline void push(T&& elem) noexcept override { wrapper.push_back(elem); }

		// --------------- Removing ---------------

		/**
		 * Removes element from the end of the stack.
		 * Complexity is O(1).
		 * @return reference to the removed element
		 */

		inline T& pop() noexcept override { return wrapper.pop_back(); }

		/**
		 * Removes all elements from the stack.
		 * Complexity is O(n)
		 */

		inline void clear() noexcept override { wrapper.clear(); }

		// --------------- Element Accessors ---------------

		/**
		 * Gets last element if present.
		 * @throw std::out_of_range if list is empty
		 * @return reference to the first element
		 */

		[[nodiscard]] constexpr inline T& top() noexcept override { return wrapper.back(); }

		/**
		 * Gets last element if present.
		 * @throw std::out_of_range if list is empty
		 * @return reference to the first element
		 */

		[[nodiscard]] constexpr inline const T& top() const noexcept override { return wrapper.back(); }

		/** @return number of elements in stack */
		[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return wrapper.get_size(); }
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

			/** @return number of inserted elements */
            [[nodiscard]] virtual constexpr std::size_t get_size() const noexcept = 0;

			/**
			 * Are there any elements?
			 * @return true if size is zero
			 */

            [[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }

			/**
			 * Are there any elements?
			 * @return true if size is greater zero
			 */

            [[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return !is_empty(); }
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
		 *
		 * -------------- Complexity --------------
		 * |  Method   | Average case | Worst case |
		 * |insert(k,v)|    O(hash)   | O(hash+n)  |
		 * | remove(k) |    O(hash)   | O(hash+n)  |
		 * |  get(k)   |    O(hash)   |  O(hash)   |
		 * | begin()   |  O(buckets)  | O(buckets) |
		 * |  end()    |    O(1)      |   O(1)     |
		 */

        template <typename K, typename V, typename H = std::hash<K>, typename KEq = std::equal_to<K>> class hash_map : public map<K, V> {
            constexpr static const float LOAD_FACTOR = 0.75F;

            H hasher = H();
            KEq key_equality_checker = KEq();

            std::size_t elems = 0;
            vec<linked_list<std::pair<K, V>>> buckets = vec<linked_list<std::pair<K, V>>>(16);

			// --------------- Bucket Iterators ---------------

            class const_bucket_iterator;

			// ############################ Bucket Iterator ############################

			/**
			 * Random access iterator to iterate over the buckets.
			 * Allows to modify bucket with its content
			 */

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

                [[nodiscard]] inline reference operator*() const noexcept { return *(begin + index); }
				[[nodiscard]] inline pointer operator->() const noexcept { return &*(begin + index); }

				// --------------- Comparison operators ---------------

				[[nodiscard]] constexpr inline bool operator==(const bucket_iterator& other) const noexcept { return index == other.index; };
				[[nodiscard]] constexpr inline bool operator!=(const bucket_iterator& other) const noexcept { return index != other.index; };

				[[nodiscard]] constexpr inline bool operator==(const const_bucket_iterator& other) const noexcept { return index == other.index; };
				[[nodiscard]] constexpr inline bool operator!=(const const_bucket_iterator& other) const noexcept { return index != other.index; };

				// --------------- Movement operators ---------------

                inline bucket_iterator operator++() noexcept { ++index; return *this; }
                inline bucket_iterator operator--() noexcept { --index; return *this; }

				[[nodiscard]] constexpr inline bucket_iterator operator+(const std::size_t move) const noexcept { return bucket_iterator(index + move, begin); }
				[[nodiscard]] constexpr inline bucket_iterator operator-(const std::size_t move) const noexcept { return bucket_iterator(index - move, begin); }
            };

			/** Gets iterator to the first bucket */

            [[nodiscard]] constexpr inline bucket_iterator get_bucket_begin_it() const noexcept {
                return bucket_iterator(0, buckets.begin());
            }

			/** Gets iterator to out of range buckets */

            [[nodiscard]] constexpr inline bucket_iterator get_bucket_end_it() const noexcept {
                return bucket_iterator(buckets.size(), buckets.begin());
            }

			// ############################ Const Bucket Iterator ############################

			/**
			 * Random access iterator to iterate over the buckets.
			 * Does not allow to modify bucket or its content
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

				[[nodiscard]] inline reference operator*() const noexcept { return *(begin + index); }
				[[nodiscard]] inline pointer operator->() const noexcept { return &*(begin + index); }

				// --------------- Comparison operators ---------------

				[[nodiscard]] constexpr inline bool operator==(const bucket_iterator& other) const noexcept { return index == other.index; };
				[[nodiscard]] constexpr inline bool operator!=(const bucket_iterator& other) const noexcept { return index != other.index; };

				[[nodiscard]] constexpr inline bool operator==(const const_bucket_iterator& other) const noexcept { return index == other.index; };
				[[nodiscard]] constexpr inline bool operator!=(const const_bucket_iterator& other) const noexcept { return index != other.index; };

				// --------------- Movement operators ---------------

                inline const_bucket_iterator operator++() noexcept { ++index; return *this; }
                inline const_bucket_iterator operator--() noexcept { --index; return *this; }

				[[nodiscard]] constexpr inline const_bucket_iterator operator+(const std::size_t move) const noexcept { return const_bucket_iterator(index + move, begin); }
				[[nodiscard]] constexpr inline const_bucket_iterator operator-(const std::size_t move) const noexcept { return const_bucket_iterator(index - move, begin); }
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
                    buckets.reserve(buckets.get_size() << 1);
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

			// --------------- Iterators ---------------

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

                [[nodiscard]] inline reference operator*() const noexcept { return *it; }
				[[nodiscard]] inline pointer operator->() const noexcept { return &*it; }

				// --------------- Validation operators ---------------

				[[nodiscard]] constexpr inline explicit operator bool() const noexcept { return it; }
				[[nodiscard]] constexpr inline bool operator!() const noexcept { return !it; }

				// --------------- Comparison operators ---------------

				[[nodiscard]] constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
				[[nodiscard]] constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };

				[[nodiscard]] constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
				[[nodiscard]] constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };

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
					bucket.push_back(std::make_pair(key, value));
					resize_if_load_factor_reached();
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

		// ############################ Set ADL ############################

		/**
		 * Set ADL with basic insertion and remove strategy
		 * @param T value type
		 */

		template <typename T> struct set {
			virtual void insert(const T& value) noexcept = 0;
			virtual void insert(T&& value) noexcept = 0;

			virtual bool remove(const T& value) noexcept = 0;

			[[nodiscard]] virtual constexpr std::size_t get_size() const noexcept = 0;
			[[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }
			[[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return get_size() != 0; }
		};

		// ############################ Hash Set ############################

		/**
		 * Hash set implementation with side-chaining (buckets).
		 * Actual implementation is a wrapper over hash_map<T, none>.
		 * By default, the number of buckets is 16; Load factor is 75%.
		 * By default, std::hash<K> is used to hash keys. To provide
		 * your personal hasher, you have to override std::size_t operator()
		 * with your hashing strategy. You can use Boost's hash_combine() method
		 * for your data structure.
		 *
		 * @param T value type
		 * @param H hasher class with overridden std::size_t operator(). By default std::hash<K> is used
		 * @param KEq key's equality checker. By default std::equal_to<K> is used
		 *
		 * -------------- Complexity --------------
		 * |  Method   | Average case | Worst case |
		 * | insert(v) |    O(hash)   | O(hash+n)  |
		 * | remove(v) |    O(hash)   | O(hash+n)  |
		 * | begin()   |  O(buckets)  | O(buckets) |
		 * |  end()    |    O(1)      |   O(1)     |
		 */

		template <typename T, typename H = std::hash<T>, typename KEq = std::equal_to<T>> class hash_set :
				public set<T>, public extendable<T> {
			struct none {};

			using map_wrapper = hash_map<T, none, H, KEq>;
			map_wrapper wrapper;

		public:

			// --------------- Iterators ---------------

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

				[[nodiscard]] inline reference operator*() const noexcept { return *it; }
				[[nodiscard]] inline pointer operator->() const noexcept { return &*it; }

				// --------------- Validation operators ---------------

				[[nodiscard]] constexpr inline explicit operator bool() const noexcept { return it; }
				[[nodiscard]] constexpr inline bool operator!() const noexcept { return !it; }

				// --------------- Comparison operators ---------------

				[[nodiscard]] constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
				[[nodiscard]] constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };

				[[nodiscard]] constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
				[[nodiscard]] constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };

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

			// ############################ Const Iterator ############################

			/**
			 * Bidirectional iterator to iterate over map's entries.
			 * Does not allow to modify its value
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
				wrapper.insert(value, none());
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
