// Arseny Savchenko

#include <iostream>
#include <memory>
#include <compare>
#include <vector>
#include <queue>
#include <algorithm>

namespace dsa_hw {
	namespace {
		template <typename T> requires std::three_way_comparable<T> struct tree_node :
				std::enable_shared_from_this<tree_node<T>> {
			std::shared_ptr<T> value;

			std::weak_ptr<tree_node<T>> parent;
			std::shared_ptr<tree_node<T>> less_child;
			std::shared_ptr<tree_node<T>> greater_child;

			tree_node() noexcept {
				value = std::make_shared<T>(nullptr);
				less_child = std::make_shared<tree_node<T>>(nullptr);
				greater_child = std::make_shared<tree_node<T>>(nullptr);
			};

			explicit tree_node(
					const T& val,
					std::weak_ptr<tree_node<T>> parent = std::weak_ptr<tree_node<T>>(),
					std::shared_ptr<tree_node<T>> less_child = nullptr,
					std::shared_ptr<tree_node<T>> greater_child = nullptr
			) noexcept : value(std::make_shared<T>(val)), parent(std::move(parent)),
				less_child(std::move(less_child)), greater_child(std::move(greater_child)) {}

			explicit tree_node(
					T&& val,
					std::weak_ptr<tree_node<T>> parent = std::weak_ptr<tree_node<T>>(),
					std::shared_ptr<tree_node<T>> less_child = std::make_shared<tree_node<T>>(nullptr),
					std::shared_ptr<tree_node<T>> greater_child = std::make_shared<tree_node<T>>(nullptr)
			) noexcept : value(std::make_shared<T>(val)), parent(parent),
				less_child(less_child), greater_child(greater_child) {}

			~tree_node() noexcept = default;

			void insert(const T& val) noexcept {
				if (val < *value) {
					if (!less_child) {
						less_child = std::make_shared<tree_node<T>>(
								tree_node(val, this->weak_from_this())
						);
						return;
					}

					less_child->insert(val);
					return;
				}

				if (!greater_child) {
					greater_child = std::make_shared<tree_node<T>>(
							tree_node(val, this->weak_from_this())
					);
					return;
				}

				greater_child->insert(val);
			}

			void insert(const T&& val) noexcept {
				if (val < *value) {
					if (!less_child) {
						less_child = std::make_shared(
								tree_node(val, this->weak_from_this())
						);
						return;
					}

					less_child->insert(val);
					return;
				}

				if (!greater_child) {
					greater_child = std::make_shared(
							tree_node(val, this->weak_from_this())
					);
					return;
				}

				greater_child->insert(val);
			}

			// Other traversals based on dfs have difficulties with indexing,
			// So it was easier to just write this unholy imperative bfs with queue.
			// Besides, we are allowed to use linked-list => queue
			// (even though in C++ it is based on deque, which is not typical linked-list
			// implemented deque, it is a little-bit close to Skip-List, but whatever,
			// it works as linked-list and queue)

			template<typename A> void bfs(A action) noexcept {
				std::size_t ind = 1;
				std::queue<tree_node<T>*> nodes; nodes.push(this);

				while (!nodes.empty()) {
					const auto* const front = nodes.front();
					action(*front, ind);
					if (front->less_child) { nodes.push(front->less_child.get()); ++ind; }
					if (front->greater_child) { nodes.push(front->greater_child.get()); ++ind; }
					nodes.pop();
				}
			}
		};
	}

	template <typename T> requires std::three_way_comparable<T> class binary_tree {
		std::size_t size = 0;
		std::shared_ptr<tree_node<T>> root = nullptr;

	public:
		[[nodiscard]] constexpr inline std::size_t get_size() const noexcept { return size; }
		[[nodiscard]] constexpr inline bool is_empty() const noexcept { return size == 0; }
		[[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return !is_empty(); }

		inline void insert(const T& value) noexcept {
			++size;

			if (!root) {
				root = std::make_shared<tree_node<T>>(tree_node(value));
				return;
			}

			root->insert(value);
		}

		inline void insert(T&& value) noexcept {
			++size;

			if (!root) {
				root = std::make_shared<tree_node<T>>(tree_node(value));
				return;
			}

			root->insert(value);
		}

		inline void debug_tree() const noexcept {
			if (root) {
				std::cout << size << std::endl;

				root->bfs([](const auto& node, std::size_t ind) {
					const int less_ind = node.less_child ? ++ind : -1;
					const int greater_ind = node.greater_child ? ++ind : -1;

					std::cout << *node.value << ' ';
					std::cout << less_ind << ' ';
					std::cout << greater_ind << std::endl;
				});

				std::cout << 1;
			}
		}
	};
}

int main() {
	std::size_t n = 0;
	std::scanf("%zu", &n);

	dsa_hw::binary_tree<int> tree;

	while (n--) {
		int e = 0;
		std::scanf("%d", &e);
		tree.insert(e);
	}

	tree.debug_tree();
	return 0;
}
