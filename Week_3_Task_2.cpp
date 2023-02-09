// Arseny Savchenko

#include <iostream>
#include <string>
#include <utility>
#include <functional>
#include <cstring>
#include <numeric>
#include <optional>
#include <list>
#include <vector>

namespace dsa_hw {
    template <typename K, typename V> struct map {
        virtual void insert(const std::pair<K, V> entry) = 0;
        virtual void insert(std::pair<K, V>&& entry) = 0;

        virtual void insert(const K key, const V value) = 0;
        virtual void insert(K&& key, V&& value) = 0;

        virtual std::optional<std::pair<K, V>>& get_if_present(const K& key) = 0;
        virtual std::pair<K, V>& get(const K& key) = 0;
        inline V& operator[] (const K& key) { return get(key).second; }

        virtual bool remove(const K& key) = 0;

        virtual std::size_t get_size() const = 0;
        inline bool is_empty() const { return get_size() == 0; }
    };

    template <typename T> struct set {
        virtual void insert(const T value) = 0;
        virtual void insert(T&& value) = 0;

        virtual bool remove(const T& value) = 0;

        virtual std::size_t get_size() const = 0;
        inline bool is_empty() const { return get_size() == 0; }
    };

    template <typename K, typename V> class hash_map : public map<K, V> {
        std::size_t size;
        std::size_t elems = 0;
        std::optional<std::pair<K, V>>* data;
        std::function<std::size_t(const K&)> hash_algo;

        inline void resize() {
            size <<= 1;

            data = reinterpret_cast<std::optional<std::pair<K, V>>*>(
                    std::realloc(data, size * sizeof(std::optional<std::pair<K, V>>))
            );
        }

        inline void resize_if_not_enough_space() {
            if (elems == size)
                resize();
        }

    public:
        template <typename T> friend class hash_set;

        class iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::pair<K, V>;
            using pointer = std::optional<std::pair<K, V>>*;
            using reference = value_type&;

        private:
            pointer ptr;
            const pointer begin_ptr;
            const pointer end_ptr;

        public:
            friend class hash_map<K, V>;

            iterator(pointer ptr, const pointer begin_ptr, const pointer end_ptr) :
                    ptr(ptr), begin_ptr(begin_ptr), end_ptr(end_ptr) {}

            ~iterator() = default;

            inline reference operator*() const { return ptr->value(); }
            inline pointer operator->() const { return ptr; }
            inline bool operator!() const { return ptr == nullptr || !ptr->has_value(); }

            inline bool operator==(const iterator& other) const { return ptr == other.ptr; };
            inline bool operator!=(const iterator& other) const { return ptr != other.ptr; };

            inline iterator& operator=(const iterator other) {
                ptr = other.ptr;
                return *this;
            }

            inline iterator operator++() {
                for (auto* self = ptr + 1; self != end_ptr; ++self) {
                    if (self->has_value()) {
                        ptr = self;
                        return *this;
                    }
                }

                ptr = end_ptr;
                return *this;
            }

            inline iterator operator--() {
                for (auto* self = ptr - 1; self != begin_ptr; --self) {
                    if (self->has_value()) {
                        ptr = self;
                        return *this;
                    }
                }

                ptr = begin_ptr;
                return *this;
            }
        };

        hash_map(const std::size_t initial_capacity, std::function<std::size_t(const K&)> hash_algo) {
            size = initial_capacity > 0 ? initial_capacity : 100000;

            data = reinterpret_cast<std::optional<std::pair<K, V>>*>(
                    std::calloc(size, sizeof(std::optional<std::pair<K, V>>))
            );

            for (auto* opt = data; opt != data + size; ++opt)
                *opt = std::nullopt;

            this->hash_algo = hash_algo;
        }

        hash_map(std::function<std::size_t(const K&)> hash_algo) {
            size = 100000;

            data = reinterpret_cast<std::optional<std::pair<K, V>>*>(
                    std::calloc(size, sizeof(std::optional<std::pair<K, V>>))
            );

            for (auto* opt = data; opt != data + size; ++opt)
                *opt = std::nullopt;

            this->hash_algo = hash_algo;
        }

        ~hash_map() { std::free(data); }

        inline void insert(const std::pair<K, V> entry) override {
            resize_if_not_enough_space();
            const auto [key, value] = entry;
            const auto hashed_ind = hash_algo(key) % size;

            for (std::size_t i = 0; i < size; ++i) {
                const auto index = (hashed_ind + i) % size;

                if (data[index].has_value()) {
                    auto map_entry = data[index].value();

                    if (map_entry.first == key) {
                        map_entry.second = value;
                        return;
                    }
                } else {
                    data[index] = std::make_optional(std::make_pair(key, value));
                    ++elems;
                    return;
                }
            }
        }

