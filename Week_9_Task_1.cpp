// Arseny Savchenko

#include <iostream>
#include <vector>
#include <memory>

#pragma GCC optimize("Ofast")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("avx,avx2,fma")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,tune=native")

// ###################### ARTICLE ABOUT SEGMENT INTERSECTION ######################
// The problem of determining the intersection of segments occurs in many applications
// with a graphical interface. One of such applications is the classic game "snake",
// in which the user, playing as a snake, eats fruits and increases in size.
// If the snake crashes into the wall, the game is over. Thus, in this problem
// it is necessary to find the intersection points of the segments.
//
// Moreover, there is currently a whole class of similar games where characters
// take a variety of forms, and similar algorithms are used to find intersections.

namespace dsa_hw {

	/**
	 * Generic implementation of out-of-place
	 * stable merge sorting algorithm.
	 * Complexity is O(n * log(n))
	 *
	 * @param begin iterator to the beginning of the collection
	 * @param end iterator to the end of the collection
	 * @param cmp comparator function. As default, std::less<T> is used.
	 */

	template <typename T, typename RandomAccessIterator, typename Cmp = std::less<T>> void merge_sort(
			RandomAccessIterator begin,
			RandomAccessIterator end,
			const Cmp cmp
	) noexcept {
		if (begin == end || begin + 1 == end)
			return;

		const auto mid = begin + (end - begin) / 2;

		merge_sort<T>(begin, mid, cmp);
		merge_sort<T>(mid, end, cmp);

		int i = 0, q = 0;

		auto* const sort_holder = new T[end - begin];

		while (begin + i != mid && mid + q != end) {
			if (cmp(*(begin + i), *(mid + q))) {
				sort_holder[i + q] = *(begin + i);
				++i;
			} else {
				sort_holder[i + q] = *(mid + q);
				++q;
			}
		}

		while (begin + i != mid) {
			sort_holder[i + q] = *(begin + i);
			++i;
		}

		while (mid + q != end) {
			sort_holder[i + q] = *(mid + q);
			++q;
		}

		for (int step = 0; step < i + q; ++step)
			*(begin + step) = sort_holder[step];

		delete[] sort_holder;
	}

	// ############################### AVL Tree Implementation ###############################

	namespace avl {

		// ############################### AVL Tree ###############################

		/**
		 * AVL tree implementation that supports only unique values.
		 *
		 * The AVL tree is a self-balancing binary search tree,
		 * proposed by George Adelson-Velsky and Eugene Landis in 1962.
		 * The main condition of the AVL tree is that for every vertex,
		 * the difference in height of subtrees less or equal to 1.
		 * In other case, the tree is rebalanced with double rotations.
		 * Provides logarithmic complexity of searching, inserting and deleting elements in the tree.
		 *
		 * @param T type of element
		 * @param Cmp comparator to sort elements in the tree. By default, std::less<T> is used.
		 */

