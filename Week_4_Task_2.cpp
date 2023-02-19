// Arseny Savchenko

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <numeric>
#include <sstream>

namespace dsa_hw {
    namespace hash {

        // ############################ Map ADL ############################

        /**
         * Map ADL with basic insertion and remove strategy
         * @param K key type
         * @param V value type
         */

        template <typename K, typename V> struct map {

            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             *
             * @param key entry's key to insert
             * @param value value to insert / update
             */

            virtual void insert(K key, V value) noexcept = 0;

            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             *
             * @param key entry's key to insert
             * @param value value to insert / update
             */

            virtual void insert(K&& key, V&& value) noexcept = 0;

            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             * @param entry key and value to insert
             */

            virtual void insert(std::pair<K, V> entry) noexcept = 0;

            /**
             * Inserts value by its key.
             * If key is already present in the map,
             * updates with given value
             * @param entry key and value to insert
             */

            virtual void insert(std::pair<K, V>&& entry) noexcept = 0;

            /**
             * Gets value by its key.
             * If key is not present in the map,
             * creates default value with default constructor
             * @param key entry's key to access value
             */

            virtual V& operator[] (const K& key) noexcept = 0;

            /**
             * Removes value by its key, if present.
             * @param key entry's key to access value
             * @return true if value was removed, otherwise false
             */

            virtual bool remove(const K& key) noexcept = 0;

            /** @return Number of inserted elements */

            [[nodiscard]] virtual std::size_t get_size() const noexcept = 0;

            /**
             * Are there any elements?
             * @return true if size is zero
             */

            [[nodiscard]] constexpr inline bool is_empty() const noexcept { return get_size() == 0; }

            /**
             * Are there any elements?
             * @return false if size is zero
             */

            [[nodiscard]] constexpr inline bool is_not_empty() const noexcept { return !is_empty(); }
        };

        // ############################ Hash Map ############################

        /**
         * Hash map implementation with side-chaining (buckets).
         * By default, the number of buckets is 16; Load factor is 75%.
         * By default, std::hash<K> is used to hash keys. To provide
         * your personal hasher, you have to override std::size_t operator()
         * with your hashing strategy. You can use Boost's hash_combine() method
         * for your data structure.
         *
         * @param K key's type
         * @param V value's type
         * @param H hasher class with overridden std::size_t operator(). By default std::hash<K> is used
         * @param KEq key's equality checker. By default std::equal_to<K> is used
         */