        inline void insert(std::pair<K, V>&& entry) override {
            resize_if_not_enough_space();
            const auto [key, value] = entry;
            const auto hashed_ind = hash_algo(key) % size;

            for (std::size_t i = 0; i < size; ++i) {
                const auto index = (hashed_ind + i) % size;

                if (data[index].has_value()) {
                    auto map_entry = data[index].value();

                    if (map_entry.first == key) {
                        map_entry.second = value;
                        return;
                    }
                } else {
                    data[index] = std::make_optional(std::make_pair(key, value));
                    ++elems;
                    return;
                }
            }
        }

        inline void insert(const K key, const V value) override {
            resize_if_not_enough_space();
            const auto hashed_ind = hash_algo(key) % size;

            for (std::size_t i = 0; i < size; ++i) {
                const auto index = (hashed_ind + i) % size;

                if (data[index].has_value()) {
                    auto entry = data[index].value();

                    if (entry.first == key) {
                        entry.second = value;
                        return;
                    }
                } else {
                    data[index] = std::make_optional(std::make_pair(key, value));
                    ++elems;
                    return;
                }
            }
        }

        inline void insert(K&& key, V&& value) override {
            resize_if_not_enough_space();
            const auto hashed_ind = hash_algo(key) % size;

            for (std::size_t i = 0; i < size; ++i) {
                const auto index = (hashed_ind + i) % size;

                if (data[index].has_value()) {
                    auto entry = data[index].value();

                    if (entry.first == key) {
                        entry.second = value;
                        return;
                    }
                } else {
                    data[index] = std::make_optional(std::make_pair(key, value));
                    ++elems;
                    return;
                }
            }
        }

        inline iterator find(const K& key) {
            const auto hashed_ind = hash_algo(key) % size;

            for (std::size_t i = 0; i < size; ++i) {
                const auto index = (hashed_ind + i) % size;

                if (data[index].has_value()) {
                    auto entry = data[index].value();

                    if (entry.first == key)
                        return iterator(data + index, data, data + size);
                }
            }

            return end();
        }

        inline std::optional<std::pair<K, V>>& get_if_present(const K& key) override {
            return *find(key).ptr;
        }

        inline std::pair<K, V>& get(const K& key) override {
            const auto hashed_ind = hash_algo(key) % size;
            auto empty_index = -1;
            auto elem_it = end();

            for (std::size_t i = 0; i < size; ++i) {
                const auto index = (hashed_ind + i) % size;

                if (data[index].has_value()) {
                    auto entry = data[index].value();

                    if (entry.first == key)
                        elem_it = iterator(data + index, data, data + size);
                } else {
                    if (empty_index == -1)
                        empty_index = index;
                }
            }

            if (empty_index == -1) {
                resize();

                for (std::size_t i = 0; i < size; ++i) {
                    const auto index = (hashed_ind + i) % size;

                    if (data[index].has_value()) {
                        auto entry = data[hashed_ind].value();

                        if (entry.first == key)
                            elem_it = iterator(data + index, data, data + size);
                    } else {
                        if (empty_index == -1)
                            empty_index = index;
                    }
                }
            }

            if (elem_it == end())
                elem_it = iterator(data + empty_index, data, data + size);

            if (!elem_it.ptr->has_value())
                *elem_it.ptr = std::make_optional(std::make_pair(key, V()));

            return elem_it.ptr->value();
        }

        inline bool remove(const K& key) override {
            const auto iter = find(key);
            const auto has_value = iter->has_value();
            remove(iter);
            return has_value;
        }

        inline void remove(const iterator it) {
            *it.ptr = std::nullopt;
            --elems;
        }

        inline iterator begin() const {
            const auto begin_ptr = data;
            const auto end_ptr = begin_ptr + size;

            for (auto* ptr = begin_ptr; ptr != end_ptr; ++ptr)
                if (ptr->has_value())
                    return iterator(ptr, begin_ptr, end_ptr);

            return iterator(data + size, begin_ptr, end_ptr);
        }

        inline iterator end() const { return iterator(data + size, data, data + size); }