		template <typename T, typename Cmp = std::less<T>> class avl_tree {
			const Cmp cmp = Cmp();

			/**
			 * Check if keys are equal with comparator.
			 * Complexity depends on comparator, in default case its O(1).
			 *
			 * @param a first key
			 * @param b second key
			 * @return true if !cmp(a, b) && !cmp(b, a)
			 */

			[[nodiscard]] inline bool keys_equal(const T& a, const T& b) const noexcept {
				return !cmp(a, b) && !cmp(b, a);
			}

			// ############################### Node ###############################

			/**
			 * Node of AVL tree provides access
			 * to the element and children subtrees.
			 * Wraps value and children with shared_ptr.
			 * Calculates height on every recomposition
			 * (max(left.height, right.height) + 1)
			 */

			struct node {
				std::shared_ptr<T> value = nullptr;
				std::size_t height = 1;

				std::shared_ptr<node> left = nullptr;
				std::shared_ptr<node> right = nullptr;

				/**
				 * Complete constructor for node
				 * @param value pointer to value (default is nullptr)
				 * @param height height of the tree
				 * (maximum from children subtrees + 1, default is 1)
				 * @param left pointer to left child (default is nullptr)
				 * @param right pointer to right child (default is nullptr)
				 */

				explicit node(
						const std::shared_ptr<T>& value = nullptr,
						const std::size_t height = 1,
						const std::shared_ptr<node>& left = nullptr,
						const std::shared_ptr<node>& right = nullptr
				) noexcept : value(value), height(height), left(left), right(right) {}

				/**
				 * Complete constructor for node
				 * @param value pointer to value (default is nullptr)
				 * @param height height of the tree
				 * (maximum from children subtrees + 1, default is 1)
				 * @param left pointer to left child (default is nullptr)
				 * @param right pointer to right child (default is nullptr)
				 */

				explicit node(
						std::shared_ptr<T>&& value = nullptr,
						const std::size_t height = 1,
						std::shared_ptr<node>&& left = nullptr,
						std::shared_ptr<node>&& right = nullptr
				) noexcept : value(value), height(height), left(left), right(right) {}

				/**
				 * Calculates maximum height from child subtrees.
				 * Complexity is O(1)
				 * @return maximum from subtree's height nodes + 1
				 */

				[[nodiscard]] inline std::size_t calc_height_from_children() const noexcept {
					const bool is_left = left != nullptr;
					const bool is_right = right != nullptr;

					if (is_left && is_right)
						return std::max(left->height, right->height) + 1;

					if (is_left)
						return left->height + 1;

					if (is_right)
						return right->height + 1;

					return 0;
				}

				/**
				 * Calculates balance factor from subtrees.
				 * Complexity is O(1)
				 *
				 * @return
				 * 1) if both nodes exists -> left->height - right->height
				 * 2) if left exists -> left-height
				 * 3) if right exists -> -right->height
				 * 4) no subtrees exists -> 0
				 */

				[[nodiscard]] inline std::size_t balance_factor() const noexcept {
					const bool is_left = left != nullptr;
					const bool is_right = right != nullptr;

					if (is_left && is_right)
						return left->height - right->height;

					if (is_left)
						return left->height;

					if (is_right)
						return -right->height;

					return 0;
				}

				// ------------------------ Rotations ------------------------

				/**
				 * Double rotation to left.
				 * Complexity is O(1).
				 * @return local root node after rotation
				 */

				[[nodiscard]] static inline std::shared_ptr<node> left_left_rotation(
						std::shared_ptr<node> node
				) noexcept {
					const auto& node1 = node;
					auto node2 = node1->left;

					node1->left = node2->right;
					node2->right = node1;

					return node2;
				}

				/**
				 * Double rotation to right.
				 * Complexity is O(1).
				 * @return local root node after rotation
				 */

				[[nodiscard]] static inline std::shared_ptr<node> right_right_rotation(
						std::shared_ptr<node> node
				) noexcept {
					const auto& node1 = node;
					auto node2 = node1->right;

					node1->right = node2->left;
					node2->left = node1;

					return node2;
				}

				/**
				 * Rotation to right then left.
				 * Complexity is O(1).
				 * @return local root node after rotation
				 */

				[[nodiscard]] static inline std::shared_ptr<node> right_left_rotation(
						std::shared_ptr<node> node
				) noexcept {
					const auto& node1 = node;
					auto node2 = node1->right;
					auto node3 = node->right->left;

					node1->right = node3->left;
					node2->left = node3->right;
					node3->left = node1;
					node3->right = node2;

					return node3;
				}

				/**
				 * Rotation to left then right.
				 * Complexity is O(1).
				 * @return local root node after rotation
				 */

				[[nodiscard]] static inline std::shared_ptr<node> left_right_rotation(
						std::shared_ptr<node> node
				) noexcept {
					const auto& node1 = node;
					auto node2 = node1->left;
					auto node3 = node->left->right;

					node1->left = node3->right;
					node2->right = node3->left;
					node3->right = node1;
					node3->left = node2;

					return node3;
				}

				// ------------------------ Min/Max nodes ------------------------

				/**
				 * Gets child node with maximum value.
				 * Goes strict to the right without any recursion.
				 * @param node node to scan in
				 * @return rightest node with biggest value
				 */

				[[nodiscard]] static std::shared_ptr<node> max(std::shared_ptr<node> node) noexcept {
					if (node == nullptr)
						return nullptr;

					auto nd = node;

					while (nd->right != nullptr)
						nd = nd->right;

					return nd;
				}

				/**
				 * Gets child node with minimum value.
				 * Goes strict to the left without any recursion.
				 * @param node node to scan in
				 * @return leftest node with smallest value
				 */

				[[nodiscard]] static std::shared_ptr<node> min(std::shared_ptr<node> node) noexcept {
					if (node == nullptr)
						return nullptr;

					auto nd = node;

					while (nd->left != nullptr)
						nd = nd->left;

					return nd;
				}
			};

		public:

			// ############################### Iterator ###############################

			/**
			 * Iterator to the node in tree.
			 * Provides immutable access to the element
			 */

			struct iterator {
			public:
				using iterator_category = std::bidirectional_iterator_tag;
				using difference_type = std::ptrdiff_t;
				using value_type = T;
				using pointer = const value_type*;
				using reference = const value_type&;

			private:
				friend class avl_tree<T, Cmp>;
				std::shared_ptr<node> ptr;
				avl_tree<T, Cmp>* tree;

				/** Initializes iterator with pointers to the element and tree */

				explicit iterator(
						std::shared_ptr<node> ptr,
						avl_tree<T, Cmp>* tree
				) noexcept : ptr(std::move(ptr)), tree(tree) {}

			public:
				~iterator() noexcept = default;

				// --------------- Dereference operators ---------------

				inline reference operator*() const noexcept { return *ptr->value; }
				inline pointer operator->() const noexcept { return ptr->value.get(); }

				// --------------- Validation operators ---------------

				[[nodiscard]] constexpr inline explicit operator bool() const noexcept { return ptr != nullptr; }
				[[nodiscard]] constexpr inline bool operator!() const noexcept { return ptr == nullptr; }

				// --------------- Comparison operators ---------------

				[[nodiscard]] constexpr inline bool operator==(const iterator& other) const noexcept { return ptr == other.ptr; };
				[[nodiscard]] constexpr inline bool operator!=(const iterator& other) const noexcept { return ptr != other.ptr; };

				// --------------- Movement operators ---------------

				/**
				 * Moves to the next value. Complexity is logarithmic
				 * @return updated iterator to the next node
				 */

				inline iterator operator++() noexcept { ptr = tree->next(tree->root, ptr); return *this; }

				/**
				 * Moves to the next value. Complexity is logarithmic
				 * @return updated iterator to the next node
				 */

				inline iterator operator--() noexcept { ptr = tree->prev(tree->root, ptr); return *this; }
			};

		private:
			std::shared_ptr<node> root = nullptr;
			std::size_t size = 0;

		public:
			/** Creates empty AVL tree */
			avl_tree() noexcept = default;

			~avl_tree() noexcept = default;

			/**
			 * Gets number of elements in the tree.
			 * Complexity is O(1)
			 * @return number of elements
			 */

			[[nodiscard]] constexpr inline std::size_t get_size() const noexcept {
				return size;
			}

			/**
			 * Checks if there are some elements in the tree.
			 * Complexity is O(1)
			 * @return true if tree is empty
			 */

			[[nodiscard]] constexpr inline bool empty() const noexcept {
				return size == 0;
			}

		private:

			// --------------- Insertion ---------------

			/**
			 * Inserts element to the node or its sub-nodes.
			 * If value is already present in the node, does nothing
			 * and returns nullptr.
			 * Provides logarithmic complexity with node's height
			 *
			 * @param to_node node in which value should be inserted
			 * @param value value to insert
			 * @return pair of new root after insertion and node with inserted value
			 */

			std::pair<std::shared_ptr<node>, std::shared_ptr<node>> insert(
					std::shared_ptr<node>& to_node,
					const T& value
			) noexcept {
				// if node does not exists, create node
				if (to_node == nullptr) {
					to_node = std::make_shared<node>(node(std::make_shared<T>(value)));
					return std::make_pair(to_node, to_node);
				}

				// Checking if value was added
				std::shared_ptr<node> res = nullptr;

				if (cmp(value, *to_node->value)) {
					// value < node -> insert to left
					auto [left_val, result] = insert(to_node->left, value);

					if (result == nullptr)
						return std::make_pair(to_node, nullptr);

					to_node->left = left_val;
					res = result;
				} else if (cmp(*to_node->value, value)) {
					// value > node -> insert to right
					auto [right_val, result] = insert(to_node->right, value);

					if (result == nullptr)
						return std::make_pair(to_node, nullptr);

					to_node->right = right_val;
					res = result;
				} else {
					// value == node -> already exists, do nothing
					return std::make_pair(to_node, nullptr);
				}

				// Calculating height to rebalance
				to_node->height = to_node->calc_height_from_children();

				const auto bf = to_node->balance_factor();
				const auto is_left = to_node->left != nullptr;
				const auto is_right = to_node->right != nullptr;

				// Rebalancing tree according to the balance factor

				if (bf == 2 && is_left && to_node->left->balance_factor() == 1)
					to_node = node::left_left_rotation(to_node);

				else if (bf == -2 && is_right && to_node->right->balance_factor() == -1)
					to_node = node::right_right_rotation(to_node);

				else if (bf == -2 && is_right && to_node->right->balance_factor() == 1)
					to_node = node::right_left_rotation(to_node);

				else if (bf == 2 && is_left && to_node->left->balance_factor() == -1)
					to_node = node::left_right_rotation(to_node);

				// Returning new local root and inserted node
				return std::make_pair(to_node, res);
			}

			/**
			 * Inserts element to the node or its sub-nodes.
			 * If value is already present in the node, does nothing
			 * and returns nullptr.
			 * Provides logarithmic complexity with node's height
			 *
			 * @param to_node node in which value should be inserted
			 * @param value value to insert
			 * @return pair of new root after insertion and node with inserted value
			 */

			std::pair<std::shared_ptr<node>, std::shared_ptr<node>> insert(
					std::shared_ptr<node>& to_node,
					T&& value
			) noexcept {
				// if node does not exists, create node
				if (to_node == nullptr) {
					to_node = std::make_shared<node>(node(std::make_shared<T>(value)));
					return std::make_pair(to_node, to_node);
				}

				// Checking if value was added
				std::shared_ptr<node> res = nullptr;

				if (cmp(value, *to_node->value)) {
					// value < node -> insert to left
					auto [left_val, result] = insert(to_node->left, value);

					if (result == nullptr)
						return std::make_pair(to_node, nullptr);

					to_node->left = left_val;
					res = result;
				} else if (cmp(*to_node->value, value)) {
					// value > node -> insert to right
					auto [right_val, result] = insert(to_node->right, value);

					if (result == nullptr)
						return std::make_pair(to_node, nullptr);

					to_node->right = right_val;
					res = result;
				} else {
					// value == node -> already exists, do nothing
					return std::make_pair(to_node, nullptr);
				}

				// Calculating height to rebalance
				to_node->height = to_node->calc_height_from_children();

				const auto bf = to_node->balance_factor();
				const auto is_left = to_node->left != nullptr;
				const auto is_right = to_node->right != nullptr;

				// Rebalancing tree according to the balance factor

				if (bf == 2 && is_left && to_node->left->balance_factor() == 1)
					to_node = node::left_left_rotation(to_node);

				else if (bf == -2 && is_right && to_node->right->balance_factor() == -1)
					to_node = node::right_right_rotation(to_node);

				else if (bf == -2 && is_right && to_node->right->balance_factor() == 1)
					to_node = node::right_left_rotation(to_node);

				else if (bf == 2 && is_left && to_node->left->balance_factor() == -1)
					to_node = node::left_right_rotation(to_node);

				// Returning new local root and inserted node
				return std::make_pair(to_node, res);
			}

		public:
			/**
			 * Inserts element to the tree.
			 * If value is already present in the node, does nothing
			 * and returns end iterator.
			 * Provides logarithmic complexity with tree's height
			 *
			 * @param value value to insert
			 * @return iterator to inserted element or end iterator
			 */

			inline iterator insert(const T& value) noexcept {
				auto res = insert(root, value).second;
				if (res) ++size;
				return iterator(res, this);
			}

			/**
			 * Inserts element to the tree.
			 * If value is already present in the node, does nothing
			 * and returns end iterator.
			 * Provides logarithmic complexity with tree's height
			 *
			 * @param value value to insert
			 * @return iterator to inserted element or end iterator
			 */

			inline iterator insert(T&& value) noexcept {
				auto res = insert(root, value).second;
				if (res) ++size;
				return iterator(res, this);
			}

		private:

			// --------------- Deletion ---------------

			/**
			 * Deletes element from the node or its sub-nodes.
			 * Does not properly handle situation if value is not present.
			 * Provides logarithmic complexity with node's height
			 *
			 * @param to_node node in which value should be inserted
			 * @param value value to insert
			 * @return new local root
			 */

			std::shared_ptr<node> erase(
					std::shared_ptr<node> to_node,
					const T& value
			) noexcept {
				// If node does not exists
				if (to_node == nullptr)
					return nullptr;

				const bool is_left = to_node->left != nullptr;
				const bool is_right = to_node->right != nullptr;

				// If it is a leaf
				if (!is_left && !is_right) {
					if (to_node == root)
						root = nullptr;
					return nullptr;
				}

				if (cmp(*to_node->value, value)) {
					// value > node -> remove from right
					to_node->right = erase(to_node->right, value);
				} else if (cmp(value, *to_node->value)) {
					// value < node -> remove from left
					to_node->left = erase(to_node->left, value);
				} else {
					// values = node, replace with child node
					if (is_left) {
						// replace with left max child (previous node in ascending order)
						auto prev = node::max(to_node->left);
						to_node->value = prev->value;
						to_node->left = erase(to_node->left, *prev->value);
					} else {
						// replace with right max child (next node in ascending order)
						auto next = node::min(to_node->right);
						to_node->value = next->value;
						to_node->right = erase(to_node->right, *next->value);
					}
				}

				// Rebalancing tree according to the balance factor

				if (to_node->balance_factor() == 2 && is_left && to_node->left->balance_factor() == 1)
					to_node = node::left_left_rotation(to_node);

				else if (to_node->balance_factor() == 2 && is_left && to_node->left->balance_factor() == -1)
					to_node = node::left_right_rotation(to_node);

				else if (to_node->balance_factor() == 2 && is_left && to_node->left->balance_factor() == 0)
					to_node = node::left_left_rotation(to_node);

				else if (to_node->balance_factor() == -2 && is_right && to_node->right->balance_factor() == -1)
					to_node = node::right_right_rotation(to_node);

				else if (to_node->balance_factor() == -2 && is_right && to_node->right->balance_factor() == 1)
					to_node = node::right_left_rotation(to_node);

				else if (to_node->balance_factor() == -2 && is_right && to_node->right->balance_factor() == 0)
					to_node = node::right_right_rotation(to_node);

				// Return new local root
				return to_node;
			}

		public:
			/**
			 * Deletes element from the tree by iterator to that node.
			 * Provides logarithmic complexity with NODE's height
			 *
			 * @param to_node node in which value should be inserted
			 * @param value value to insert
			 * @return new local root
			 */

			inline void erase(const iterator node) noexcept {
					root = erase(root, *node.ptr->value);
					--size;
				}

			/**
			 * Deletes element from the tree by its value.
			 * Provides logarithmic complexity with TREE's height
			 *
			 * @param to_node node in which value should be inserted
			 * @param value value to insert
			 * @return new local root
			 */

			inline void erase(const T& value) noexcept {
				auto it = find(value);
				if (it != nullptr) erase(it);
			}

		private:

			// --------------- Search ---------------

			/**
			 * Searches for a node with given value in the node.
			 * Provides logarithmic complexity with node's height
			 * @param to_node node in which searching
			 * @param value value to search
			 * @return pointer to node or nullptr
			 */

			[[nodiscard]] inline std::shared_ptr<node> find(
					const std::shared_ptr<node>& to_node,
					const T& value
			) const noexcept {
				if (to_node == nullptr)
					return nullptr;

				const bool is_left = to_node->left != nullptr;
				const bool is_right = to_node->right != nullptr;

				if (!is_left && !is_right) {
					if (keys_equal(value, *to_node->value))
						return to_node;
					return nullptr;
				}

				if (cmp(*to_node->value, value)) {
					return find(to_node->right, value);
				} else if (cmp(value, *to_node->value)) {
					return find(to_node->left, value);
				} else {
					if (keys_equal(value, *to_node->value))
						return to_node;
					return nullptr;
				}
			}

		public:

			/**
			 * Searches for a node with given value.
			 * Provides logarithmic complexity with tree's height
			 * @param value value to search
			 * @return iterator to node or end iterator
			 */

			[[nodiscard]] inline iterator find(const T& value) noexcept {
				return iterator(find(root, value), this);
			}

		private:
			/**
			 * Searches for a node with value >= than given key.
			 * Provides logarithmic complexity with node's height
			 * @param to_node node in which searching
			 * @param value value to search
			 * @return pointer to node or nullptr
			 */

			[[nodiscard]] std::shared_ptr<node> greater_or_equal(
					const std::shared_ptr<node>& node,
					const T& key
			) const noexcept {
				if (node == nullptr)
					return nullptr;

				if (keys_equal(*node->value, key))
					return node;

				// node < key, not acceptable, go to right
				if (cmp(*node->value, key))
					return node->right ? greater_or_equal(node->right, key) : nullptr;

				// node > key, acceptable, but check left
				if (!node->left)
					return node;

				auto left_res = greater_or_equal(node->left, key);
				return left_res ? left_res : node;
			}

			/**
			 * Searches for a node with value <= than given key.
			 * Provides logarithmic complexity with node's height
			 * @param to_node node in which searching
			 * @param value value to search
			 * @return pointer to node or nullptr
			 */

			[[nodiscard]] std::shared_ptr<node> less_or_equal(
					const std::shared_ptr<node>& node,
					const T& key
			) const noexcept {
				if (node == nullptr)
					return nullptr;

				if (keys_equal(*node->value, key))
					return node;

				// node < key, acceptable, but check right
				if (cmp(*node->value, key)) {
					if (!node->right)
						return node;

					auto right_res = less_or_equal(node->right, key);
					return right_res ? right_res : node;
				}

				// node > key, not acceptable, go to left
				return node->left ? less_or_equal(node->left, key) : nullptr;
			}

		public:

			/**
			 * Searches for a node with value >= than given key.
			 * Provides logarithmic complexity with tree's height
			 * @param to_node node in which searching
			 * @param value value to search
			 * @return iterator to such element or end iterator
			 */

			[[nodiscard]] iterator greater_or_equal(const T& key) noexcept {
				return iterator(greater_or_equal(root, key), this);
			}

			/**
			 * Searches for a node with value <= than given key.
			 * Provides logarithmic complexity with tree's height
			 * @param to_node node in which searching
			 * @param value value to search
			 * @return iterator to such element or end iterator
			 */

			[[nodiscard]] iterator less_or_equal(const T& key) noexcept {
				return iterator(less_or_equal(root, key), this);
			}

		private:

			/**
			 * Searches for a node with previous value in ascending order.
			 * Provides logarithmic complexity with node's height
			 * @param cur_node node in which we are iterating
			 * @param search_node node for which we are looking to find previous
			 * @return pointer to node or nullptr
			 */

			[[nodiscard]] std::shared_ptr<node> prev(
					const std::shared_ptr<node>& cur_node,
					const std::shared_ptr<node>& search_node
			) const noexcept {
				if (cur_node == nullptr)
					return nullptr;

				const auto& key = *search_node->value;

				if (keys_equal(*cur_node->value, key))
					return node::max(cur_node->left);

				// cur_node < key, acceptable, but check right
				if (cmp(*cur_node->value, key)) {
					if (!cur_node->right)
						return cur_node;

					auto right_res = prev(cur_node->right, search_node);
					return right_res ? right_res : cur_node;
				}

				// cur_node > key, not acceptable, go to left
				return cur_node->left ? prev(cur_node->left, search_node) : nullptr;
			}

			/**
			 * Searches for a node with next value in ascending order.
			 * Provides logarithmic complexity with node's height
			 * @param cur_node node in which we are iterating
			 * @param search_node node for which we are looking to find next
			 * @return pointer to node or nullptr
			 */

			[[nodiscard]] std::shared_ptr<node> next(
					const std::shared_ptr<node>& cur_node,
					const std::shared_ptr<node>& search_node
			) const noexcept {
				if (cur_node == nullptr)
					return nullptr;

				const auto& key = *search_node->value;

				if (keys_equal(*cur_node->value, key))
					return node::min(cur_node->right);

				// node < key, not acceptable, go to right
				if (cmp(*cur_node->value, key))
					return cur_node->right ? next(cur_node->right, search_node) : nullptr;

				// node > key, acceptable, but check left
				if (!cur_node->left)
					return cur_node;

				auto left_res = next(cur_node->left, search_node);
				return left_res ? left_res : cur_node;
			}

		public:

			/**
			 * Searches for an iterator with previous value in ascending order.
			 * Provides logarithmic complexity with tree's height
			 * @param it iterator for which we are looking to find previous
			 * @return iterator to previous value or end iterator
			 */

			[[nodiscard]] inline iterator prev(const iterator& it) noexcept {
				return iterator(prev(root, it.ptr), this);
			}

			/**
			 * Searches for an iterator with next value in ascending order.
			 * Provides logarithmic complexity with tree's height
			 * @param it iterator for which we are looking to find next
			 * @return iterator to next value or end iterator
			 */

			[[nodiscard]] inline iterator next(const iterator& node) noexcept {
				return iterator(next(root, node.ptr), this);
			}

			/**
			 * Gets iterator with smallest value (or just leftest node).
			 * Provides logarithmic complexity with tree's height
			 * @return iterator with smallest value
			 */

			[[nodiscard]] constexpr inline iterator begin() noexcept {
				return iterator(node::min(root), this);
			}

			/**
			 * Gets iterator with smallest value (or just leftest node).
			 * Provides logarithmic complexity with tree's height
			 * @return iterator with smallest value
			 */

			[[nodiscard]] constexpr inline iterator end() noexcept {
				return iterator(nullptr, nullptr);
			}
		};
	}

