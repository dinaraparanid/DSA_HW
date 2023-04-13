// Arseny Savchenko

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <optional>
#include <algorithm>
#include <unordered_map>

#pragma GCC optimize("Ofast")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("avx,avx2,fma")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,tune=native")

namespace dsa_hw {
// ############################### AVL Tree Implementation ###############################

	namespace avl {

		// ############################### AVL Tree ###############################

		/**
		 * AVL it implementation that supports only unique values.
		 *
		 * The AVL it is a self-balancing binary search it,
		 * proposed by George Adelson-Velsky and Eugene Landis in 1962.
		 * The main condition of the AVL it is that for every vertex,
		 * the difference in height of subtrees less or equal to 1.
		 * In other case, the it is rebalanced with double rotations.
		 * Provides logarithmic complexity of searching, inserting and deleting elements in the it.
		 *
		 * @param T type of element
		 * @param Cmp comparator to sort elements in the it. By default, std::less<T> is used.
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
			 * Node of AVL it provides access
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
				 * @param height height of the it
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
				 * @param height height of the it
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
			 * Iterator to the node in it.
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
				const avl_tree<T, Cmp>* tree;

				/** Initializes iterator with pointers to the element and it */

				explicit iterator(
						std::shared_ptr<node> ptr,
						const avl_tree<T, Cmp>* tree
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
			/** Creates empty AVL it */
			avl_tree() noexcept = default;

			~avl_tree() noexcept = default;

			/**
			 * Gets number of elements in the it.
			 * Complexity is O(1)
			 * @return number of elements
			 */

			[[nodiscard]] constexpr inline std::size_t get_size() const noexcept {
				return size;
			}

			/**
			 * Checks if there are some elements in the it.
			 * Complexity is O(1)
			 * @return true if it is empty
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
			 * @param value value to push
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
					// value < node -> push to left
					auto [left_val, result] = insert(to_node->left, value);

					if (result == nullptr)
						return std::make_pair(to_node, nullptr);

					to_node->left = left_val;
					res = result;
				} else if (cmp(*to_node->value, value)) {
					// value > node -> push to right
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

				// Rebalancing it according to the balance factor

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
			 * @param value value to push
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
					// value < node -> push to left
					auto [left_val, result] = insert(to_node->left, value);

					if (result == nullptr)
						return std::make_pair(to_node, nullptr);

					to_node->left = left_val;
					res = result;
				} else if (cmp(*to_node->value, value)) {
					// value > node -> push to right
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

				// Rebalancing it according to the balance factor

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
			 * Inserts element to the it.
			 * If value is already present in the node, does nothing
			 * and returns end iterator.
			 * Provides logarithmic complexity with it's height
			 *
			 * @param value value to push
			 * @return iterator to inserted element or end iterator
			 */

			inline iterator insert(const T& value) noexcept {
				auto res = insert(root, value).second;
				if (res) ++size;
				return iterator(res, this);
			}

			/**
			 * Inserts element to the it.
			 * If value is already present in the node, does nothing
			 * and returns end iterator.
			 * Provides logarithmic complexity with it's height
			 *
			 * @param value value to push
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
			 * @param value value to push
			 * @return new local root
			 */

			std::shared_ptr<node> erase(
					std::shared_ptr<node> to_node,
					const T& value
			) noexcept {
				// If node does not exist
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

				// Rebalancing it according to the balance factor

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
			 * Deletes element from the it by iterator to that node.
			 * Provides logarithmic complexity with NODE's height
			 *
			 * @param to_node node in which value should be inserted
			 * @param value value to push
			 * @return new local root
			 */

			inline void erase(const iterator node) noexcept {
				root = erase(root, *node.ptr->value);
				--size;
			}

			/**
			 * Deletes element from the it by its value.
			 * Provides logarithmic complexity with TREE's height
			 *
			 * @param to_node node in which value should be inserted
			 * @param value value to push
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
			 * Provides logarithmic complexity with it's height
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
			 * Provides logarithmic complexity with it's height
			 * @param to_node node in which searching
			 * @param value value to search
			 * @return iterator to such element or end iterator
			 */

			[[nodiscard]] iterator greater_or_equal(const T& key) noexcept {
				return iterator(greater_or_equal(root, key), this);
			}

			/**
			 * Searches for a node with value <= than given key.
			 * Provides logarithmic complexity with it's height
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
			 * Provides logarithmic complexity with it's height
			 * @param it iterator for which we are looking to find previous
			 * @return iterator to previous value or end iterator
			 */

			[[nodiscard]] inline iterator prev(const iterator& it) noexcept {
				return iterator(prev(root, it.ptr), this);
			}

			/**
			 * Searches for an iterator with next value in ascending order.
			 * Provides logarithmic complexity with it's height
			 * @param it iterator for which we are looking to find next
			 * @return iterator to next value or end iterator
			 */

			[[nodiscard]] inline iterator next(const iterator& node) noexcept {
				return iterator(next(root, node.ptr), this);
			}

			/**
			 * Gets iterator with smallest value (or just leftest node).
			 * Provides logarithmic complexity with it's height
			 * @return iterator with smallest value
			 */

			[[nodiscard]] constexpr inline iterator begin() const noexcept {
				return iterator(node::min(root), this);
			}

			/**
			 * Gets iterator with smallest value (or just leftest node).
			 * Provides logarithmic complexity with it's height
			 * @return iterator with smallest value
			 */

			[[nodiscard]] constexpr inline iterator end() const noexcept {
				return iterator(nullptr, nullptr);
			}
		};
	}

