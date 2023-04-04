// Arseny Savchenko

#include <vector>
#include <queue>
#include <memory>
#include <ranges>
#include <algorithm>

namespace dsa_hw {
	template <typename V, typename E = void*, typename Eq = std::equal_to<V>> class graph {
		struct edge;

		struct vertex {
			std::shared_ptr<V> val = nullptr;
			std::vector<std::shared_ptr<edge>> connections;

			explicit vertex(const V& val) noexcept { this->val = std::make_shared<V>(val); }
			explicit vertex(V&& val) noexcept { this->val = std::make_shared<V>(val); }
			~vertex() noexcept = default;
		};

		struct edge {
			std::shared_ptr<E> val = nullptr;
			std::weak_ptr<vertex> from;
			std::weak_ptr<vertex> to;

			edge(const E& val, std::shared_ptr<vertex> from, std::shared_ptr<vertex> to) noexcept {
				this->val = std::make_shared<E>(val);
				this->from = std::weak_ptr(from);
				this->to = std::weak_ptr(to);
			}

			edge(E&& val, std::shared_ptr<vertex> from, std::shared_ptr<vertex> to) noexcept {
				this->val = std::make_shared<E>(val);
				this->from = std::weak_ptr(from);
				this->to = std::weak_ptr(to);
			}

			~edge() noexcept = default;
		};

		Eq eq;
		std::vector<std::shared_ptr<vertex>> adjacency_list;

	public:
		graph() noexcept = default;

		explicit graph(const std::size_t initial_capacity) noexcept {
			adjacency_list.reserve(initial_capacity);
		}

		~graph() noexcept = default;

		[[nodiscard]] constexpr inline std::size_t size() const noexcept {
			return adjacency_list.size();
		}

	private:
		inline void insert(std::shared_ptr<vertex> vertex) noexcept {
			adjacency_list.push_back(vertex);
		}

	public:
		inline void insert(const V& value) noexcept {
			insert(std::make_shared<vertex>(vertex(value)));
		}

		inline void insert(V&& value) noexcept {
			insert(std::make_shared<vertex>(vertex(value)));
		}

		inline void connect_unchecked(const V& from, const V& to, const E& edge_val) noexcept {
			const auto from_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&from, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, from);
			});

			const auto to_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&to, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, to);
			});

			auto e = std::make_shared<edge>(edge_val, *from_it, *to_it);
			auto& from_connections = (**from_it).connections;
			from_connections.push_back(e);
		}

		inline void connect_unchecked(const V& from, const V& to, E&& edge_val) noexcept {
			const auto from_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&from, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, from);
			});

			const auto to_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&to, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, to);
			});

			auto e = std::make_shared<edge>(edge_val, *from_it, *to_it);
			auto& from_connections = (**from_it).connections;
			from_connections.push_back(e);
		}

		inline void connect(const V& from, const V& to, const E& edge_val) noexcept {
			const auto from_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&from, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, from);
			});

			const auto to_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&to, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, to);
			});

			if (from_it == adjacency_list.end() || to_it == adjacency_list.end())
				throw std::runtime_error("Value is not present in the graph");

			auto& from_connections = (**from_it).connections;

			const auto from_connect_to_it = std::find_if(
					from_connections.begin(),
					from_connections.end(),
					[&to, this](const auto& edge_ptr) {
						return eq(*edge_ptr->to, to);
					}
			);

			if (from_connect_to_it != from_connections.end())
				throw std::runtime_error("Vertices are already connected");

			auto e = std::make_shared<edge>(edge_val, *from_it, *to_it);
			from_connections.push_back(e);
		}

		inline void connect(const V& from, const V& to, E&& edge_val) noexcept {
			const auto from_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&from, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, from);
			});

			const auto to_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&to, this](const auto& vertex_ptr) {
				return eq(*vertex_ptr->val, to);
			});

			if (from_it == adjacency_list.end() || to_it == adjacency_list.end())
				throw std::runtime_error("Value is not present in the graph");

			auto& from_connections = (**from_it).connections;

			const auto from_connect_to_it = std::find_if(
					from_connections.begin(),
					from_connections.end(),
					[&to, this](const auto& edge_ptr) {
						return eq(*edge_ptr->to.lock(), to);
					}
			);

			if (from_connect_to_it != from_connections.end())
				throw std::runtime_error("Vertices are already connected");

			auto e = std::make_shared<edge>(edge_val, *from_it, *to_it);
			from_connections.push_back(e);
		}

		[[nodiscard]] inline bool has_connection(const V& from, const V& to) const noexcept {
			const auto from_it = std::find_if(adjacency_list.begin(), adjacency_list.end(), [&from, this](const auto& vertex_ptr) {
				return eq(vertex_ptr->val, from);
			});

			auto& from_connections = from_it->connections;

			const auto from_connect_to_it = std::find_if(
					from_connections.begin(),
					from_connections.end(),
					[&to, this](const auto& edge_ptr) {
						return eq(*edge_ptr->to.lock(), to);
					}
			);

			return from_connect_to_it != from_connections.end();
		}

		[[nodiscard]] inline bool is_undirected_connected() const noexcept {
			// TODO: Implementation the case of directed graph
			std::vector<std::shared_ptr<vertex>> unvisited;
			std::queue<std::shared_ptr<vertex>> q;

			q.push(adjacency_list.front());
			std::copy(adjacency_list.begin() + 1, adjacency_list.end(), std::back_inserter(unvisited));

			while (!q.empty()) {
				auto front = q.front();

				std::erase_if(unvisited, [&front, this](const auto& v) {
					return eq(*v->val, *front->val);
				});

				std::ranges::for_each(
						front->connections
								| std::views::transform([](const auto& edge) {
									return edge->to.lock();
								})
								| std::views::filter([&unvisited, this](const auto& vertex) {
									return std::find_if(unvisited.begin(), unvisited.end(), [&vertex, this](const auto& v) {
										return eq(*vertex->val, *v->val);
									}) != unvisited.end();
								}),
						[&q](const auto& vertex) { q.push(vertex); }
				);

				q.pop();
			}

			return unvisited.empty();
		}
	};
}

int main() {
	std::size_t n = 0;
	std::scanf("%zu", &n);

	dsa_hw::graph<std::size_t> graph(n);

	for (std::size_t i = 0; i < n; ++i)
		graph.insert(i);

	for (std::size_t i = 0; i < n; ++i) {
		for (std::size_t q = 0; q < n; ++q) {
			int is_connected = 0;
			std::scanf("%d", &is_connected);

			if (is_connected)
				graph.connect_unchecked(i, q, nullptr);
		}
	}

	std::puts(graph.is_undirected_connected() ? "YES" : "NO");
	return 0;
}
