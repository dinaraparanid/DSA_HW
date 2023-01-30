// Arseny Savchenko

#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>
#include <algorithm>

namespace {
    template <typename T, typename Cmp> auto merge_sort(
            T* const arr,
            const std::size_t left_border,
            const std::size_t right_border,
            const Cmp cmp
    ) {
        if (left_border + 1 >= right_border)
            return;

        const auto mid = (left_border + right_border) >> 1;

        merge_sort(arr, left_border, mid, cmp);
        merge_sort(arr, mid, right_border, cmp);

        int i = 0, q = 0;
        auto* const sort_holder = new T[right_border - left_border];

        while (left_border + i < mid && mid + q < right_border) {
            if (cmp(arr[left_border + i], arr[mid + q])) {
                sort_holder[i + q] = arr[left_border + i];
                ++i;
            } else {
                sort_holder[i + q] = arr[mid + q];
                ++q;
            }
        }

        while (left_border + i < mid) {
            sort_holder[i + q] = arr[left_border + i];
            ++i;
        }

        while (mid + q < right_border) {
            sort_holder[i + q] = arr[mid + q];
            ++q;
        }

        for (int step = 0; step < i + q; ++step)
            arr[left_border + step] = sort_holder[step];

        delete[] sort_holder;
    }
}

template <typename T, typename Cmp> auto merge_sort(
        T* const arr,
        const std::size_t size,
        const Cmp cmp
) { merge_sort(arr, 0, size, cmp); }

class player {
    std::string name;
    std::uint64_t score;

public:
    player() {}
    ~player() {}

    player(const std::string& name, const std::uint64_t score) : name(name), score(score) {}
    player(std::string&& name, const std::uint64_t score) : name(std::move(name)), score(score) {}

    const std::string& get_name() const { return name; }
    std::uint64_t get_score() const { return score; }
};

int main() {
    std::ios_base::sync_with_stdio(false);

    int n = 0, k = 0;
    std::cin >> n >> k;

    auto* const players = new player[n];

    for (auto* p = players; p != players + n; ++p) {
        std::string input;
        std::getline(std::cin >> std::ws, input);
        std::stringstream s;
        s << input;

        std::string name; std::uint64_t score;
        s >> name >> score;
        *p = player(std::move(name), score);
    }

    merge_sort(players, n, [](const auto& a, const auto& b) { return a.get_score() >= b.get_score(); });

    const auto bound = std::min(n, k);

    for (auto* i = players; i != players + bound; ++i)
        std::cout << i->get_name() << ' ' << i->get_score() << std::endl;

    delete[] players;
    return 0;
}
