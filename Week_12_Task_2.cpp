// Arseny Savchenko

#include <cstdio>
#include <vector>
#include <queue>
#include <memory>
#include <optional>
#include <set>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_map>

#pragma GCC optimize("Ofast")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("avx,avx2,fma")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,tune=native")

namespace dsa_hw {
	namespace priority_queue {

		/**
		 * Since any BST is actually satisfies interface of Priority Queue,
		 * It will be appropriate to just wrap any BST implementation and call it
		 * Priority Queue. Consider it as a Binary Heap implementation
		 */

		template <typename T, typename Cmp = std::less<T>> class priority_queue {
			std::set<T, Cmp> data;

		public:
			/**
			 * Inserts new element to the queue.
			 * Complexity is logarithmic.
			 * @param value value to insert
			 */

			inline void push(const T& value) noexcept { data.insert(value); }

			/**
			 * Inserts new element to the queue.
			 * Complexity is logarithmic.
			 * @param value value to insert
			 */

			inline void push(T&& value) noexcept { data.insert(value); }

			/**
			 * Gets number of elements in the queue. Complexity is O(1).
			 * @return number of elements in the queue
			 */

			[[nodiscard]] constexpr inline std::size_t size() const noexcept {
				return data.size();
			}

			/**
			 * Checks if queue is empty. Complexity is O(1).
			 * @return true if queue is empty
			 */

			[[nodiscard]] constexpr inline bool empty() const noexcept {
				return data.empty();
			}

			/**
			 * Gets lowest element of queue. Complexity is O(1)
			 * (see https://en.cppreference.com/w/cpp/container/set/begin).
			 * Note that this method does not properly handles situation
			 * where queue may be empty.
			 * @return reference to the first element
			 */

			[[nodiscard]] inline const T& peek() const noexcept { return *data.begin(); }

			/**
			 * Removes lowest element of queue. Complexity is logarithmic.
			 * Note that this method does not properly handles situation
			 * where queue may be empty.
			 * @return reference to the first element
			 */

			inline void pop() noexcept { data.erase(data.begin()); }
		};
	}

	namespace graph {

		/**
		 * Representation of Graph ADL.
		 * Graph is a set of vertices and a relation between them described as edges.
		 * Both vertices and edges may and may not have some data.
		 * In order to recognize, compare and contrast different vertices,
		 * next implementations of the graph use keys of vertices.
		 * Implementation allows to insert, connect and read vertices in the graph
		 *
		 * @param VKey key of the vertex
		 * @param V data of the vertex (default may be void*)
		 * @param E data of the edge between vertices (default mat be void*)
		 */

		template <typename VKey, typename V, typename E> struct graph {
			[[nodiscard]] virtual std::size_t vertices_number() const noexcept = 0;

			virtual void insert(const VKey& key, const V& value) noexcept = 0;
			virtual void insert(const VKey& key, V&& value) noexcept = 0;

			[[nodiscard]] virtual const V& operator[] (const VKey& key) const noexcept = 0;
			[[nodiscard]] virtual V& operator[] (const VKey& key) noexcept = 0;

			[[nodiscard]] virtual bool has_connection(const VKey& from, const VKey& to) const noexcept = 0;
			virtual void connect(const VKey& from, const VKey& to, const E& edge_val) noexcept = 0;
			virtual void connect(VKey&& from, VKey&& to, E&& edge_val) noexcept = 0;
		};

		/**
		 * Graph ADL which can construct minimal spanning trees.
		 * MST is a subset of the edges of a connected, edge-weighted graph
		 * that connects all the vertices together without any cycles.
		 * Total edge weight must be minimum possible.
		 * Implementation allows to construct both directed and undirected trees.
		 *
		 * @param VKey key of the vertex
		 * @param V data of the vertex (default can be void*)
		 * @param E data of the edge between vertices (default can be void*)
		 * @param ResGraph resulting graph type after operation (probably decltype(*this))
		 */

		template <typename VKey, typename V, typename E, typename ResGraph> struct span_tree_graph : public graph<VKey, V, E> {
			[[nodiscard]] virtual ResGraph directed_minimal_spanning_tree() const noexcept = 0;
			[[nodiscard]] virtual ResGraph undirected_minimal_spanning_tree() const noexcept = 0;
		};

		/**
		 * Graph ADL which can detect negative cycles with Bellman-Ford algorithm.
		 * A negative cycle is a cycle whose edges' weights' sum is a negative value.
		 * Implementation allows to find any of such cycles as the path.
		 *
		 * @param VKey key of the vertex
		 * @param V data of the vertex (default can be void*)
		 * @param E data of the edge between vertices (default can be void*)
		 */

		template <typename VKey, typename V, std::integral E> struct negative_cycles_graph : public graph<VKey, V, E> {
			[[nodiscard]] virtual std::optional<std::vector<VKey>> has_negative_cycles() const noexcept = 0;
		};

