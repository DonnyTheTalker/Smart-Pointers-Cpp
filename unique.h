#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t
#include <type_traits>

template <class U>
struct Slug {
    Slug() {
    }
    template <class V>
    Slug(const Slug<V>&) {
    }

    template <class V>
    Slug(Slug<V>&&) {
    }

    void operator()(U* ptr) const {
        delete ptr;
    }
};

template <class U>
struct Slug<U[]> {
    Slug() {
    }
    template <class V>
    Slug(const Slug<V>&) {
    }

    template <class V>
    Slug(Slug<V>&&) {
    }

    void operator()(U* ptr) const {
        delete[] ptr;
    }
};

template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    template <typename Q, typename V>
    friend class UniquePtr;

public:

    explicit UniquePtr(T* ptr = nullptr) : value_(ptr, Deleter()) {
    }

    template <typename V>
    UniquePtr(T* ptr, V&& deleter) noexcept : value_(ptr, std::forward<V>(deleter)) {
    }

    UniquePtr(const UniquePtr& other) = delete;

    template <typename V, typename Q>
    UniquePtr(UniquePtr<V, Q>&& other) noexcept : value_(other.Get(), other.GetDeleter()) {
        other.Get() = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Clear();
            Get() = std::move(other.Get());
            GetDeleter() = std::move(other.GetDeleter());
            other.Get() = nullptr;
        }
        return *this;
    }

    UniquePtr& operator=(const UniquePtr& other) = delete;
    template <typename V, typename Q>
    UniquePtr& operator=(UniquePtr<V, Q>&& other) noexcept {
        Clear();
        Get() = std::move(other.Get());
        GetDeleter() = std::move(other.GetDeleter());
        other.Get() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Clear();
        return *this;
    }

    ~UniquePtr() {
        Clear();
    }

    T* Release() {
        T* ptr = Get();
        Get() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) {
        T* old_ptr = Get();
        Get() = ptr;
        GetDeleter()(old_ptr);
    }
    void Swap(UniquePtr& other) {
        std::swap(value_, other.value_);
    }

    T* Get() const {
        return value_.first();
    }
    explicit operator bool() const {
        return (Get() != nullptr);
    }

    std::add_lvalue_reference_t<T> operator*() const {
        return *Get();
    }
    T* operator->() const {
        return Get();
    }

private:  
    void Clear() {
        auto& ptr = Get();
        if (ptr != nullptr) {
            GetDeleter()(ptr);
            ptr = nullptr;
        }
    }

    Deleter& GetDeleter() {
        return value_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return value_.GetSecond();
    }

private:
    compressed_pair<T*, Deleter> value_;
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    template <typename Q, typename V>
    friend class UniquePtr;

public:

    explicit UniquePtr(T* ptr = nullptr) : value_(ptr, Deleter()) {
    }

    template <typename V>
    UniquePtr(T* ptr, V&& deleter) noexcept : value_(ptr, std::forward<V>(deleter)) {
    }

    UniquePtr(const UniquePtr& other) = delete;

    template <typename V, typename Q>
    UniquePtr(UniquePtr<V, Q>&& other) noexcept : value_(other.Get(), other.GetDeleter()) {
        other.Get() = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Clear();
            Get() = std::move(other.Get());
            GetDeleter() = std::move(other.GetDeleter());
            other.Get() = nullptr;
        }
        return *this;
    }

    UniquePtr& operator=(const UniquePtr& other) = delete;
    template <typename V, typename Q>
    UniquePtr& operator=(UniquePtr<V, Q>&& other) noexcept {
        Clear();
        Get() = std::move(other.Get());
        GetDeleter() = std::move(other.GetDeleter());
        other.Get() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Clear();
        return *this;
    }

    ~UniquePtr() {
        Clear();
    }

    T* Release() {
        T* ptr = Get();
        Get() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) {
        T* old_ptr = Get();
        Get() = ptr;
        GetDeleter()(old_ptr);
    }
    void Swap(UniquePtr& other) {
        std::swap(value_, other.value_);
    }

    T* Get() const {
        return value_.first();
    }
    explicit operator bool() const {
        return (Get() != nullptr);
    }

    std::add_lvalue_reference_t<T> operator*() const {
        return *Get();
    }
    T* operator->() const {
        return Get();
    }

    T& operator[](size_t i) const {
        return Get()[i];
    }

private:  
    void Clear() {
        auto& ptr = Get();
        if (ptr != nullptr) {
            GetDeleter()(ptr);
            ptr = nullptr;
        }
    }

    Deleter& GetDeleter() {
        return value_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return value_.GetSecond();
    }

private:
    compressed_pair<T*, Deleter> value_;
};

     