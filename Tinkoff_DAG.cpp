#include <cstdio>
#include <cstdint>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <queue>

namespace topological_sort {
	namespace {
		void topological_sort(
				const int cur_vertex,
				const std::vector<std::unordered_set<int>>& adj_list,
				std::vector<int>& top_sorted,
				std::unordered_set<int>& unvisited
		) noexcept {
			unvisited.erase(cur_vertex);

			for (const auto& child : adj_list[cur_vertex])
				if (unvisited.find(child) != unvisited.end())
					topological_sort(child, adj_list, top_sorted, unvisited);

			top_sorted.push_back(cur_vertex);
		}
	}

	inline auto partially_sorted(const std::vector<std::unordered_set<int>>& adj_list, const int start = 0) noexcept {
		std::vector<int> top_sorted;

		std::unordered_set<int> unvisited;
		for (int i = 0; i < adj_list.size(); ++i) unvisited.insert(i);

		topological_sort(start, adj_list, top_sorted, unvisited);
		return top_sorted;
	}

	inline auto sorted(const std::vector<std::unordered_set<int>>& adj_list) noexcept {
		std::vector<int> top_sorted;

		std::unordered_set<int> unvisited;
		for (int i = 0; i < adj_list.size(); ++i) unvisited.insert(i);

		while (!unvisited.empty())
			topological_sort(*unvisited.begin(), adj_list, top_sorted, unvisited);

		std::reverse(top_sorted.begin(), top_sorted.end());
		return top_sorted;
	}
}

namespace condensation {
	namespace {
		void unite(
				const int cur_vertex,
				const int cur_strong_comp,
				const std::vector<std::unordered_set<int>>& adj_list,
				std::vector<int>& vertices_to_strong_comp,
				std::vector<std::unordered_set<int>>& strong_adj_list
		) noexcept {
			vertices_to_strong_comp[cur_vertex] = cur_strong_comp;

			for (const auto& v : adj_list[cur_vertex]) {
				if (vertices_to_strong_comp[v] == -1)
					unite(v, cur_strong_comp, adj_list, vertices_to_strong_comp, strong_adj_list);
				else if (vertices_to_strong_comp[v] != cur_strong_comp)
					strong_adj_list[vertices_to_strong_comp[v]].insert(cur_strong_comp);
			}
		}
	}

	inline auto unite(
			const std::vector<std::unordered_set<int>>& adj_list,
			const std::vector<int>& top_sorted
	) noexcept {
		int cur_strong_comp = 0;
		std::vector<int> vertices_to_strong_comp(adj_list.size(), -1);
		std::vector<std::unordered_set<int>> strong_adj_list(adj_list.size());

		for (const auto& v : top_sorted)
			if (vertices_to_strong_comp[v] == -1)
				unite(v, cur_strong_comp++, adj_list, vertices_to_strong_comp, strong_adj_list);

		strong_adj_list.resize(cur_strong_comp);
		return std::make_tuple(strong_adj_list, vertices_to_strong_comp, cur_strong_comp);
	}
}

auto dfs(
		const int cur_vertex,
		const std::vector<std::unordered_set<int>>& adj_list,
		const std::vector<int>& costs,
		std::vector<std::pair<std::uint64_t, std::size_t>>& dp,
		std::vector<int>& created_details
) noexcept {
	if (dp[cur_vertex].first != UINT64_MAX) {
		created_details.push_back(cur_vertex);
		return dp[cur_vertex];
	}

	dp[cur_vertex] = std::accumulate(
			adj_list[cur_vertex].begin(),
			adj_list[cur_vertex].end(),
			std::make_pair(costs[cur_vertex], 1),
			[&](const auto& acc, const auto& v) {
				auto&& [sum, details] = dfs(v, adj_list, costs, dp, created_details);
				return std::make_pair(acc.first + sum, acc.second + details);
			}
	);

	created_details.push_back(cur_vertex);
	return dp[cur_vertex];
}

inline auto bfs(
		const std::vector<std::unordered_set<int>>& adj_list,
		const std::vector<int>& costs
) noexcept {
	std::uint64_t sum = 0;
	std::size_t details = 0;

	std::queue<int> q; q.push(0);
	std::unordered_set<int> created_details;

	while (!q.empty()) {
		const auto cur_detail = q.front(); q.pop();

		if (created_details.find(cur_detail) != created_details.end())
			continue;

		sum += costs[cur_detail];
		++details;

		created_details.insert(cur_detail);

		for (const auto& v : adj_list[cur_detail])
			if (created_details.find(v) == created_details.end())
				q.push(v);
	}

	return std::make_pair(sum, details);
}

int main() {
	int n = 0, m = 0;
	std::scanf("%d%d", &n, &m);

	std::vector<std::unordered_set<int>> adj_list(n);
	std::vector<std::unordered_set<int>> reversed_adj_list(n);

	while (m--) {
		int a = 0, b = 0;
		std::scanf("%d%d", &a, &b); --a, --b;
		adj_list[a].insert(b);
		reversed_adj_list[b].insert(a);
	}

	auto&& [strong_adj_list, vertices_to_strong_comp, strong_components] = condensation::unite(
			reversed_adj_list, topological_sort::sorted(adj_list)
	);

	std::printf("%d\n", strong_components);

	for (const auto& comp : vertices_to_strong_comp)
		std::printf("%d ", comp + 1);

	return 0;
}