        template <typename K, typename V, typename H = std::hash<K>, typename KEq = std::equal_to<K>> class hash_map : public map<K, V> {
            constexpr static const float LOAD_FACTOR = 0.75F;

            H hasher = H();
            KEq key_equality_checker = KEq();

            std::size_t elems = 0;
            std::vector<std::list<std::pair<K, V>>> buckets;

            class const_bucket_iterator;

            // ############################ Bucket Iterator ############################

            /** Random access iterator to iterate over the buckets */

            class bucket_iterator {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::list<std::pair<K, V>>;
                using pointer = std::list<std::pair<K, V>>*;
                using reference = value_type&;

            private:
                /** Index of current bucket */
                std::size_t index;

                /** Begin iterator over the buckets (first element's iterator) */
                std::vector<std::list<std::pair<K, V>>>::iterator begin;

            public:

                /**
                 * Creates bucket iterator from bucket's index and begin iterator
                 * @param index index of current bucket
                 * @param begin iterator to the beginning of buckets
                 */

                bucket_iterator(
                        const std::size_t index,
                        std::vector<std::list<std::pair<K, V>>>::iterator begin
                ) noexcept : index(index), begin(begin) {}

                ~bucket_iterator() noexcept = default;

                // --------------- Dereference operators ---------------

                inline reference operator*() const noexcept { return *(begin + index); }
                inline pointer operator->() const noexcept { return &*(begin + index); }

                // --------------- Comparison operators ---------------

                constexpr inline bool operator==(const bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const bucket_iterator& other) const noexcept { return index != other.index; };

                constexpr inline bool operator==(const const_bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const const_bucket_iterator& other) const noexcept { return index != other.index; };

                // --------------- Movement operators ---------------

                inline bucket_iterator operator++() noexcept { ++index; return *this; }
                inline bucket_iterator operator--() noexcept { --index; return *this; }
                constexpr inline bucket_iterator operator+(const std::size_t move) const noexcept { return bucket_iterator(index + move, begin); }
                constexpr inline bucket_iterator operator-(const std::size_t move) const noexcept { return bucket_iterator(index - move, begin); }
            };

            [[nodiscard]] constexpr inline bucket_iterator get_bucket_begin_it() const noexcept {
                return bucket_iterator(0, buckets.begin());
            }

            [[nodiscard]] constexpr inline bucket_iterator get_bucket_end_it() const noexcept {
                return bucket_iterator(buckets.size(), buckets.begin());
            }

            // ############################ Const Bucket Iterator ############################

            /**
             * Random access iterator to iterate over the buckets.
             * Not able to modify its value
             */

            class const_bucket_iterator {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::list<std::pair<K, V>>;
                using pointer = const std::list<std::pair<K, V>>*;
                using reference = const value_type&;

            private:
                /** Index of current bucket */
                std::size_t index;

                /** Begin iterator over the buckets (first element's iterator) */
                std::vector<std::list<std::pair<K, V>>>::const_iterator begin;

            public:

                /**
                 * Creates bucket iterator from bucket's index and begin iterator
                 * @param index index of current bucket
                 * @param begin iterator to the beginning of buckets
                 */

                const_bucket_iterator(
                        const std::size_t index,
                        const std::vector<std::list<std::pair<K, V>>>::const_iterator begin
                ) noexcept : index(index), begin(begin) {}

                ~const_bucket_iterator() noexcept = default;

                // --------------- Dereference operators ---------------

                inline reference operator*() const noexcept { return *(begin + index); }
                inline pointer operator->() const noexcept { return &*(begin + index); }

                // --------------- Comparison operators ---------------

                constexpr inline bool operator==(const bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const bucket_iterator& other) const noexcept { return index != other.index; };

                constexpr inline bool operator==(const const_bucket_iterator& other) const noexcept { return index == other.index; };
                constexpr inline bool operator!=(const const_bucket_iterator& other) const noexcept { return index != other.index; };

                // --------------- Movement operators ---------------

                inline const_bucket_iterator operator++() noexcept { ++index; return *this; }
                inline const_bucket_iterator operator--() noexcept { --index; return *this; }
                constexpr inline const_bucket_iterator operator+(const std::size_t move) const noexcept { return const_bucket_iterator(index + move, begin); }
                constexpr inline const_bucket_iterator operator-(const std::size_t move) const noexcept { return const_bucket_iterator(index - move, begin); }
            };

			// --------------- Resize strategies ---------------

            /** Checks if number of elements greater than load factor */

            [[nodiscard]] constexpr inline bool is_load_factor_reached() const noexcept {
                return 1.0F * elems / buckets.size() > LOAD_FACTOR;
            }

			/**
			 * If load factor is reached, doubles the number of buckets
			 * and reinserts all elements from buffer.
			 *
			 * Takes O(n) to buffer values with move semantics +
			 * O(n + hash strategy) to insert new values in average case
			 *
			 * @return true if load factor was reached and
			 * the number of buckets was increased
			 */

            inline bool resize_if_load_factor_reached() noexcept {
                if (is_load_factor_reached()) {
                    std::vector<std::pair<K, V>> store;
                    std::move(begin(), end(), std::back_inserter(store));

                    const std::size_t new_buckets_number = buckets.size() << 1;

					// Trying to save as much memory as possible

                    elems = 0;
                    buckets.clear();
                    buckets.resize(new_buckets_number);
                    buckets.shrink_to_fit();

                    std::for_each(store.begin(), store.end(), [this](auto&& p) { this->insert_after_resize(p.first, p.second); });
                    return true;
                }

                return false;
            }

			// --------------- Search key in bucket ---------------

			/**
			 * Searches entry by given key with O(n) in worst case.
			 * @param bucket bucket to search in
			 * @param key entry's key
			 * @return entry's iterator or bucket's end if it was not found
			 */

			[[nodiscard]] inline std::list<std::pair<K, V>>::iterator find_by_key(
					std::list<std::pair<K, V>>& bucket,
					const K& key
			) noexcept {
                return std::find_if(
                        bucket.begin(),
                        bucket.end(),
                        [this, &key](const auto& p) { return key_equality_checker(p.first, key); }
                );
            }

			/**
			 * Searches entry by given key with O(n) in worst case.
			 * @param bucket bucket to search in
			 * @param key entry's key
			 * @return entry's iterator or bucket's end if it was not found
			 */

            [[nodiscard]] constexpr inline std::list<std::pair<K, V>>::const_iterator find_by_key(
					const std::list<std::pair<K, V>>& bucket,
					const K& key
			) const noexcept {
                return std::find_if(
                        bucket.begin(),
                        bucket.end(),
                        [this, &key](const auto& p) { return key_equality_checker(p.first, key); }
                );
            }

        public:
            class const_iterator;

			// ############################ Iterator ############################

			/** Bidirectional iterator to iterate over map's entries */

            class iterator {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::pair<K, V>;
                using pointer = value_type*;
                using reference = value_type&;

            private:
				friend class hash_map<K, V, H>;

				/** Iterator over current bucket */
                std::list<std::pair<K, V>>::iterator it;

				/** Iterator to current bucket */
                bucket_iterator bucket_it;

				/** Iterator to first bucket */
                bucket_iterator first_bucket_it;

				/** Iterator to buckets' end */
                bucket_iterator end_buckets_it;

				/**
				 * Creates new iterator
				 * @param it current bucket's iterator
				 * @param bucket_it iterator to current bucket
				 * @param first_bucket_it iterator to first bucket
				 * @param end_buckets_it iterator to buckets' end
				 */

				iterator(
						const std::list<std::pair<K, V>>::iterator it,
						const bucket_iterator bucket_it,
						const bucket_iterator first_bucket_it,
						const bucket_iterator end_buckets_it
				) noexcept : it(it), bucket_it(bucket_it), first_bucket_it(first_bucket_it), end_buckets_it(end_buckets_it) {};

			public:
                ~iterator() noexcept = default;

				// --------------- Dereference operators ---------------

                inline reference operator*() const noexcept { return *it; }
                inline pointer operator->() const noexcept { return &*it; }

				// --------------- Validation operators ---------------

				constexpr inline explicit operator bool() const noexcept { return it != nullptr && it->has_value(); }
                constexpr inline bool operator!() const noexcept { return it == nullptr || !it->has_value(); }

				// --------------- Comparison operators ---------------

                constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };

                constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };

