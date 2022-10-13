#pragma once

#include <exception>

class BadWeakPtr : public std::exception {
};

template<typename T>
class SharedPtr;

template<typename T>
class WeakPtr;


class ControlBlock {
public:

    virtual ~ControlBlock() = default;

private:
    virtual void Destroy() = 0;

public:
    void IncreaseStrongCounter() {
        ++cnt_strong_;
    }

    void IncreaseWeakCounter() {
        ++cnt_weak_;
    }

    void DecreaseStrongCounter() {
        --cnt_strong_;
        if (cnt_strong_ == 0) {
            Destroy();
        }
    }

    void DecreaseWeakCounter() {
        --cnt_weak_;
    }

    size_t UseCountStrong() const {
        return cnt_strong_;
    }

    size_t UseCountWeak() const {
        return cnt_weak_;
    }

private:
    size_t cnt_strong_;
    size_t cnt_weak_;
};

template<typename T>
class ControlBlockHeap : public ControlBlock {
public:
    ControlBlockHeap(T *ptr) {
        cnt_strong_ = 1;
        cnt_weak_ = 0;
        ptr_ = ptr;
    }

    ControlBlockHeap(const ControlBlockHeap &other) {
        cnt_strong_ = other.cnt_strong_;
        ptr_ = other.ptr_;
    }

    template<typename Y>
    ControlBlockHeap(const ControlBlockHeap<Y> &other) {
        cnt_strong_ = other.cnt_strong_;
        ptr_ = other.ptr_;
    }

    ControlBlockHeap &operator=(const ControlBlockHeap &other) {
        if (this == &other) {
            return *this;
        }

        cnt_strong_ = other.cnt_strong_;
        ptr_ = other.ptr_;

        return *this;
    }

    template<typename Y>
    ControlBlockHeap &operator=(const ControlBlockHeap<Y> &other) {
        if (this == &other) {
            return *this;
        }

        cnt_strong_ = other.cnt_strong_;
        ptr_ = other.ptr_;

        return *this;
    }

private:
    void Destroy() override {
        delete ptr_;
    }

private:
    T *ptr_;
};

template<typename T>
class ControlBlockStack : public ControlBlock {
public:
    template<typename... Args>
    ControlBlockStack(Args &&... args) {
        cnt_strong_ = 1;
        cnt_weak_ = 0;
        ::new(&ptr_) T(std::forward<Args>(args)...);
    }

    T *GetPtr() {
        return reinterpret_cast<T *>(&ptr_);
    }

private:
    void Destroy() override {
        std::destroy_at(std::launder(reinterpret_cast<T *>(&ptr_)));
    }


private:
    std::aligned_storage_t<sizeof(T), alignof(T)> ptr_;
};
