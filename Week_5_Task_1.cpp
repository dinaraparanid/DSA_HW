// Arseny Savchenko

#include <cstdio>
#include <cstdint>
#include <vector>

namespace dsa_hw {
	struct item {
		std::int32_t weight = 0;
		std::int32_t cost = 0;
	};

	namespace {
		const std::int32_t UNINIT = INT32_MIN;

		std::int32_t calc_knapsack(
				const std::int32_t items_number,
				const std::int32_t cur_weight,
				const std::vector<item>& items,
				std::vector<std::vector<std::int32_t>>& memo
		) noexcept {
			if (cur_weight <= 0 || items_number <= 0)
				return 0;

			if (memo[items_number][cur_weight] != UNINIT)
				return memo[items_number][cur_weight];

			if (cur_weight < items[items_number - 1].weight) {
				// Item cannot be in bag
				memo[items_number][cur_weight] = calc_knapsack(items_number - 1, cur_weight, items, memo);
			} else {
				// Item can be both in bag and not in bag
				memo[items_number][cur_weight] = std::max(
						calc_knapsack(items_number - 1, cur_weight, items, memo),
						calc_knapsack(items_number - 1, cur_weight - items[items_number - 1].weight, items, memo) + items[items_number - 1].cost
				);
			}

			return memo[items_number][cur_weight];
		}

		void get_knapsack(
				const std::int32_t items_number,
				const std::int32_t cur_weight,
				const std::vector<item>& items,
				const std::vector<std::vector<std::int32_t>>& memo,
				std::vector<std::uint32_t>& indices
		) noexcept {
			if (memo[items_number][cur_weight] == 0 || memo[items_number][cur_weight] == UNINIT)
				return;

			if (memo[items_number - 1][cur_weight] == memo[items_number][cur_weight]) {
				// This item does not affect result => not present, check previous one
				get_knapsack(items_number - 1, cur_weight, items, memo, indices);
			} else {
				// Item is present, check previous one without this item
				get_knapsack(items_number - 1, cur_weight - items[items_number - 1].weight, items, memo, indices);
				indices.push_back(items_number);
			}
		}
	}

	inline std::vector<std::uint32_t> solve_knapsack(const std::int32_t max_weight, const std::vector<item>& items) noexcept {
		std::vector<std::vector<std::int32_t>> memo(
				items.size() + 1, std::vector<std::int32_t>(max_weight + 1, UNINIT)
		);

		for (int i = 0; i <= items.size(); ++i)
			memo[i][0] = 0;

		for (int i = 0; i <= max_weight; ++i)
			memo[0][i] = 0;

		calc_knapsack(items.size(), max_weight, items, memo);
		std::vector<uint32_t> indices;
		get_knapsack(items.size(), max_weight, items, memo, indices);
		return indices;
	}
}

int main() {
	std::int32_t n = 0, max_weight = 0;
	std::scanf("%u%u", &n, &max_weight);

	std::vector<dsa_hw::item> items(n);

	for (auto& [weight, _] : items)
		std::scanf("%u", &weight);

	for (auto& [_, cost] : items)
		std::scanf("%u", &cost);

	const auto& indices = dsa_hw::solve_knapsack(max_weight, items);

	std::printf("%zu\n", indices.size());

	for (const auto index : indices)
		std::printf("%u ", index);

	return 0;
}
