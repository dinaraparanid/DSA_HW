// Arseny Savchenko

#include <cstdio>
#include <cstdlib>
#include <cstdint>

namespace {
    template <typename T, typename Cmp> auto merge_sort(
            T* const arr,
            const std::size_t left_border,
            const std::size_t right_border,
            const Cmp cmp
    ) {
        if (left_border == right_border)
            return;

        const auto mid = (left_border + right_border) >> 1;

        merge_sort(arr, left_border, mid, cmp);
        merge_sort(arr, mid + 1, right_border, cmp);

        auto* const sort_holder = reinterpret_cast<T* const>(std::malloc(right_border * sizeof(T) + 1));

        const auto holder_len = right_border - left_border + 1;
        const void* const out_of_holder_ptr = sort_holder + holder_len;
        auto i = left_border, q = mid + 1;

        for (auto* holder_elem = sort_holder; holder_elem != out_of_holder_ptr; ++holder_elem) {
            const auto i_elem = arr[i];
            const auto q_elem = arr[q];

            if (q > right_border || i <= mid && cmp(i_elem, q_elem)) {
                *holder_elem = i_elem;
                i++;
            } else {
                *holder_elem = q_elem;
                q++;
            }
        }

        for (int step = 0; step < right_border - left_border + 1; step++)
            arr[left_border + step] = sort_holder[step];

        std::free(sort_holder);
    }
}

template <typename T, typename Cmp> auto merge_sort(
        T* const arr,
        const std::size_t size,
        const Cmp cmp
) { merge_sort(arr, 0, size - 1, cmp); }

int main() {
    int n = 0;
    std::scanf("%d", &n);

    int* const arr = reinterpret_cast<int* const>(std::malloc(n * sizeof(int)));

    for (int* i = arr; i != arr + n; ++i)
        std::scanf("%d", i);

    merge_sort(arr, n, [](const int a, const int b) { return a < b; });

    for (int* i = arr; i != arr + n; ++i)
        std::printf("%d ", *i);

    std::free(arr);
    return 0;
}