	namespace priority_queue {

		// Since any BST is actually satisfies interface of Priority Queue,
		// It will be appropriate to just wrap any BST implementation and call it
		// Priority Queue. Consider it as a Binary Heap implementation

		template <typename T, typename Cmp = std::less<T>> class priority_queue {
			avl::avl_tree<T, Cmp> data;

		public:
			inline void push(const T& value) noexcept { data.insert(value); }
			inline void push(T&& value) noexcept { data.insert(value); }

			[[nodiscard]] constexpr inline std::size_t get_size() const noexcept {
				return data.get_size();
			}

			[[nodiscard]] constexpr inline bool empty() const noexcept {
				return data.empty();
			}

			[[nodiscard]] inline const T& peek() const noexcept { return *data.begin(); }
			inline void pop() noexcept { data.erase(data.begin()); }
		};
	}

	#define GRAPH_TEMPLATE \
		typename V,           \
		typename VKey,        \
		typename E,           \
		typename VEq = std::equal_to<V>, \
		typename VKeyEq = std::equal_to<VKey>, \
		typename EdgeEq = std::equal_to<E>,    \
		typename VertexCmp = std::less<VKey>,  \
		typename VKeyCmp = std::less<VKey>,    \
		typename EdgeCmp = std::less<E>, \
		typename VKeyHasher = std::hash<VKey>