		/**
		 * Graph ADL which can find minimum paths with Dijkstra's algorithm.
		 * Dijkstra's algorithm is an algorithm for finding the shortest paths
		 * between nodes in a weighted graph.
		 * Implementation requires distance to edge as an integer value.
		 * Allows to find all minimum distances from given vertex to all other.
		 * Besides, helps to acquire paths with list of vertices' parents
		 *
		 * @param VKey key of the vertex
		 * @param V data of the vertex (default can be void*)
		 * @param E data of the edge between vertices (default can be void*)
		 * @param D distance to the edge
		 * @param DistCalc calculates distance to the edge by its value [override operator() for custom distance calculator]
		 */

		template <typename VKey, typename V, typename E, std::integral D, typename DistCalc> struct dijkstra_graph : public graph<VKey, V, E> {
			using distances_to_parents = std::pair<std::vector<D>, std::vector<std::optional<VKey>>>;
			static const D INF = INT32_MAX;

			[[nodiscard]] virtual distances_to_parents dijkstra(const VKey& from) const noexcept = 0;

			[[nodiscard]] static std::vector<VKey> path(
					const VKey& to,
					const std::vector<std::optional<VKey>>& parents
			) noexcept {
				std::vector<VKey> path = { to };

				for (auto cur_parent = parents[to]; cur_parent.has_value(); cur_parent = parents[cur_parent.value()])
					path.push_back(cur_parent.value());

				std::reverse(path.begin(), path.end());
				return path;
			}
		};

		/**
		 * Extension to Dijkstra's graph.
		 * Allows to execute same dijkstra's algorithm with additional edges' filter.
		 * Implementation requires distance to edge as an integer value.
		 *
		 * @param VKey key of the vertex
		 * @param V data of the vertex (default can be void*)
		 * @param E data of the edge between vertices (default can be void*)
		 * @param D distance to the edge
		 * @param DistCalc calculates distance to the edge by its value
		 * @param EdgeFilter filter's edge by its value
		 */

		template <typename VKey, typename V, typename E, std::integral D, typename DistCalc, typename EdgeFilter> struct dijkstra_filter_graph : public dijkstra_graph<VKey, V, E, D, DistCalc> {
			[[nodiscard]] virtual dijkstra_graph<VKey, V, E, D, DistCalc>::distances_to_parents dijkstra(const VKey& from, EdgeFilter filter) const noexcept = 0;
		};

		#define LIST_GRAPH_TEMPLATE_DECLARE \
				typename VKey,                     \
				typename V = void*,                \
				typename E = void*,                \
				typename VKeyEq = std::equal_to<VKey>, \
				typename VEq = std::equal_to<V>,       \
				typename EdgeEq = std::equal_to<E>,    \
				typename VKeyCmp = std::less<VKey>,    \
				typename VCmp = std::less<VKey>,       \
				typename EdgeCmp = std::less<E>,       \
				typename VKeyHasher = std::hash<VKey>

		#define LIST_GRAPH_TEMPLATE_USAGE \
				VKey, V, E, VKeyEq, VEq, EdgeEq, VKeyCmp, VCmp, EdgeCmp, VKeyHasher

		/**
		 * Generic directed adjacency list graph implementation.
		 * Stores all vertices into hash table, where VKey is the key of vertex itself.
		 * Vertex data structure stores value as V and contains all references to connections,
		 * where this vertex is the beginning (from point) of the edge.
		 *
		 * @param VKey key of vertex
		 * @param V vertex's data (default is void* if no data is required)
		 * @param E edge's data (usually, represents weight; default is void* if no data is required)
		 * @param VKeyEq equality comparator for vertex key's data (default is std::equal_to<VKey>)
		 * @param VEq equality comparator for vertex's data (default is std::equal_to<V>)
		 * @param EdgeEq equality comparator for edge's data (default is std::equal_to<E>)
		 * @param VKeyCmp less comparator for vertex key's data (default is std::less<VKey>)
		 * @param VCmp less comparator for vertex's data (default is std::less<V>)
		 * @param EdgeCmp less comparator for edge's data (default is std::less<E>)
		 * @param VKeyHasher hasher for vertex key (default is std::hash<VKey>)
		 */