				// --------------- Movement operators ---------------

				/**
				 * Searches for the next entry and moves iterator to it.
				 * Firstly, checks if there is next entry to move in bucket (not the end of bucket).
				 * If so, it will return iterator on it. In other case, starts searching for the next
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Implementation does not check if the next entry exists! In other words, if you are out of bounds,
				 * the exception will not be thrown and you will have segmentation fault. It is up to you
				 * to check bounds.
				 * @return iterator to the next entry
				 */

                inline iterator operator++() noexcept {
                    if (std::next(it) == bucket_it->end()) {
                        while (bucket_it + 1 != end_buckets_it && (bucket_it + 1)->empty())
                            ++bucket_it;

                        it = (bucket_it + 1 == end_buckets_it) ? bucket_it->end() : (++bucket_it)->begin();
                    } else { ++it; }

                    return *this;
                }

				/**
				 * Searches for the previous entry and moves iterator to it.
				 * Firstly, checks if there is previous entry to move in bucket.
				 * If so, it will return iterator on it. In other case, starts searching for the previous
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Iterator before bucket's end will be returned.
				 * Implementation does not check if the previous entry exists!
				 * In other words, if you are out of bounds, the exception will not be thrown
				 * and you will have segmentation fault. It is up to you to check bounds.
				 * @return iterator to the previous entry
				 */

