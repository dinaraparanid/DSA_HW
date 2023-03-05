// Arseny Savсhenko

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

namespace dsa_hw {
	// Yeah, I know, it looks weird, but using just FAILURE
	// without context also looks weird...

	namespace line_separating_state {
		enum line_separating_state {
			UNINIT,
			SUCCESS,
			FAILURE
		};
	}

	// Whole idea in few words:
	// We store states that indicate if it is ok to put space here
	// If we walked through all string, but did not separate last part,
	// separation was not valid, and we have to move further

	namespace {
		line_separating_state::line_separating_state solve_line_separating(
				const std::unordered_set<std::string>& dictionary,
				const std::string& s,
				std::vector<line_separating_state::line_separating_state>& memo,
				std::int32_t separator = 0
		) noexcept {
			// Just to be sure
			if (separator > s.length())
				return line_separating_state::FAILURE;

			if (separator == s.length())
				return line_separating_state::SUCCESS;

			if (memo[separator] != line_separating_state::UNINIT)
				return memo[separator];

			std::string separated_part;

			while (separator < s.length()) {
				separated_part.push_back(s[separator++]);

				if (dictionary.find(separated_part) != dictionary.end()) {
					const auto res = solve_line_separating(dictionary, s, memo, separator);
					memo[separator] = res;

					if (res == line_separating_state::SUCCESS)
						return memo[separator];
				}
			}

			return line_separating_state::FAILURE;
		}
	}

	std::vector<line_separating_state::line_separating_state> solve_line_separating(const std::unordered_set<std::string>& dictionary, const std::string& s) noexcept {
		std::vector<line_separating_state::line_separating_state> memo(s.length() + 1, line_separating_state::UNINIT);
		solve_line_separating(dictionary, s, memo);
		return memo;
	}
}

int main() {
	std::ios_base::sync_with_stdio(false);

	int n = 0, k = 0;
	std::cin >> n >> k;

	std::unordered_set<std::string> dictionary;
	std::string word;

	for (int i = 0; i < n; ++i) {
		std::cin >> word;
		dictionary.insert(word);
	}

	std::string s;
	std::getline(std::cin >> std::ws, s);

	const auto& separators = dsa_hw::solve_line_separating(dictionary, s);

	std::putchar(s[0]);

	for (int i = 1; i < s.length(); ++i) {
		if (separators[i] == dsa_hw::line_separating_state::SUCCESS)
			std::putchar(' ');

		std::putchar(s[i]);
	}

	return 0;
}