	// ######################### Line Sweep algorithm #########################
	// The main purpose of this algorithm is to find intersecting segments
	// with O(n * log(n)) complexity. Brief description:
	//
	// 1) Sorting points by X coordinates (then by Y)
	// 2) Traversing through points and searching for intersection
	// ------------------------------------------------------------------------------
	// 3) If point is the beginning of the segment, inserting into the tree
	// 4) Check for intersection with previous and next segments from the tree
	// ------------------------------------------------------------------------------
	// 5) If point is the ending, checking intersection of previous and next segments
	// 6) Removing point
	// ------------------------------------------------------------------------------
	// 7) If no intersections were found, then there is no intersecting segments

	struct segment;

	// ------------------------ Point ------------------------

	/**
	 * Point structure with X and Y coordinates.
	 * Since points are used in segments,
	 * structure provides information about point's
	 * location (position in input and position according to X axis)
	 */

	struct point {
		/** Position in input (or chronological order) */
		enum input_position { FIRST, SECOND };

		/** Position according to X axis in ascending order */
		enum segment_position { BEGIN, END };

		long long x = 0, y = 0;

		/** Position according to X axis in ascending order */
		segment_position seg_pos = BEGIN;

		/** Position in input (or chronological order) */
		input_position input_pos = FIRST;