                inline iterator operator--() noexcept {
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

        private:

			/**
			 * Simplified way to create iterator
			 * @param it current bucket's iterator
			 * @param hashed_ind key's hashed index or bucket's index
			 */

            [[nodiscard]] constexpr inline iterator iter(
                    std::list<std::pair<K, V>>::iterator it,
                    const std::size_t hashed_ind
            ) noexcept {
                return iterator(
                        it,
                        bucket_iterator(hashed_ind, buckets.begin()),
                        bucket_iterator(0, buckets.begin()),
                        bucket_iterator(buckets.size(), buckets.end())
                );
            }

        public:

			// ############################ Const Iterator ############################

			/**
			 * Bidirectional iterator to iterate over map's entries.
			 * Not able to modify its value
			 */

            class const_iterator {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using difference_type = std::ptrdiff_t;
                using value_type = std::pair<K, V>;
                using pointer = const value_type*;
                using reference = const value_type&;

            private:
				friend class hash_map<K, V, H>;

				/** Iterator over current bucket */
                std::list<std::pair<K, V>>::const_iterator it;

				/** Iterator to current bucket */
                const_bucket_iterator bucket_it;

				/** Iterator to first bucket */
                const const_bucket_iterator first_bucket_it;

				/** Iterator to buckets' end */
                const const_bucket_iterator end_buckets_it;

				/**
				 * Creates new iterator
				 * @param it current bucket's iterator
				 * @param bucket_it iterator to current bucket
				 * @param first_bucket_it iterator to first bucket
				 * @param end_buckets_it iterator to buckets' end
				 */

				const_iterator(
						const std::list<std::pair<K, V>>::const_iterator it,
						const const_bucket_iterator bucket_it,
						const const_bucket_iterator first_bucket_it,
						const const_bucket_iterator end_buckets_it
				) noexcept : it(it), bucket_it(bucket_it), first_bucket_it(first_bucket_it), end_buckets_it(end_buckets_it) {};

			public:
                ~const_iterator() noexcept = default;

				// --------------- Dereference operators ---------------

                inline reference operator*() const noexcept { return *it; }
                inline pointer operator->() const noexcept { return &*it; }

				// --------------- Validation operators ---------------

				constexpr inline explicit operator bool() const noexcept { return it != nullptr && it->has_value(); }
                constexpr inline bool operator!() const noexcept { return it == nullptr || !it->has_value(); }

				// --------------- Comparison operators ---------------

                constexpr inline bool operator==(const iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const iterator& other) const noexcept { return it != other.it; };

                constexpr inline bool operator==(const const_iterator& other) const noexcept { return it == other.it; };
                constexpr inline bool operator!=(const const_iterator& other) const noexcept { return it != other.it; };

				// --------------- Movement operators ---------------

				/**
				 * Searches for the next entry and moves iterator to it.
				 * Firstly, checks if there is next entry to move in bucket (not the end of bucket).
				 * If so, it will return iterator on it. In other case, starts searching for the next
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Implementation does not check if the next entry exists! In other words, if you are out of bounds,
				 * the exception will not be thrown and you will have segmentation fault. It is up to you
				 * to check bounds.
				 * @return iterator to the next entry
				 */

                inline const_iterator operator++() noexcept {
                    if (std::next(it) == bucket_it->end()) {
                        while (bucket_it + 1 != end_buckets_it && (bucket_it + 1)->empty())
                            ++bucket_it;

                        it = (bucket_it + 1 == end_buckets_it) ? bucket_it->end() : (++bucket_it)->begin();
                    } else { ++it; }

                    return *this;
                }

				/**
				 * Searches for the previous entry and moves iterator to it.
				 * Firstly, checks if there is previous entry to move in bucket.
				 * If so, it will return iterator on it. In other case, starts searching for the previous
				 * bucket that is not empty. In worst case O(k), where k is the current number of buckets.
				 * Iterator before bucket's end will be returned.
				 * Implementation does not check if the previous entry exists!
				 * In other words, if you are out of bounds, the exception will not be thrown
				 * and you will have segmentation fault. It is up to you to check bounds.
				 * @return iterator to the previous entry
				 */

                inline const_iterator operator--() noexcept {
                    if (it == bucket_it->begin()) {
                        while (bucket_it != first_bucket_it && (bucket_it - 1)->empty())
                            --bucket_it;

                        if (bucket_it != first_bucket_it)
                            it = std::prev((--bucket_it)->end());
                        else if (it != first_bucket_it->begin())
                            it = std::prev(bucket_it->end());
                    } else { --it; }

                    return *this;
                }
            };

        private:

			/**
			 * Simplified way to create constant iterator
			 * @param it current bucket's iterator
			 * @param hashed_ind key's hashed index or bucket's index
			 */

            [[nodiscard]] constexpr inline const_iterator const_iter(
                    std::list<std::pair<K, V>>::const_iterator it,
                    const std::size_t hashed_ind
            ) const noexcept {
                return const_iterator(
                        it,
                        const_bucket_iterator(hashed_ind, buckets.begin()),
                        const_bucket_iterator(0, buckets.begin()),
                        const_bucket_iterator(buckets.size(), buckets.end())
                );
            }

			// --------------- Unsafe insertions ---------------

			/**
			 * Version of insertion that does not checks if value was already
			 * present in the map. However, it checks if load factor is reached and
			 * resizes map, if needed. In total O(hash strategy) with insertion only,
			 * and O(hash strategy + n) with resizing
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 * @return iterator to inserted entry
			 */

            inline iterator insert_unchecked(const K key, const V value) noexcept {
				++elems;
				const auto is_resized = resize_if_load_factor_reached();

                const auto hashed_ind = hasher(key) % buckets.size();
                auto& bucket = buckets[hashed_ind];

                bucket.push_back(std::make_pair(key, value));
                if (is_resized) ++elems;

                return iter(std::prev(bucket.end()), hashed_ind);
            }

			/**
			 * Version of insertion that does not checks if value was already
			 * present in the map. However, it checks if load factor is reached and
			 * resizes map, if needed. In total O(hash strategy) with insertion only,
			 * and O(hash strategy + n) with resizing
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 * @return iterator to inserted entry
			 */

            inline iterator insert_unchecked(K&& key, V&& value) noexcept {
				++elems;
				const auto is_resized = resize_if_load_factor_reached();

				const auto hashed_ind = hasher(key) % buckets.size();
				auto& bucket = buckets[hashed_ind];

				bucket.push_back(std::make_pair(key, value));
				if (is_resized) ++elems;

				return iter(std::prev(bucket.end()), hashed_ind);
            }

			/**
			 * Version of insertion that assumes that function was called after
			 * resizing when load factor was reached. No checks (load factor or presence)
			 * are done. Function must be called only after resizing! In total O(hashing strategy)
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */

            inline void insert_after_resize(const K key, const V value) noexcept {
                const auto hashed_ind = hasher(key) % buckets.size();
                auto& bucket = buckets[hashed_ind];
                bucket.push_back(std::make_pair(key, value));
                ++elems;
            }

			/**
			 * Version of insertion that assumes that function was called after
			 * resizing when load factor was reached. No checks (load factor or presence)
			 * are done. Function must be called only after resizing! In total O(hashing strategy)
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */

            inline void insert_after_resize(K&& key, V&& value) noexcept {
                const auto hashed_ind = hasher(key) % buckets.size();
                auto& bucket = buckets[hashed_ind];
                bucket.push_back(std::make_pair(key, value));
                ++elems;
            }

        public:

			// --------------- Constructors ---------------

			/** Creates hash_map with initial number of buckets */

            explicit hash_map(const std::size_t initial_number_of_buckets) noexcept {
                buckets = std::vector<std::list<std::pair<K, V>>>(initial_number_of_buckets);
                buckets.shrink_to_fit();
            }

			/** Creates hash_map with 16 buckets */

            hash_map() noexcept {
                buckets = std::vector<std::list<std::pair<K, V>>>(16);
                buckets.shrink_to_fit();
            }

            ~hash_map() noexcept = default;

			// --------------- Insertions ---------------

			/**
			 * Inserts element or updates present value with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */

            inline void insert(const K key, const V value) noexcept override {
                const auto hashed_ind = hasher(key) % buckets.size();
                auto& bucket = buckets[hashed_ind];
                auto it = find_by_key(bucket, key);

                if (it == bucket.end()) {
					++elems;

					if (resize_if_load_factor_reached())
						insert_after_resize(key, value);
					else
						bucket.push_back(std::make_pair(key, value));
                } else {
                    it->second = value;
                }
            }

			/**
			 * Inserts element or updates present value with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 *
			 * @param key entry's key to insert
			 * @param value entry's value to insert
			 */

            inline void insert(K&& key, V&& value) noexcept override {
				const auto hashed_ind = hasher(key) % buckets.size();
				auto& bucket = buckets[hashed_ind];
				auto it = find_by_key(bucket, key);

				if (it == bucket.end()) {
					++elems;

					if (resize_if_load_factor_reached())
						insert_after_resize(key, value);
					else
						bucket.push_back(std::make_pair(key, value));
				} else {
					it->second = value;
				}
            }

			/**
			 * Inserts element or updates present entry with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param entry key and value to insert
			 */

            inline void insert(const std::pair<K, V> entry) noexcept override {
                insert(entry.first, entry.second);
            }

			/**
			 * Inserts element or updates present entry with new one.
			 * Checks if load factor is reached and resizes map, if needed.
			 * In total O(hash strategy + n) in all cases.
			 * @param entry key and value to insert
			 */

            inline void insert(std::pair<K, V>&& value) noexcept override {
                insert(value.first, value.second);
            }

			// --------------- Searching ---------------

			/**
			 * Searches for entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return iterator to entry or end iterator if not present
			 */

			[[nodiscard]] inline iterator find(const K& key) noexcept {
                const auto hashed_ind = hasher(key) % buckets.size();
                auto& bucket = buckets[hashed_ind];
                const auto it = find_by_key(bucket, key);
                return it == bucket.end() ? end() : iter(it, hashed_ind);
            }

			/**
			 * Searches for entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return iterator to entry or end iterator if not present
			 */

            [[nodiscard]] inline const_iterator find(const K& key) const noexcept {
                const auto hashed_ind = hasher(key) % buckets.size();
                const auto& bucket = buckets[hashed_ind];
                const auto it = find_by_key(bucket, key);
                return it == bucket.end() ? end() : const_iter(it, hashed_ind);
            }

			/**
			 * Gets entry with given key or creates
			 * with default key if not present.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return reference to value
			 */

            inline V& operator[] (const K& key) noexcept override {
                auto it = find(key);

                if (it == end())
                    it = insert_unchecked(key, V());

                return it->second;
            }

			// --------------- Removing ---------------

			/**
			 * Removes entry with given key.
			 * In total O(hash strategy) in average case
			 * and O(hash strategy + n) in worst case.
			 * @param key entry's key to search
			 * @return true if value was removed
			 */

            inline bool remove(const K& key) noexcept override {
                const auto iter = find(key);

                if (iter == end())
                    return false;

                remove(iter);
                return true;
            }

			/**
			 * Removes entry by iterator.
			 * In total O(1) in all cases
			 * @param iter entry's iterator
			 */

            inline void remove(const iterator iter) noexcept {
                iter.bucket_it->erase(iter.it);
                --elems;
            }

			// --------------- Iterators ---------------

			/**
			 * Gets iterator to the first entry, if present.
			 * In worst case O(k), where k is the current number of buckets.
			 * @return iterator to the first entry or end iterator
			 * if there are no elements in map
			 */

			[[nodiscard]] inline iterator begin() noexcept {
				if (elems == 0)
					return end();

                auto try_begin_it = iter(buckets[0].begin(), 0);

                if (buckets[0].empty())
                    ++try_begin_it;

                return try_begin_it;
            }

			/**
			 * Gets iterator to the first entry, if present.
			 * In worst case O(k), where k is the current number of buckets.
			 * @return iterator to the first entry or end iterator
			 * if there are no elements in map
			 */

            [[nodiscard]] inline const_iterator begin() const noexcept {
				if (elems == 0)
					return end();

                auto try_begin_it = const_iter(buckets[0].begin(), 0);

                if (buckets[0].empty())
                    ++try_begin_it;

                return try_begin_it;
            }

			/**
			 * Gets last iterator that is out of bounds
			 * and does not have a value. Equality to this iterator
			 * means end of map.
			 * @return out of bounds iterator
			 */

			[[nodiscard]] constexpr inline iterator end() noexcept {
                return iter(buckets[buckets.size() - 1].end(), buckets.size() - 1);
            }

			/**
			 * Gets last iterator that is out of bounds
			 * and does not have a value. Equality to this iterator
			 * means end of map.
			 * @return out of bounds iterator
			 */

			[[nodiscard]] constexpr inline const_iterator end() const noexcept {
                return const_iter(buckets[buckets.size() - 1].end(), buckets.size() - 1);
            }

			/** Gets current number of elements in map */

			[[nodiscard]] constexpr inline std::size_t get_size() const noexcept override { return elems; }
        };
    }