	template <GRAPH_TEMPLATE> class graph {
		struct edge;

		struct vertex {
			V val;
			std::vector<edge> connections;

			explicit vertex(const V& val) noexcept : val(val) {}
			explicit vertex(V&& val) noexcept : val(val) {}

			vertex() noexcept = default;
			~vertex() noexcept = default;
		};

		struct edge {
			E val;
			VKey from_key;
			VKey to_key;

			edge() noexcept = default;

			edge(const E& val, const VKey& from_key, const VKey& to_key) noexcept :
				val(val), from_key(from_key), to_key(to_key) {}

			edge(E&& val, VKey&& from_key, VKey&& to_key) noexcept :
				val(val), from_key(from_key), to_key(to_key) {}

			~edge() noexcept = default;
		};

		struct edge_ptr_cmp {
			VKeyEq v_eq;
			EdgeEq eq;

			EdgeCmp cmp;
			VKeyCmp v_cmp;

			[[nodiscard]] inline bool operator() (const edge* const v, const edge* const u) const noexcept {
				if (eq(v->val, u->val)) {
					if (v_eq(v->from_key, u->from_key))
						return v_cmp(v->to_key, u->to_key);

					return v_cmp(v->from_key, u->from_key);
				}

				return cmp(v->val, u->val);
			}
		};

		VEq eq;
		std::unordered_map<VKey, vertex, VKeyHasher, VKeyEq> adjacency_list;

	public:
		graph() noexcept = default;

		explicit graph(const std::size_t initial_capacity) noexcept {
			adjacency_list.reserve(initial_capacity);
		}

		~graph() noexcept = default;

		[[nodiscard]] constexpr inline std::size_t size() const noexcept {
			return adjacency_list.size();
		}

	public:
		inline void insert(const VKey& key, const V& value) noexcept {
			adjacency_list.insert(std::make_pair(key, vertex(value)));
		}

		inline void insert(const VKey& key, V&& value) noexcept {
			adjacency_list.insert(std::make_pair(key, vertex(value)));
		}

		[[nodiscard]] inline const V& operator[] (const VKey& key) const noexcept {
			return adjacency_list[key].val;
		}

		[[nodiscard]] inline V& operator[] (const VKey& key) noexcept {
			return adjacency_list[key].val;
		}

		[[nodiscard]] inline bool has_connection(const VKey& from, const VKey& to) const noexcept {
			const auto& from_connections = adjacency_list[from].connections;

			const auto from_connect_to_it = std::find_if(
					from_connections.begin(),
					from_connections.end(),
					[&to, this](const auto& edge) { return eq(edge.to_key, to); }
			);

			return from_connect_to_it != from_connections.end();
		}

		inline void connect_unchecked(const VKey& from, const VKey& to, const E& edge_val) noexcept {
			adjacency_list[from].connections.push_back(edge(edge_val, from, to));
		}

		inline void connect_unchecked(VKey&& from, VKey&& to, E&& edge_val) noexcept {
			adjacency_list[from].connections.push_back(edge(edge_val, from, to));
		}

		inline void connect(const VKey& from, const VKey& to, const E& edge_val) noexcept {
			const auto from_it = adjacency_list.find(from);
			const auto to_it = adjacency_list.find(to);

			if (from_it == adjacency_list.end() || to_it == adjacency_list.end())
				throw std::runtime_error("Value is not present in the graph");

			auto& from_connections = from_it->connections;

			const auto from_connect_to_it = std::find_if(
					from_connections.begin(),
					from_connections.end(),
					[&to, this](const auto& edge) { return eq(edge.to_key, to); }
			);

			if (from_connect_to_it != from_connections.end())
				throw std::runtime_error("Vertices are already connected");

			connect_unchecked(from, to, edge_val);
		}

		inline void connect(VKey&& from, VKey&& to, E&& edge_val) noexcept {
			const auto from_it = adjacency_list.find(from);
			const auto to_it = adjacency_list.find(to);

			if (from_it == adjacency_list.end() || to_it == adjacency_list.end())
				throw std::runtime_error("Value is not present in the graph");

			auto& from_connections = from_it->connections;

			const auto from_connect_to_it = std::find_if(
					from_connections.begin(),
					from_connections.end(),
					[&to, this](const auto& edge) { return eq(edge.to_key, to); }
			);

			if (from_connect_to_it != from_connections.end())
				throw std::runtime_error("Vertices are already connected");

			connect_unchecked(from, to, edge_val);
		}

		[[nodiscard]] inline graph directed_minimal_spanning_tree() const noexcept {
			graph tree;

			if (adjacency_list.empty())
				return tree;

			std::unordered_map<VKey, const vertex*, VKeyHasher, VKeyEq> unconnected_vertices;
			priority_queue::priority_queue<const edge*, edge_ptr_cmp> priority_queue;

			for (const auto& [key, vtx] : adjacency_list)
				unconnected_vertices[key] = &vtx;

			while (!unconnected_vertices.empty()) {
				auto vertex_it = unconnected_vertices.begin();
				const auto& vrt_key = vertex_it->first;
				auto* vtx_ptr = vertex_it->second;

				unconnected_vertices.erase(vertex_it);
				tree.insert(vrt_key, vtx_ptr->val);

				for (const auto& e : vtx_ptr->connections)
					priority_queue.push(&e);

				while (!priority_queue.empty()) {
					while (!priority_queue.empty() && !unconnected_vertices.contains(priority_queue.peek()->to_key))
						priority_queue.pop();

					if (priority_queue.empty())
						break;

					auto* edge_ptr = priority_queue.peek();
					const auto& to = adjacency_list.find(edge_ptr->to_key)->second;
					priority_queue.pop();

					for (const auto& e : to.connections)
						if (unconnected_vertices.contains(edge_ptr->to_key))
							priority_queue.push(&e);

					unconnected_vertices.erase(edge_ptr->to_key);
					tree.insert(edge_ptr->to_key, to.val);
					tree.connect_unchecked(edge_ptr->from_key, edge_ptr->to_key, edge_ptr->val);
				}
			}

			return tree;
		}

		[[nodiscard]] inline graph undirected_minimal_spanning_tree() const noexcept {
			graph tree;

			if (adjacency_list.empty())
				return tree;

			std::unordered_map<VKey, const vertex*, VKeyHasher, VKeyEq> unconnected_vertices;
			avl::avl_tree<const edge*, edge_ptr_cmp> priority_queue;

			for (const auto& [key, vtx] : adjacency_list)
				unconnected_vertices[key] = &vtx;

			while (!unconnected_vertices.empty()) {
				auto vertex_it = unconnected_vertices.begin();
				const auto& vrt_key = vertex_it->first;
				auto* vtx_ptr = vertex_it->second;

				unconnected_vertices.erase(vertex_it);
				tree.insert(vrt_key, vtx_ptr->val);

				for (const auto& e : vtx_ptr->connections)
					priority_queue.insert(&e);

				while (!priority_queue.empty()) {
					while (!priority_queue.empty() && !unconnected_vertices.contains((*priority_queue.begin())->to_key))
						priority_queue.erase(priority_queue.begin());

					if (priority_queue.empty())
						break;

					auto* edge_ptr = *priority_queue.begin();
					const auto& to = adjacency_list.find(edge_ptr->to_key)->second;
					priority_queue.erase(priority_queue.begin());

					for (const auto& e : to.connections)
						if (unconnected_vertices.contains(edge_ptr->to_key))
							priority_queue.insert(&e);

					unconnected_vertices.erase(edge_ptr->to_key);
					tree.insert(edge_ptr->to_key, to.val);

					tree.connect_unchecked(edge_ptr->from_key, edge_ptr->to_key, edge_ptr->val);
					tree.connect_unchecked(edge_ptr->to_key, edge_ptr->from_key, edge_ptr->val);
				}
			}

			return tree;
		}

		friend inline std::ostream& operator<< (
				std::ostream& out,
				const graph& graph
		) noexcept {
			for (const auto& v : graph.adjacency_list)
				for (const auto& e : v.second.connections)
					out << v.first << ':' << e.to_key << ' ';
			return out;
		}
	};