		template <LIST_GRAPH_TEMPLATE_DECLARE> class list_graph :
				public span_tree_graph<VKey, V, E, list_graph<LIST_GRAPH_TEMPLATE_USAGE>>,
				public negative_cycles_graph<VKey, V, E>
		{
			struct edge;

			/**
			 * Vertex structure with value and all its connections,
			 * where this vertex is the beginning of edge
			 */

			struct vertex {
				V val = V();
				std::vector<edge> connections;

				explicit vertex(const V& val) noexcept : val(val) {}
				explicit vertex(V&& val) noexcept : val(val) {}

				vertex() noexcept = default;
				~vertex() noexcept = default;
			};

			/**
			 * Edge structure with value (usually, represents weight), from and to vertices keys.
			 * Vertices themselves can be obtained from the adjacency_list by keys
			 */

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

			/**
			 * Comparator for edges by pointer.
			 * Used in spanning tree algorithm to avoid usage of std::weak_ptr
			 * (and potential unnecessary thread synchronization during
			 * transformation to std::shared_ptr).
			 *
			 * 1) Compares edges' values according to EdgeCmp
			 * 2) Checks if they are equal according to EdgeEq
			 * 3) Compares from vertices' keys according to VKeyCmp
			 * 4) Checks if they are equal according to VKeyEq
			 * 5) Does the same with to vertices
			 *
			 * Complexity is O(VKeyEq + EdgeEq + VKeyCmp + EdgeCmp)
			 */

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

			VKeyEq eq;
			std::unordered_map<VKey, vertex, VKeyHasher, VKeyEq> adjacency_list;

		public:
			/** Creates empty list_graph */
			list_graph() noexcept = default;

			/**
			 * Creates list_graph with initial capacity.
			 * It is recommended to use this method for better performance
			 * @param initial_capacity preferred number of vertices that will be used
			 */

			explicit list_graph(const std::size_t initial_capacity) noexcept {
				adjacency_list.reserve(initial_capacity);
			}

			~list_graph() noexcept = default;

			/**
			 * Number of vertices in the list graph.
			 * Complexity is O(1)
			 * @return number of vertices in the graph
			 */

			[[nodiscard]] constexpr inline std::size_t vertices_number() const noexcept override {
				return adjacency_list.size();
			}

			/**
			 * Inserts new vertex to the list graph.
			 * Complexity is O(1)
			 * @param key key of vertex
			 * @param value vertex's value
			 */

			inline void insert(const VKey& key, const V& value) noexcept override {
				adjacency_list.insert(std::make_pair(key, vertex(value)));
			}

			/**
			 * Inserts new vertex to the list graph.
			 * Complexity is O(VKeyHash(VKey))
			 * @param key key of vertex
			 * @param value vertex's value
			 */

			inline void insert(const VKey& key, V&& value) noexcept override {
				adjacency_list.insert(std::make_pair(key, vertex(value)));
			}

		private:
			/**
			 * Gets vertex by its key. Complexity is O(VKeyHash(VKey)).
			 * Note that this method does not properly handles case
			 * where key is not present in the list graph (default vertex will be created)
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline const vertex& get_vertex(const VKey& key) const noexcept {
				return adjacency_list.find(key)->second;
			}

			/**
			 * Gets vertex by its key. Complexity is O(VKeyHash(VKey)).
			 * Note that this method does not properly handles case
			 * where key is not present in the list graph (default vertex will be created)
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline vertex& get_vertex(const VKey& key) noexcept {
				return adjacency_list[key];
			}

		public:
			/**
			 * Gets vertex by its key. Complexity is O(VKeyHash(VKey)).
			 * Note that this method does not properly handles case
			 * where key is not present in the list graph (SEGFAULT)
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline const V& operator[] (const VKey& key) const noexcept override {
				return get_vertex(key).val;
			}

			/**
			 * Gets vertex by its key. Complexity is O(VKeyHash(VKey)).
			 * Note that this method does not properly handles case
			 * where key is not present in the list graph (default vertex will be created)
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline V& operator[] (const VKey& key) noexcept override {
				return adjacency_list[key].val;
			}

			/**
			 * Checks if two vertices have connection.
			 * Complexity is O(VKeyHash(VKey) + E)
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @return true if edge between two vertices exists
			 */

			[[nodiscard]] inline bool has_connection(const VKey& from, const VKey& to) const noexcept override {
				const auto& from_connections = get_vertex(from).connections;

				const auto from_connect_to_it = std::find_if(
						from_connections.begin(),
						from_connections.end(),
						[&to, this](const auto& edge) { return eq(edge.to_key, to); }
				);

				return from_connect_to_it != from_connections.end();
			}

			/**
			 * Connects two vertices with an edge. Complexity is O(VKeyHash(VKey)).
			 * Note that this method does not properly handles case
			 * where connection between vertices is already exists
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 */

			inline void connect_unchecked(const VKey& from, const VKey& to, const E& edge_val) noexcept {
				adjacency_list[from].connections.push_back(edge(edge_val, from, to));
			}

			/**
			 * Connects two vertices with an edge. Complexity is O(VKeyHash(VKey)).
			 * Note that this method does not properly handles case
			 * where connection between vertices is already exists.
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 */

			inline void connect_unchecked(VKey&& from, VKey&& to, E&& edge_val) noexcept {
				adjacency_list[from].connections.push_back(edge(edge_val, from, to));
			}

			/**
			 * Connects two vertices with an edge.
			 * Complexity is O(VKeyHash(VKey) + Edges(VKey)).
			 * In case if connection is already present, std::runtime_error will be thrown.
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 * @throws std::runtime_error if connection is already present
			 */

			inline void connect(const VKey& from, const VKey& to, const E& edge_val) noexcept override {
				const auto from_it = adjacency_list.find(from);
				const auto to_it = adjacency_list.find(to);

				if (from_it == adjacency_list.end() || to_it == adjacency_list.end())
					throw std::runtime_error("Value is not present in the list graph");

				auto& from_connections = from_it->second.connections;

				const auto from_connect_to_it = std::find_if(
						from_connections.begin(),
						from_connections.end(),
						[&to, this](const auto& edge) { return eq(edge.to_key, to); }
				);

				if (from_connect_to_it != from_connections.end())
					throw std::runtime_error("Vertices are already connected");

				connect_unchecked(from, to, edge_val);
			}

			/**
			 * Connects two vertices with an edge.
			 * Complexity is O(VKeyHash(VKey) + Edges(VKey)).
			 * In case if connection is already present, std::runtime_error will be thrown.
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 * @throws std::runtime_error if connection is already present
			 */

			inline void connect(VKey&& from, VKey&& to, E&& edge_val) noexcept override {
				const auto from_it = adjacency_list.find(from);
				const auto to_it = adjacency_list.find(to);

				if (from_it == adjacency_list.end() || to_it == adjacency_list.end())
					throw std::runtime_error("Value is not present in the list graph");

				auto& from_connections = from_it->second.connections;

				const auto from_connect_to_it = std::find_if(
						from_connections.begin(),
						from_connections.end(),
						[&to, this](const auto& edge) { return eq(edge.to_key, to); }
				);

				if (from_connect_to_it != from_connections.end())
					throw std::runtime_error("Vertices are already connected");

				connect_unchecked(from, to, edge_val);
			}

			/**
			 * Gets all edges of the list graph.
			 * Complexity is O(E).
			 * @return all edges of the list graph
			 */

			[[nodiscard]] inline std::vector<edge> edges() const noexcept {
				std::vector<edge> edges;

				for (const auto& [key, vertex] : adjacency_list) {
					const auto& vertex_connection = vertex.connections;
					std::copy(vertex_connection.begin(), vertex_connection.end(), std::back_inserter(edges));
				}

				return edges;
			}

			/**
			 * Implementation of Minimal Spanning Tree algorithm for directed list graph.
			 * Complexity is dependant on Priority Queue implementation (default is std::set).
			 * For the default case, complexity is O(E * log(V))
			 * @return resulting MSP of the list graph
			 */

			[[nodiscard]] inline list_graph directed_minimal_spanning_tree() const noexcept override {
				list_graph tree;

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

			/**
			 * Implementation of Minimal Spanning Tree algorithm for undirected list graph.
			 * Complexity is dependant on Priority Queue implementation (default is std::set).
			 * For the default case, complexity is O(E * log(V))
			 * @return resulting MSP of the list graph
			 */

			[[nodiscard]] inline list_graph undirected_minimal_spanning_tree() const noexcept override {
				list_graph tree;

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
						tree.connect_unchecked(edge_ptr->to_key, edge_ptr->from_key, edge_ptr->val);
					}
				}

				return tree;
			}