		/**
		 * Constructs point from given coordinates and input position
		 * @param x coordinate on X axis
		 * @param y coordinate on Y axis
		 * @param input_pos Position in input (or chronological order).
		 * If not necessary, by default its FIRST
		 */

		point(
				const long long x,
				const long long y,
				enum input_position input_pos = FIRST
		) noexcept :x(x), y(y), input_pos(input_pos) {}

		/**
		 * Constructs default point with zero coordinates,
		 * FIRST as input position and BEGIN as segment position
		 */

		point() noexcept = default;
		~point() noexcept = default;

		/**
		 * Parses point from input stream
		 * @param input_pos position in input
		 * @return parsed point from the
		 */

		[[nodiscard]] static inline point from_input(
				enum input_position input_pos,
				std::istream& in = std::cin
		) noexcept {
			long long x = 0, y = 0;
			in >> x >> y;
			return { x, y, input_pos };
		}

		/**
		 * Check if point is the beginning of the segment
		 * @return true if seg_pos == BEGIN
		 */

		[[nodiscard]] constexpr inline bool is_begin() const noexcept {
			return seg_pos == BEGIN;
		}

		/**
		 * Check if point is the ending of the segment
		 * @return true if seg_pos == END
		 */

		[[nodiscard]] constexpr inline bool is_end() const noexcept {
			return seg_pos == END;
		}