	struct vertex {
		std::string name;
		int penalty = 0;

		vertex(const std::string& name, const int penalty) noexcept : name(name), penalty(penalty) {}
		vertex(std::string&& name, const int penalty) noexcept : name(name), penalty(penalty) {}

		vertex() noexcept = default;
		~vertex() noexcept = default;

		[[nodiscard]] inline bool operator< (const vertex& other) const noexcept { return name < other.name; }

		friend inline std::ostream& operator<< (std::ostream& out, const vertex& v) noexcept {
			out << v.name;
			return out;
		}
	};

	[[nodiscard]] inline double edge_weight(
			const int penalty_1,
			const int penalty_2,
			const int distance
	) noexcept {
		return static_cast<double>(distance) / (penalty_1 + penalty_2);
	}
}

namespace std {
	template <> struct equal_to<dsa_hw::vertex> {
		[[nodiscard]] bool operator() (const dsa_hw::vertex& rhs, const dsa_hw::vertex& lhs) const noexcept {
			return rhs.name == lhs.name;
		}
	};
}

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	int n = 0;
	std::cin >> n;
	dsa_hw::graph<dsa_hw::vertex, std::string, double> graph;

	while (n--) {
		std::string inp;
		std::cin >> inp;

		if (inp == "ADD") {
			std::string point;
			std::cin >> point;

			int penalty = 0;
			std::cin >> penalty;

			graph.insert(point, dsa_hw::vertex(point, penalty));
			continue;
		}

		if (inp == "CONNECT") {
			std::string point_1, point_2;
			std::cin >> point_1 >> point_2;

			int distance = 0;
			std::cin >> distance;

			const auto& vertex_1 = graph[point_1];
			const auto& vertex_2 = graph[point_2];
			const auto weight = dsa_hw::edge_weight(vertex_1.penalty, vertex_2.penalty, distance);

			graph.connect_unchecked(point_1, point_2, weight);
			graph.connect_unchecked(point_2, point_1, weight);
			continue;
		}

		auto mst = graph.directed_minimal_spanning_tree();
		std::cout << mst << std::endl;
	}

	return 0;
}