			/**
			 * Checking if list graph has negative cycles with Bellman-Ford algorithm.
			 * Complexity is O(V * E).
			 * Note that this method requires E type to be a number with at least 32 bits.
			 * @return negative cycle or std::nullopt
			 */

			[[nodiscard]] inline std::optional<std::vector<VKey>> has_negative_cycles() const noexcept override {
				const E INF = 1e9;

				const auto vertices_num = adjacency_list.size();
				std::vector<E> distances(vertices_num);
				std::vector<const VKey*> parents(vertices_num, nullptr);

				const VKey* negative_cycle_checker = nullptr;

				for (int i = 0; i < vertices_num; ++i) {
					negative_cycle_checker = nullptr;

					for (const auto& [key, vertex] : adjacency_list) {
						const auto& connections = vertex.connections;

						for (const auto& edge : connections) {
							if (distances[edge.to_key] > distances[edge.from_key] + edge.val) {
								distances[edge.to_key] = std::max(-INF, distances[edge.from_key] + edge.val);
								parents[edge.to_key] = &edge.from_key;
								negative_cycle_checker = &edge.to_key;
							}
						}
					}
				}

				if (negative_cycle_checker == nullptr)
					return std::nullopt;

				const VKey* ancestor = negative_cycle_checker;

				for (int i = 0; i < vertices_num; ++i)
					ancestor = parents[*ancestor];

				std::vector<VKey> path;

				for (auto cur_anc = *ancestor; ; cur_anc = *parents[cur_anc]) {
					path.push_back(cur_anc);

					if (cur_anc == *ancestor && path.size() > 1)
						break;
				}

				std::reverse(path.begin(), path.end());
				return std::make_optional(path);
			}
		};