		/**
		 * Check if point was read firstly from the input
		 * @return true if input_pos == FIRST
		 */

		[[nodiscard]] constexpr inline bool is_first() const noexcept {
			return input_pos == FIRST;
		}

		/**
		 * Check if point was read second from the input
		 * @return true if input_pos == SECOND
		 */

		[[nodiscard]] constexpr inline bool is_second() const noexcept {
			return input_pos == SECOND;
		}

		/**
		 * Check if point satisfies y = kx + b line
		 * @return true if Py = k * Px + b
		 */

		[[nodiscard]] constexpr inline bool is_belong_to_line(
				const long long k,
				const long long b
		) const noexcept { return y == k * x + b; }
	};

	// ------------------------ Point ------------------------

	/**
	 * Segment structure with two points.
	 * Both points are stored as begin of the segment (according to X axis)
	 * and end of the segment (according to X axis)
	 */

	struct segment {
		point begin;
		point end;

		/**
		 * Constructs segment from two points.
		 * @param begin begin point of the segment (according to X axis)
		 * @param end end point of the segment (according to X axis)
		 */

		segment(const point& begin, const point& end) noexcept : begin(begin), end(end) {}

		/**
		 * Constructs segment from two points.
		 * @param begin begin point of the segment (according to X axis)
		 * @param end end point of the segment (according to X axis)
		 */