    namespace utils {
		/**
		 * Converts `$number` string to long double
		 * @param cost_str string to convert
		 * @return parsed cost
		 */

        inline long double parse_cost(const std::string& cost_str) noexcept {
            std::string clone(cost_str.size(), '0');
            std::copy(cost_str.begin(), cost_str.end(), clone.begin());
            clone.replace(clone.find('$'), 1, "");
            return std::stold(clone);
        }

		/**
		 * Converts float128 to string with at least 1 number in mantissa part.
		 * Example: assert(to_string_with_mantissa(45.0) = std::string("45.0"))
		 * @param ld float128 to convert to string
		 */

        inline std::string to_string_with_mantissa(const long double ld) noexcept {
            std::stringstream out;
            out << ld;

            const auto& str = out.str();

            if (str.find('.') == std::string::npos)
                out << ".0";

            return out.str();
        }
    }
}

int main() {
    int n = 0;
    std::cin >> n;

	// Map with dates to clients; clients is map with IDs to vectors of costs
    dsa_hw::hash::hash_map<std::string, dsa_hw::hash::hash_map<std::string, std::vector<long double>>> dates_to_clients;

    for (int i = 0; i < n; ++i) {
        std::string input;
        std::getline(std::cin >> std::ws, input);

        std::stringstream stream(input);

        std::string date;
        stream >> date;

        std::string tmp;
        stream >> tmp;

        std::string id;
        stream >> id;

        stream >> tmp;
        const auto cost = dsa_hw::utils::parse_cost(tmp);

        dates_to_clients[date][id].push_back(cost);
    }

    for (const auto& [date, clients_with_orders] : dates_to_clients) {
        long double sum = 0;

		    // Calculating total sum for a client
        for (const auto& [id, orders] : clients_with_orders)
            sum += std::accumulate(orders.begin(), orders.end(), 0.0, std::plus<>());

        std::cout << date << " $" << dsa_hw::utils::to_string_with_mantissa(sum) << ' ' << clients_with_orders.get_size() << std::endl;
    }

    return 0;
}