		#define MATRIX_GRAPH_TEMPLATE_DECLARE \
			std::integral VKey,                  \
			typename V,                          \
			typename E,                          \
			std::integral D,                     \
			typename DistCalc,                   \
			typename EdgeFilter

		#define MATRIX_GRAPH_TEMPLATE_USAGE VKey, V, E, D, DistCalc, EdgeFilter

		/**
		 * Generic directed adjacency matrix graph implementation.
		 * Stores all vertices into hash table, where VKey is the key of vertex itself.
		 * Vertex data structure stores value as V and contains all references to connections,
		 * where this vertex is the beginning (from point) of the edge.
		 *
		 * @param VKey key of vertex
		 * @param V vertex's data (default is void* if no data is required)
		 * @param E edge's data (usually, represents weight; default is void* if no data is required)
		 * @param VKeyEq equality comparator for vertex key's data (default is std::equal_to<VKey>)
		 * @param VEq equality comparator for vertex's data (default is std::equal_to<V>)
		 * @param EdgeEq equality comparator for edge's data (default is std::equal_to<E>)
		 * @param VKeyCmp less comparator for vertex key's data (default is std::less<VKey>)
		 * @param VCmp less comparator for vertex's data (default is std::less<V>)
		 * @param EdgeCmp less comparator for edge's data (default is std::less<E>)
		 * @param VKeyHasher hasher for vertex key (default is std::hash<VKey>)
		 */