		segment(point&& begin, point&& end) noexcept : begin(begin), end(end) {}

		/** Constructs segment with default points */
		segment() noexcept = default;
		~segment() noexcept = default;

		/**
		 * Reads two segment's points and sorts them according
		 * to input order and X-axis order. Then stores them in segment
		 *
		 * @param in input stream
		 * @param s segment to read
		 */

		friend inline std::istream& operator>> (std::istream& in, segment& s) noexcept {
			auto first = point::from_input(point::FIRST, in);
			auto second = point::from_input(point::SECOND, in);

			if (first.x < second.x) {
				first.seg_pos = point::BEGIN;
				second.seg_pos = point::END;
				s = { first, second };
				return in;
			}

			if (first.x > second.x) {
				second.seg_pos = point::BEGIN;
				first.seg_pos = point::END;
				s = { second, first };
				return in;
			}

			if (first.y < second.y) {
				first.seg_pos = point::BEGIN;
				second.seg_pos = point::END;
				s = { first, second };
				return in;
			}

			second.seg_pos = point::BEGIN;
			first.seg_pos = point::END;
			s = { second, first };
			return in;
		}

		/**
		 * Checks if point belongs to the segment's rectangle
		 * formed by 2 points: (min(X), min(Y)) and (max(X), max(Y)).
		 * Complexity is O(1)
		 *
		 * @param in input stream
		 * @param s segment to read
		 */

