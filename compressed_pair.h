#pragma once

#include <memory>
#include <type_traits>

namespace details {
    enum compressed_pair_type {
        BOTH_EMPTY = 0,
        FIRST_EMPTY,
        SECOND_EMPTY,
        NONE_EMPTY,
        BOTH_EMPTY_EQ,
        NONE_EMPTY_EQ
    };

    template<typename T, typename V, bool IsSame, bool FirstEmpty, bool SecondEmpty>
    struct compressed_pair_def;

    template<typename T, typename V>
    struct compressed_pair_def<T, V, false, false, false> {
        static constexpr int val = NONE_EMPTY;
    };

    template<typename T, typename V>
    struct compressed_pair_def<T, V, false, false, true> {
        static constexpr int val = SECOND_EMPTY;
    };

    template<typename T, typename V>
    struct compressed_pair_def<T, V, false, true, false> {
        static constexpr int val = FIRST_EMPTY;
    };

    template<typename T, typename V>
    struct compressed_pair_def<T, V, false, true, true> {
        static constexpr int val = BOTH_EMPTY;
    };

    template<typename T, typename V>
    struct compressed_pair_def<T, V, true, true, true> {
        static constexpr int val = BOTH_EMPTY_EQ;
    };

    template<typename T, typename V>
    struct compressed_pair_def<T, V, true, false, false> {
        static constexpr int val = NONE_EMPTY_EQ;
    };

    template<typename T, bool IsEmpty = (!std::is_final_v<T> & std::is_empty_v<T>)>
    struct empty_def {
        static constexpr int val = IsEmpty;
    };


    template<typename T, typename V, int Variation>
    class compressed_pair_val;

    template<typename T, typename V>
    class compressed_pair_val<T, V, NONE_EMPTY> {
    public:
        compressed_pair_val() : first_(), second_() {
        }

        template<typename First, typename Second>
        compressed_pair_val(First &&first, Second &&second)
                : first_(std::forward<First>(first)), second_(std::forward<Second>(second)) {
        }

        T &first() {
            return first_;
        }

        const T &first() const {
            return first_;
        }

        V &second() {
            return second_;
        }

        const V &second() const {
            return second_;
        }

    private:
        T first_;
        V second_;
    };

    template<typename T, typename V>
    class compressed_pair_val<T, V, SECOND_EMPTY> : public V {
    public:
        compressed_pair_val() : first_(), V() {
        }

        template<typename First, typename Second>
        compressed_pair_val(First &&first, Second &&second)
                : first_(std::forward<First>(first)), V(std::forward<Second>(second)) {
        }

        T &first() {
            return first_;
        }

        const T &first() const {
            return first_;
        }

        V &second() {
            return *this;
        }

        const V &second() const {
            return *this;
        }

    private:
        T first_;
    };

    template<typename T, typename V>
    class compressed_pair_val<T, V, FIRST_EMPTY> : public T {
    public:
        compressed_pair_val() : T(), second_() {
        }

        template<typename First, typename Second>
        compressed_pair_val(First &&first, Second &&second)
                : T(std::forward<First>(first)), second_(std::forward<Second>(second)) {
        }

        T &first() {
            return *this;
        }

        const T &first() const {
            return *this;
        }

        V &second() {
            return second_;
        }

        const V &second() const {
            return second_;
        }

    private:
        V second_;
    };

    template<typename T, typename V>
    class compressed_pair_val<T, V, BOTH_EMPTY> : public T, public V {
    public:
        compressed_pair_val() : T(), V() {
        }

        template<typename First, typename Second>
        compressed_pair_val(First &&first, Second &&second)
                : T(std::forward<First>(first)), V(std::forward<Second>(second)) {
        }

        T &first() {
            return *this;
        }

        const T &first() const {
            return *this;
        }

        V &second() {
            return *this;
        }

        const V &second() const {
            return *this;
        }
    };

    template<typename T, typename V>
    class compressed_pair_val<T, V, NONE_EMPTY_EQ> {
    public:
        compressed_pair_val() : first_(), second_() {
        }

        template<typename First, typename Second>
        compressed_pair_val(First &&first, Second &&second)
                : first_(std::forward<First>(first)), second_(std::forward<Second>(second)) {
        }

        T &first() {
            return first_;
        }

        const T &first() const {
            return first_;
        }

        V &second() {
            return second_;
        }

        const V &second() const {
            return second_;
        }

    private:
        T first_;
        V second_;
    };

    template<typename T, typename V>
    class compressed_pair_val<T, V, BOTH_EMPTY_EQ> : public T {
    public:
        compressed_pair_val() : first_(), T() {
        }

        template<typename First, typename Second>
        compressed_pair_val(First &&first, Second &&second)
                : first_(std::forward<First>(first)), V(std::forward<Second>(second)) {
        }

        T &first() {
            return first_;
        }

        const T &first() const {
            return first_;
        }

        V &second() {
            return *this;
        }

        const V &second() const {
            return *this;
        }

    private:
        T first_;
    };
}// namespace details

template<typename T, typename V>
class compressed_pair
        : public details::compressed_pair_val<T, V,
                details::compressed_pair_def<T, V, false, details::empty_def<T>::val, details::empty_def<V>::val>::val> {
public:
    using base = details::compressed_pair_val<T, V,
            details::compressed_pair_def<T, V, false, details::empty_def<T>::val, details::empty_def<V>::val>::val>;
    using base::base;
};

template<typename T>
class compressed_pair<T, T>
        : public details::compressed_pair_val<
                T, T, details::compressed_pair_def<T, T, true, details::empty_def<T>::val, details::empty_def<T>::val>::val> {
public:
    using base = details::compressed_pair_val<
            T, T, details::compressed_pair_def<T, T, true, details::empty_def<T>::val, details::empty_def<T>::val>::val>;
    using base::base;
};