		template <MATRIX_GRAPH_TEMPLATE_DECLARE> class matrix_graph : public dijkstra_filter_graph<MATRIX_GRAPH_TEMPLATE_USAGE> {
			struct edge;

			/**
			 * Vertex structure with value and all its connections,
			 * where this vertex is the beginning of edge
			 */

			struct vertex {
				V val = V();

				explicit vertex(const V& val) noexcept : val(val) {}
				explicit vertex(V&& val) noexcept : val(val) {}

				vertex() noexcept = default;
				~vertex() noexcept = default;
			};

			/**
			 * Edge structure with value (usually, represents weight), from and to vertices keys.
			 * Vertices themselves can be obtained from the adjacency_list by keys
			 */

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

			DistCalc distance_getter;
			std::vector<vertex> vertices;
			std::vector<std::vector<std::optional<edge>>> adjacency_matrix;

		public:
			/** Creates empty matrix graph */
			matrix_graph() noexcept = default;

			/**
			 * Creates matrix graph with given number of elements.
			 * Note that this graph will not have any connections.
			 * Complexity is O(V)
			 * @param size initial matrix graph's size
			 */

			explicit matrix_graph(const std::vector<V>& vertices) noexcept {
				adjacency_matrix = std::vector<std::vector<std::optional<edge>>>(
						vertices.size(), std::vector<std::optional<edge>>(vertices.size(), std::nullopt)
				);

				std::ranges::copy(
						vertices | std::views::transform([](const auto& v) { return vertex(v); }),
						std::back_inserter(this->vertices)
				);
			}

			/**
			 * Creates matrix graph with given number of elements.
			 * Note that this graph will not have any connections.
			 * Complexity is O(V)
			 * @param size initial matrix graph's size
			 */

			explicit matrix_graph(std::vector<V>&& vertices) noexcept {
				adjacency_matrix = std::vector<std::vector<std::optional<edge>>>(
						vertices.size(), std::vector<std::optional<edge>>(vertices.size(), std::nullopt)
				);

				std::ranges::copy(
						vertices | std::views::transform([](const auto& v) { return vertex(v); }),
						std::back_inserter(this->vertices)
				);
			}

			/**
			 * Creates matrix graph with initial capacity with factory constructor.
			 * It is recommended to use this method for better performance
			 * @param initial_capacity preferred number of vertices that will be used
			 * @return new matrix graph with given capacity
			 */

			static matrix_graph<MATRIX_GRAPH_TEMPLATE_USAGE> with_capacity(const std::size_t initial_capacity) noexcept {
				matrix_graph graph;
				graph.adjacency_matrix.reserve(initial_capacity);
				return graph;
			}

			~matrix_graph() noexcept = default;

			/**
			 * Number of vertices in the matrix graph.
			 * Complexity is O(1)
			 * @return number of vertices in the graph
			 */

			[[nodiscard]] constexpr inline std::size_t vertices_number() const noexcept override {
				return vertices.size();
			}

			/**
			 * Inserts new vertex to the end of matrix graph.
			 * Complexity is O(E).
			 * Note that this method is significantly faster than indexed version.
			 * @param value vertex's value
			 */

			inline void insert(const V& value) noexcept {
				vertices.push_back(vertex(value));

				for (auto& row : adjacency_matrix)
					row.push_back(std::nullopt);

				adjacency_matrix.push_back(
						std::vector<std::optional<edge>>(adjacency_matrix.size() + 1, std::nullopt)
				);
			}

			/**
			 * Inserts new vertex to the end of matrix graph.
			 * Complexity is O(E).
			 * Note that this method is significantly faster than indexed version.
			 * @param value vertex's value
			 */

			inline void insert(V&& value) noexcept {
				vertices.push_back(vertex(value));

				for (auto& row : adjacency_matrix)
					row.push_back(std::nullopt);

				adjacency_matrix.push_back(
						std::vector<std::optional<edge>>(adjacency_matrix.size() + 1, std::nullopt)
				);
			}

			/**
			 * Inserts new vertex to the matrix graph by index.
			 * Complexity is O(E)
			 * @param key key of vertex (index)
			 * @param value vertex's value
			 */

			inline void insert(const VKey& key, const V& value) noexcept override {
				if (key < 0 || key > vertices.size())
					throw std::range_error("Vertex key should be in range [0; graph.size]");

				vertices.insert(vertices.begin() + key, vertex(value));

				for (auto& row : adjacency_matrix)
					row.insert(row.begin() + key, std::nullopt);

				adjacency_matrix.insert(
						adjacency_matrix.begin() + key,
						std::vector<std::optional<edge>>(vertices.size() + 1, std::nullopt)
				);
			}

			/**
			 * Inserts new vertex to the matrix graph by index.
			 * Complexity is O(E)
			 * @param key key of vertex
			 * @param value vertex's value
			 */

			inline void insert(const VKey& key, V&& value) noexcept override {
				if (key < 0 || key > vertices.size())
					throw std::range_error("Vertex key should be in range [0; graph.size]");

				vertices.insert(vertices.begin() + key, vertex(value));

				for (auto& row : adjacency_matrix)
					row.insert(row.begin() + key, std::nullopt);

				adjacency_matrix.insert(
						adjacency_matrix.begin() + key,
						std::vector<std::optional<edge>>(vertices.size() + 1, std::nullopt)
				);
			}

		private:
			/**
			 * Gets vertex by its key. Complexity is O(1).
			 * If key is not present in the matrix, exception will be thrown
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline const vertex& get_vertex(const VKey& key) const noexcept {
				return vertices.at(key);
			}

			/**
			 * Gets vertex by its key. Complexity is O(1).
			 * If key is not present in the matrix, exception will be thrown
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline vertex& get_vertex(const VKey& key) noexcept {
				return vertices.at(key);
			}

		public:
			/**
			 * Gets edge by its keys. Complexity is O(1).
			 * If keys are not present in the matrix, exception will be thrown
			 *
			 * @param from key to the beginning of edge
			 * @param to key to the ending of edge
			 * @return edge's value
			 */

			[[nodiscard]] inline const E& get_edge_unchecked(const VKey& from, const VKey& to) const noexcept {
				return adjacency_matrix.at(from).at(to).value().val;
			}

			/**
			 * Gets edge by its keys. Complexity is O(1).
			 * Note that this method does not properly handles
			 * cases where keys are illegal
			 *
			 * @param from key to the beginning of edge
			 * @param to key to the ending of edge
			 * @return edge's value
			 */

			[[nodiscard]] inline E& get_edge_unchecked(const VKey& from, const VKey& to) noexcept {
				return adjacency_matrix.at(from).at(to).value().val;
			}

			/**
			 * Gets edge by its keys. Complexity is O(1).
			 * @param from key to the beginning of edge
			 * @param to key to the ending of edge
			 * @return edge's value or std::nullopt if keys are illegal
			 */

			[[nodiscard]] inline std::optional<std::reference_wrapper<E>> get_edge(const VKey& from, const VKey& to) const noexcept {
				if (from < 0 || to < 0 || from >= vertices.size() || to >= vertices.size())
					return std::nullopt;

				return adjacency_matrix.at(from).at(to).transform([](const auto& e) { return e.val; });
			}

			/**
			 * Gets edge by its keys. Complexity is O(1).
			 * @param from key to the beginning of edge
			 * @param to key to the ending of edge
			 * @return edge's value or std::nullopt if keys are illegal
			 */

			[[nodiscard]] inline E& get_edge(const VKey& from, const VKey& to) noexcept {
				if (from < 0 || to < 0 || from >= vertices.size() || to >= vertices.size())
					return std::nullopt;

				return adjacency_matrix.at(from).at(to).transform([](const auto& e) { return e.val; });
			}

			/**
			 * Gets vertex by its key. Complexity is O(1).
			 * If key is not present in the matrix, exception will be thrown
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline const V& operator[] (const VKey& key) const noexcept override {
				return get_vertex(key).val;
			}

			/**
			 * Gets vertex by its key. Complexity is O(1).
			 * If key is not present in the matrix, exception will be thrown
			 *
			 * @param key key to the vertex
			 * @return vertex's value
			 */

			[[nodiscard]] inline V& operator[] (const VKey& key) noexcept override {
				return get_vertex(key).val;
			}

			/**
			 * Checks if two vertices have connection.
			 * Complexity is O(1)
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @return true if edge between two vertices exists
			 */

			[[nodiscard]] inline bool has_connection(const VKey& from, const VKey& to) const noexcept override {
				if (from < 0 || to < 0 || from >= vertices.size() || to >= vertices.size())
					return false;

				return adjacency_matrix[from][to].has_value();
			}

			/**
			 * Connects two vertices with an edge. Complexity is O(1).
			 * Note that this method does not properly handles case
			 * where connection between vertices is already exists
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 */

			inline void connect_unchecked(const VKey& from, const VKey& to, const E& edge_val) noexcept {
				adjacency_matrix[from][to] = std::make_optional(edge(edge_val, from, to));
			}

			/**
			 * Connects two vertices with an edge. Complexity is O(1).
			 * Note that this method does not properly handles case
			 * where connection between vertices is already exists
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 */

			inline void connect_unchecked(VKey&& from, VKey&& to, E&& edge_val) noexcept {
				adjacency_matrix[from][to] = std::make_optional(edge(edge_val, from, to));
			}

			/**
			 * Connects two vertices with an edge. Complexity is O(1).
			 * In case if connection is already present, std::runtime_error is thrown.
			 * In case if keys are not in bounds [0; size), std::range_error is thrown
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 * @throws std::runtime_error if connection is already present
			 * @throws std::range_error if keys are illegal
			 */

			inline void connect(const VKey& from, const VKey& to, const E& edge_val) noexcept override {
				if (from < 0 || to < 0 || from >= vertices.size() || to >= vertices.size())
					throw std::range_error("Vertex key should be in range [0; graph.size)");

				if (has_connection(from, to))
					throw std::runtime_error("Vertices are already connected");

				connect_unchecked(from, to, edge_val);
			}

			/**
			 * Connects two vertices with an edge. Complexity is O(1).
			 * In case if connection is already present, std::runtime_error is thrown.
			 * In case if keys are not in bounds [0; size), std::range_error is thrown
			 *
			 * @param from key to beginning vertex
			 * @param to key to ending vertex
			 * @param edge_val edge's value
			 * @throws std::runtime_error if connection is already present
			 * @throws std::range_error if keys are illegal
			 */

			inline void connect(VKey&& from, VKey&& to, E&& edge_val) noexcept override {
				if (from < 0 || to < 0 || from >= vertices.size() || to >= vertices.size())
					throw std::range_error("Vertex key should be in range [0; graph.size)");

				if (has_connection(from, to))
					throw std::runtime_error("Vertices are already connected");

				connect_unchecked(from, to, edge_val);
			}

			using dij_graph = dijkstra_filter_graph<MATRIX_GRAPH_TEMPLATE_USAGE>;

			/**
			 * Dijkstra algorithm for matrix graph. Complexity is O(V^2)
			 * Searches for minimum path from given vertex to all other vertices.
			 *
			 * @param from start vertex
			 * @return pair of 1) vector of distances from given vertex to all other vertices
			 * 2) vector of references to parents (or std::nullopt if no parent was obtained)
			 */

			[[nodiscard]] inline dij_graph::distances_to_parents dijkstra(const VKey& from) const noexcept override {
				const auto size = vertices.size();
				std::vector<D> distances(size, INT32_MAX);
				std::vector<std::optional<VKey>> parents(size, std::nullopt);

				// We will use raw pointer, because std::vector<bool>
				// flags are 1 bit size and require bit operations,
				// which may be too slow comparing with 1 byte size

				auto* visited = static_cast<bool*>(std::calloc(size, sizeof(bool)));
				distances[from] = 0;

				const auto next_min_index = [&]() {
					D min = dij_graph::INF;
					VKey min_ind = UINT32_MAX;

					for (std::size_t i = 0; i < size; ++i)
						if (!visited[i] && distances[i] <= min)
							min = distances[i], min_ind = i;

					return min_ind;
				};

				const auto should_update_neighbour = [&](const VKey min_ind, const VKey i) {
					if (!adjacency_matrix[min_ind][i].has_value())
						return false;

					const auto& edge = adjacency_matrix[min_ind][i].value().val;
					const auto edge_dist = distance_getter(edge);
					const auto new_sum = distances[min_ind] + edge_dist;

					return !visited[i] && distances[min_ind] != dij_graph::INF && new_sum < distances[i];
				};

				for (std::size_t q = 0; q < size - 1; ++q) {
					const auto min_ind = next_min_index();

					if (min_ind == UINT32_MAX)
						break;

					visited[min_ind] = true;

					for (std::size_t i = 0; i < size; ++i) {
						if (should_update_neighbour(min_ind, i)) {
							parents[i] = std::make_optional(min_ind);
							distances[i] = distances[min_ind] + distance_getter(adjacency_matrix[min_ind][i].value().val);
						}
					}
				}

				std::free(visited);
				return std::make_pair(distances, parents);
			}

			/**
			 * Dijkstra algorithm for matrix graph with additional filter for edges.
			 * Searches for minimum path from given vertex to all other vertices.
			 * Complexity is O(V^2)
			 *
			 * @param from start vertex
			 * @return pair of 1) vector of distances from given vertex to all other vertices
			 * 2) vector of references to parents (or std::nullopt if no parent was obtained)
			 */

			[[nodiscard]] inline dij_graph::distances_to_parents dijkstra(const VKey& from, EdgeFilter filter) const noexcept override {
				const auto size = vertices.size();
				std::vector<D> distances(size, INT32_MAX);
				std::vector<std::optional<VKey>> parents(size, std::nullopt);

				// We will use raw pointer, because std::vector<bool>
				// flags are 1 bit size and require bit operations,
				// which may be too slow comparing with 1 byte size

				auto* visited = static_cast<bool*>(std::calloc(size, sizeof(bool)));
				distances[from] = 0;

				const auto next_min_index = [&]() {
					D min = dij_graph::INF;
					VKey min_ind = UINT32_MAX;

					for (std::size_t i = 0; i < size; ++i)
						if (!visited[i] && distances[i] <= min)
							min = distances[i], min_ind = i;

					return min_ind;
				};

				const auto should_update_neighbour = [&](const VKey min_ind, const VKey i) {
					if (!adjacency_matrix[min_ind][i].has_value())
						return false;

					const auto& edge = adjacency_matrix[min_ind][i].value().val;
					const auto edge_dist = distance_getter(edge);
					const auto new_sum = distances[min_ind] + edge_dist;

					return !visited[i]
						&& distances[min_ind] != dij_graph::INF
						&& new_sum < distances[i]
						&& filter(edge);
				};

				for (std::size_t q = 0; q < size - 1; ++q) {
					const auto min_ind = next_min_index();

					if (min_ind == UINT32_MAX)
						break;

					visited[min_ind] = true;

					for (std::size_t i = 0; i < size; ++i) {
						if (should_update_neighbour(min_ind, i)) {
							parents[i] = std::make_optional(min_ind);
							distances[i] = distances[min_ind] + distance_getter(adjacency_matrix[min_ind][i].value().val);
						}
					}
				}

				std::free(visited);
				return std::make_pair(distances, parents);
			}
		};
	}

	const int NO_EDGE = 100000;

	struct network_edge {
		int length = 0;
		int bandwidth = 0;

		network_edge() noexcept = default;
		~network_edge() noexcept = default;

		network_edge(const int length, const int bandwidth) noexcept :
			length(length), bandwidth(bandwidth) {}
	};

	struct network_edge_distance {
		[[nodiscard]] constexpr inline int operator() (const network_edge& edge) const noexcept {
			return edge.length;
		}
	};

	class network_edge_filter {
		int min_bandwidth;

	public:
		explicit network_edge_filter(const int min_bandwidth) noexcept : min_bandwidth(min_bandwidth) {}
		~network_edge_filter() noexcept = default;

		[[nodiscard]] inline bool operator() (const network_edge edge) const noexcept {
			return edge.bandwidth >= min_bandwidth;
		}
	};
}