		[[nodiscard]] inline bool is_inside_area(const long long px, const long long py) const noexcept {
			return px <= std::max(begin.x, end.x)
					&& px >= std::min(begin.x, end.x)
					&& py <= std::max(begin.y, end.y)
					&& py >= std::min(begin.y, end.y);
		}

	private:

		/**
		 * Formula's fragment to calculate
		 * parts of intersection's formula.
		 * Obviously, complexity is O(1)
		 */

		[[nodiscard]] static constexpr inline long long formula(
				const long long a,
				const long long b,
				const long long c,
				const long long d,
				const long long e,
				const long long f,
				const long long g,
				const long long h
		) noexcept { return ((a - b) * (c - d)) - ((e - f) * (g - h)); }

		/**
		 * Calculates denominator part in intersection formula
		 * Obviously, complexity is O(1)
		 * @param seg segment for which denominator is calculated
		 * @return denominator for the formula
		 */

		[[nodiscard]] inline long long intersect_denominator(const segment& seg) const noexcept {
			const auto& a = begin;
			const auto& b = end;
			const auto& c = seg.begin;
			const auto& d = seg.end;
			return formula(a.x, b.x, c.y, d.y, a.y, b.y, c.x, d.x);
		}

		/**
		 * Calculates numerator part in intersection formula for X coordinate
		 * Obviously, complexity is O(1)
		 * @param seg segment for which X's numerator is calculated
		 * @return X's numerator for the formula
		 */

		[[nodiscard]] inline long long intersects_x(const segment& seg) const noexcept {
			const auto& a = begin;
			const auto& b = end;
			const auto& c = seg.begin;
			const auto& d = seg.end;

			auto numerator = formula(a.x * b.y, a.y * b.x, c.x, d.x, a.x, b.x, c.x * d.y, c.y * d.x);
			auto denominator = intersect_denominator(seg);
			return numerator / denominator;
		}

		/**
		 * Calculates numerator part in intersection formula for Y coordinate
		 * Obviously, complexity is O(1)
		 * @param seg segment for which Y's numerator is calculated
		 * @return Y's numerator for the formula
		 */

		[[nodiscard]] inline long long intersects_y(const segment& seg) const noexcept {
			const auto& a = begin;
			const auto& b = end;
			const auto& c = seg.begin;
			const auto& d = seg.end;

			auto numerator = formula(a.x * b.y, a.y * b.x, c.y, d.y, a.y, b.y, c.x * d.y, c.y * d.x);
			auto denominator = intersect_denominator(seg);
			return numerator / denominator;
		}

		/**
		 * Gets line equation from given segments.
		 * Uses y = kx + b equation for two points.
		 * Complexity is O(1)
		 * @return pair of K and B from y = Kx + B
		 */

		[[nodiscard]] inline auto get_line() const noexcept {
			// y0 = kx0 + b
			// y1 = kx1 + b
			//
			// b = y0 - kx0
			// y1 = y0 + k(x1 - x0)
			//
			// b = y0 - kx0
			// k = (y1 - y0) / (x1 - x0)

			auto k = end.y - begin.y / end.x - begin.x;
			auto b = begin.y - k * begin.x;
			return std::make_pair(k, b);
		}

		/**
		 * Checks if two segments belong to the same line.
		 * Gets y = kx + b equation for two points of this segment.
		 * Then checks if provided segment's coordinates belong to
		 * the calculated equation.
		 * Complexity is O(1)
		 * @return true if both segments are on the same line
		 */

		[[nodiscard]] inline bool on_same_line_with(const segment& seg) const noexcept {
			const auto [k, b] = get_line();
			return seg.begin.is_belong_to_line(k, b) && seg.end.is_belong_to_line(k, b);
		}

	public:

		/**
		 * Checks if two segments intersect with each other.
		 * Firstly calculates denominator from formula of intersection of two lines.
		 * If denominator is 0 -> they are either on the same line or on parallel lines.
		 * Otherwise, checks if intersection point really belongs to both segments.
		 * Complexity is O(1)
		 * @return true if both segments are intersecting
		 */