        inline std::size_t get_size() const override { return elems; }
    };

    template <typename T> class hash_set : public set<T> {
        std::size_t number_of_buckets;
        std::size_t elems = 0;
        std::vector<std::list<T>> buckets;
        std::function<std::size_t(const T&)> hash_algo;

        class const_bucket_iterator;

        class bucket_iterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::list<T>;
            using pointer = std::list<T>*;
            using reference = value_type&;

        private:
            std::size_t index;
            std::vector<std::list<T>>::iterator begin;

        public:
            friend class hash_set<T>;

            bucket_iterator(const std::size_t index, std::vector<std::list<T>>::iterator begin) :
                    index(index), begin(begin) {}

            ~bucket_iterator() = default;

            inline reference operator*() const { return *(begin + index); }
            inline pointer operator->() const { return &*(begin + index); }

            inline bool operator==(const bucket_iterator& other) const { return index == other.index; };
            inline bool operator!=(const bucket_iterator& other) const { return index != other.index; };

            inline bool operator==(const const_bucket_iterator& other) const { return index == other.index; };
            inline bool operator!=(const const_bucket_iterator& other) const { return index != other.index; };

            inline bucket_iterator operator++() { ++index; return *this; }
            inline bucket_iterator operator--() { --index; return *this; }
            inline bucket_iterator operator+(const std::size_t move) { return bucket_iterator(index + move, begin); }
            inline bucket_iterator operator-(const std::size_t move) { return bucket_iterator(index - move, begin); }
        };

        class const_bucket_iterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::list<T>;
            using pointer = const std::list<T>*;
            using reference = const value_type&;

        private:
            std::size_t index;
            std::vector<std::list<T>>::const_iterator begin;

        public:
            friend class hash_set<T>;

            const_bucket_iterator(const std::size_t index, const std::vector<std::list<T>>::const_iterator begin) :
                    index(index), begin(begin) {}

            ~const_bucket_iterator() = default;

            inline reference operator*() const { return *(begin + index); }
            inline pointer operator->() const { return &*(begin + index); }

            inline bool operator==(const bucket_iterator& other) const { return index == other.index; };
            inline bool operator!=(const bucket_iterator& other) const { return index != other.index; };

            inline bool operator==(const const_bucket_iterator& other) const { return index == other.index; };
            inline bool operator!=(const const_bucket_iterator& other) const { return index != other.index; };

            inline const_bucket_iterator operator++() { ++index; return *this; }
            inline const_bucket_iterator operator--() { --index; return *this; }
            inline const_bucket_iterator operator+(const std::size_t move) { return const_bucket_iterator(index + move, begin); }
            inline const_bucket_iterator operator-(const std::size_t move) { return const_bucket_iterator(index - move, begin); }
        };

    public:
        friend class iterator;

        class const_iterator;

        class iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = value_type*;
            using reference = value_type&;

        private:
            std::list<T>::iterator it;
            bucket_iterator bucket_it;
            const bucket_iterator first_bucket_it;
            const bucket_iterator end_buckets_it;

        public:
            friend class hash_set<T>;

            iterator(
                    const std::list<T>::iterator it,
                    const bucket_iterator bucket_it,
                    const bucket_iterator first_bucket_it,
                    const bucket_iterator end_buckets_it
            ) : it(it), bucket_it(bucket_it), first_bucket_it(first_bucket_it), end_buckets_it(end_buckets_it) {};

            ~iterator() = default;

            inline reference operator*() const { return *it; }
            inline pointer operator->() const { return &*it; }
            inline bool operator!() const { return it == nullptr || !it->has_value(); }

            inline bool operator==(const iterator& other) const { return it == other.it; };
            inline bool operator!=(const iterator& other) const { return it != other.it; };

            inline bool operator==(const const_iterator& other) const { return it == other.it; };
            inline bool operator!=(const const_iterator& other) const { return it != other.it; };

            inline iterator operator++() {
                if (std::next(it) == bucket_it->end()) {
                    while (bucket_it + 1 != end_buckets_it && (bucket_it + 1)->empty())
                        ++bucket_it;

                    it = (bucket_it + 1 == end_buckets_it) ? bucket_it->end() : (++bucket_it)->begin();
                } else { ++it; }

                return *this;
            }

            inline iterator operator--() {
                if (it == bucket_it->begin()) {
                    while (bucket_it != first_bucket_it && (bucket_it - 1)->empty())
                        --bucket_it;

                    if (bucket_it != first_bucket_it) {
                        it = std::prev((--bucket_it)->end());
                    } else if (it != first_bucket_it->begin()) {
                        it = std::prev(bucket_it->end());
                    }
                } else { --it; }

                return *this;
            }
        };

        class const_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = const value_type*;
            using reference = const value_type&;

        private:
            std::list<T>::const_iterator it;
            const_bucket_iterator bucket_it;
            const const_bucket_iterator first_bucket_it;
            const const_bucket_iterator end_buckets_it;

        public:
            friend class hash_set<T>;

            const_iterator(
                    const std::list<T>::const_iterator it,
                    const const_bucket_iterator bucket_it,
                    const const_bucket_iterator first_bucket_it,
                    const const_bucket_iterator end_buckets_it
            ) : it(it), bucket_it(bucket_it), first_bucket_it(first_bucket_it), end_buckets_it(end_buckets_it) {};

            ~const_iterator() = default;

            inline reference operator*() const { return *it; }
            inline pointer operator->() const { return &*it; }
            inline bool operator!() const { return it == nullptr || !it->has_value(); }

            inline bool operator==(const iterator& other) const { return it == other.it; };
            inline bool operator!=(const iterator& other) const { return it != other.it; };

            inline bool operator==(const const_iterator& other) const { return it == other.it; };
            inline bool operator!=(const const_iterator& other) const { return it != other.it; };

            inline const_iterator operator++() {
                if (std::next(it) == bucket_it->end()) {
                    while (bucket_it + 1 != end_buckets_it && (bucket_it + 1)->empty())
                        ++bucket_it;

                    it = (bucket_it + 1 == end_buckets_it) ? bucket_it->end() : (++bucket_it)->begin();
                } else { ++it; }

                return *this;
            }

            inline const_iterator operator--() {
                if (it == bucket_it->begin()) {
                    while (bucket_it != first_bucket_it && (bucket_it - 1)->empty())
                        --bucket_it;

                    if (bucket_it != first_bucket_it) {
                        it = std::prev((--bucket_it)->end());
                    } else if (it != first_bucket_it->begin()) {
                        it = std::prev(bucket_it->end());
                    }
                } else { --it; }

                return *this;
            }
        };

        hash_set(const std::size_t number_of_buckets, const std::function<std::size_t(const T&)> hash_algo) {
            this->number_of_buckets = number_of_buckets > 0 ? number_of_buckets : 100000;
            buckets = std::vector<std::list<T>>(number_of_buckets);
            this->hash_algo = hash_algo;
        }

        hash_set(const std::function<std::size_t(const T&)> hash_algo) {
            number_of_buckets = 100000;
            buckets = std::vector<std::list<T>>(number_of_buckets);
            this->hash_algo = hash_algo;
        }

        ~hash_set() = default;

        inline void insert(const T value) override {
            const auto hashed_ind = hash_algo(value) % number_of_buckets;
            const auto bucket = buckets[hashed_ind];

            if (std::find(bucket.begin(), bucket.end(), value) == bucket.end()) {
                buckets[hashed_ind].push_back(value);
                ++elems;
            }
        }

        inline void insert(T&& value) override {
            const auto hashed_ind = hash_algo(value) % number_of_buckets;
            const auto bucket = buckets[hashed_ind];

            if (std::find(bucket.begin(), bucket.end(), value) == bucket.end()) {
                buckets[hashed_ind].push_back(value);
                ++elems;
            }
        }

        inline iterator find(const T& value) {
            const auto hashed_ind = hash_algo(value) % number_of_buckets;
            auto bucket = buckets[hashed_ind];
            const auto it = std::find(bucket.begin(), bucket.end(), value);

            return it == bucket.end() ? end() :
                iterator(
                        it,
                        bucket_iterator(hashed_ind, buckets.begin()),
                        bucket_iterator(0, buckets.begin()),
                        bucket_iterator(buckets.size(), buckets.end())
                );
        }

        inline const_iterator find(const T& value) const {
            const auto hashed_ind = hash_algo(value) % number_of_buckets;
            const auto bucket = buckets[hashed_ind];
            const auto it = std::find(bucket.begin(), bucket.end(), value);

            return it == bucket.end() ? end() :
                const_iterator(
                        it,
                        const_bucket_iterator(hashed_ind, buckets.begin()),
                        const_bucket_iterator(0, buckets.begin()),
                        const_bucket_iterator(buckets.size(), buckets.end())
                );
        }

        inline bool remove(const T& value) override {
            const auto iter = find(value);

            if (iter == end())
                return false;

            remove(iter);
            return true;
        }

        inline void remove(const iterator iter) {
            iter.bucket_it->erase(iter.it);
            --elems;
        }

        inline iterator begin() {
            auto try_begin_it = iterator(
                    buckets[0].begin(),
                    bucket_iterator(0, buckets.begin()),
                    bucket_iterator(0, buckets.begin()),
                    bucket_iterator(buckets.size(), buckets.end())
            );

            if (buckets[0].empty())
                ++try_begin_it;

            return try_begin_it;
        }

        inline const_iterator begin() const {
            auto try_begin_it = const_iterator(
                    buckets[0].begin(),
                    const_bucket_iterator(0, buckets.begin()),
                    const_bucket_iterator(0, buckets.begin()),
                    const_bucket_iterator(buckets.size(), buckets.end())
            );

            if (buckets[0].empty())
                ++try_begin_it;

            return try_begin_it;
        }

        inline iterator end() {
            return iterator(
                    buckets[number_of_buckets - 1].end(),
                    bucket_iterator(number_of_buckets - 1, buckets.begin()),
                    bucket_iterator(0, buckets.begin()),
                    bucket_iterator(buckets.size(), buckets.end())
            );
        }

        inline const_iterator end() const {
            return const_iterator(
                    buckets[number_of_buckets - 1].end(),
                    const_bucket_iterator(number_of_buckets - 1, buckets.begin()),
                    const_bucket_iterator(0, buckets.begin()),
                    const_bucket_iterator(buckets.size(), buckets.end())
            );
        }

        inline std::size_t get_size() const override { return elems; }

        template<typename ForwardIterator> inline hash_set<T> difference(
                const std::size_t len,
                const ForwardIterator begin,
                const ForwardIterator end
        ) const {
            hash_set<T> set(len, hash_algo);
            const const_iterator end_iter = this->end();

            for (auto it = begin; it != end; ++it)
                if (find(*it) == end_iter)
                    set.insert(*it);

            return set;
        }

        template<typename ForwardIterator> inline std::vector<T> difference_vec(
                const std::size_t len,
                const ForwardIterator begin,
                const ForwardIterator end
        ) const {
            std::vector<T> set; set.reserve(len);
            const const_iterator end_iter = this->end();

            for (auto it = begin; it != end; ++it)
                if (find(*it) == end_iter)
                    set.push_back(*it);

            return set;
        }
    };

    inline std::size_t hash_string(const std::string& str) {
        return std::accumulate(
                str.begin(),
                str.end(),
                0,
                [](std::size_t acc, const char& c) {
                    const std::size_t x = static_cast<std::size_t>(c - 'a' + 1);
                    return (acc * 12345 + x) % 1000000007;
                }
        );
    }
}