int main() {
	int n = 0, m = 0;
	std::scanf("%d%d", &n, &m);

	// Holly graph...
	dsa_hw::graph::matrix_graph<std::size_t, void*, dsa_hw::network_edge, int, dsa_hw::network_edge_distance, dsa_hw::network_edge_filter> graph(
			std::vector<void*>(n, nullptr)
	);

	while (m--) {
		std::size_t from = 0, to = 0;
		int len = 0, bw = 0;
		std::scanf("%zu%zu%d%d", &from, &to, &len, &bw); --from, --to;
		graph.connect_unchecked(from, to, dsa_hw::network_edge(len, bw));
	}

	std::size_t start = 0, finish = 0;
	int min_bandwidth = 0;
	std::scanf("%zu%zu%d", &start, &finish, &min_bandwidth); --start, --finish;
	
	// Calculating distances to other vertices

	auto&& [distances, parents] = graph.dijkstra(start, dsa_hw::network_edge_filter(min_bandwidth));

	// Checking if path was found

	if (distances[finish] == decltype(graph)::INF) {
		std::puts("IMPOSSIBLE");
		return 0;
	}

	auto&& path = decltype(graph)::path(finish, parents);

	if (path.size() < 2) {
		std::puts("IMPOSSIBLE");
		return 0;
	}

	// Calculating total path's len and minimum bandwidth
	
	auto&& [len, bw] = std::accumulate(
			path.begin() + 1,
			path.end(),
			std::make_pair(0, INT32_MAX),
			[&graph, &path, ind = 0](const auto& acc, const auto& key) mutable {
				const auto& edge = graph.get_edge_unchecked(path[ind++], key);
				return std::make_pair(acc.first + edge.length, std::min(acc.second, edge.bandwidth));
			}
	);

	std::printf("%zu %d %d\n", path.size(), len, bw);

	for (auto&& i : path)
		std::printf("%zu ", i + 1);

	return 0;
}