		[[nodiscard]] inline bool intersects_with(const segment& seg) const noexcept {
			if (intersect_denominator(seg) == 0) {
				if (!on_same_line_with(seg))
					return false;

				return seg.begin.x >= begin.x && seg.begin.x <= end.x
						|| seg.end.x >= begin.x && seg.end.x <= end.x;
			}

			auto ins_x = intersects_x(seg);
			auto ins_y = intersects_y(seg);
			return is_inside_area(ins_x, ins_y) && seg.is_inside_area(ins_x, ins_y);
		}

		/** Outputs segment in `${first.x} ${first.y} ${second.x} ${second.y}` format */

		friend inline std::ostream& operator<< (std::ostream& out, const segment& seg) noexcept {
			const auto& first = seg.begin.is_first() ? seg.begin : seg.end;
			const auto& second = seg.begin.is_second() ? seg.begin : seg.end;
			out << first.x << ' ' << first.y << ' ' << second.x << ' ' << second.y;
			return out;
		}
	};

	/**
	 * Compares two segments firstly by Y coordinates,
	 * then by X coordinates in descending order.
	 *
	 * @param lhs left argument
	 * @param rhs right argument
	 */

	struct segment_cmp {
		bool operator() (const segment& lhs, const segment& rhs) const noexcept {
			const std::vector<long long> lhs_arr = {
					std::max(lhs.begin.y, lhs.end.y),
					std::min(lhs.begin.y, lhs.end.y),
					std::max(lhs.begin.x, lhs.end.x),
					std::min(lhs.begin.x, lhs.end.x),
			};

			const std::vector<long long> rhs_arr = {
					std::max(rhs.begin.y, rhs.end.y),
					std::min(rhs.begin.y, rhs.end.y),
					std::max(rhs.begin.x, rhs.end.x),
					std::min(rhs.begin.x, rhs.end.x),
			};

			return lhs_arr < rhs_arr;
		}
	};

	using avl_segment_tree = dsa_hw::avl::avl_tree<dsa_hw::segment, dsa_hw::segment_cmp>;
}

#define LOG_INTERSECTION_AND_RETURN(SEG1, SEG2) \
	std::cout << "INTERSECTION\n";                 \
	std::cout << (SEG1) << '\n';                   \
	std::cout << (SEG2) << '\n';                   \
	return 0;

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	int n = 0;
	std::cin >> n;

	// Reading and sorting points firstly by X, then by Y, then by beginning status

	std::vector<std::pair<dsa_hw::point, dsa_hw::segment>> points(2 * n);

	for (int i = 0; i < points.size(); ++i) {
		dsa_hw::segment seg;
		std::cin >> seg;

		points[i] = std::make_pair(seg.begin, seg);
		points[++i] = std::make_pair(seg.end, seg);
	}

	dsa_hw::merge_sort<std::pair<dsa_hw::point, dsa_hw::segment>>(points.begin(), points.end(), [](const auto& lhs, const auto& rhs) {
		if (lhs.first.x == rhs.first.x) {
			if (lhs.first.y == rhs.first.y)
				return lhs.first.is_begin();
			return lhs.first.y < rhs.first.y;
		}
		return lhs.first.x < rhs.first.x;
	});

	// Traversing through points and store them in tree
	// according to their X-axis status in the segment (begin or end)

	dsa_hw::avl_segment_tree seg_tree;

	for (const auto& [point, seg] : points) {
		if (seg_tree.empty()) {
			// If tree is empty, just add the segment
			seg_tree.insert(seg);
			continue;
		}

		if (point.is_begin()) {
			// If point is the beginning of the segment,
			// inserting segment and checking on intersection
			// with previous and next segments from the tree (if they are present)

			auto seg_it = seg_tree.insert(seg);
			const auto prev_seg_it = std::prev(seg_it);

			if (prev_seg_it != seg_tree.end() && seg.intersects_with(*prev_seg_it)) {
				LOG_INTERSECTION_AND_RETURN(seg, *prev_seg_it);
			}

			const auto next_seg_it = std::next(seg_it);

			if (next_seg_it != seg_tree.end() && seg.intersects_with(*next_seg_it)) {
				LOG_INTERSECTION_AND_RETURN(seg, *next_seg_it);
			}
		} else {
			// If point is the beginning of the segment,
			// segment is already in the tree.
			// Checking if previous and next segments are intersecting

			auto seg_it = seg_tree.find(seg);
			const auto prev_seg_it = std::prev(seg_it);
			const auto next_seg_it = std::next(seg_it);

			if (prev_seg_it != seg_tree.end() && next_seg_it != seg_tree.end()) {
				const auto& prev_segment = *prev_seg_it;
				const auto& next_segment = *next_seg_it;

				if (prev_segment.intersects_with(next_segment)) {
					LOG_INTERSECTION_AND_RETURN(prev_segment, next_segment);
				}
			}

			// Removing the segment after all checks
			seg_tree.erase(seg_it);
		}
	}

	std::puts("NO INTERSECTIONS");
	return 0;
}