namespace {
    template <typename T, typename RandomAccessIterator, typename Cmp> void merge_sort(
            RandomAccessIterator begin,
            RandomAccessIterator end,
            const Cmp cmp
    ) {
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

    struct enumerated_string {
        std::string s;
        std::size_t ind;

        enumerated_string() = default;
        enumerated_string(const std::string s, const std::size_t ind) : s(s), ind(ind) {}
        enumerated_string(const std::string&& s, const std::size_t&& ind) : s(s), ind(ind) {}
        ~enumerated_string() = default;

        inline bool operator== (const enumerated_string& other) const {
            return s == other.s;
        }

        inline enumerated_string& operator= (const enumerated_string other) {
            s = other.s;
            ind = other.ind;
            return *this;
        }
    };
}

int main() {
    std::ios_base::sync_with_stdio(false);

    int n = 0;
    std::cin >> n;

    const auto hash_enumerated_strings = [](const enumerated_string& p) {
        return dsa_hw::hash_string(p.s);
    };

    dsa_hw::hash_set<enumerated_string> set1(n, hash_enumerated_strings);

    for (int i = 0; i < n; ++i) {
        std::string s;
        std::cin >> s;

        const enumerated_string p(s, i);
        set1.insert(p);
    }

    int m = 0;
    std::cin >> m;

    dsa_hw::hash_set<enumerated_string> set2(m, hash_enumerated_strings);

    for (int i = 0; i < m; ++i) {
        std::string s;
        std::cin >> s;

        const enumerated_string p(s, i);
        set2.insert(p);
    }

    auto difference_btw_1_2 = set1.difference_vec(set2.get_size(), set2.begin(), set2.end());

    std::cout << difference_btw_1_2.size() << std::endl;

    merge_sort<enumerated_string>(difference_btw_1_2.begin(), difference_btw_1_2.end(), [](const auto& f, const auto& s) {
        return f.ind < s.ind;
    });

    for (const auto& [string, ind] : difference_btw_1_2)
        std::cout << string << std::endl;

    return 0;
}
