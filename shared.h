#pragma once

#include "sw_fwd.h"

#include <cstddef>
#include <memory>

template<typename T>
class SharedPtr {
    template<typename Y>
    friend class SharedPtr;

    template<typename Y>
    friend class WeakPtr;
public:

    SharedPtr() {
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    SharedPtr(std::nullptr_t) {
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    explicit SharedPtr(T *ptr) {
        control_block_ = new ControlBlockHeap<T>(ptr);
        ptr_ = ptr;
    }

    template<typename Y>
    SharedPtr(Y *ptr) {
        control_block_ = new ControlBlockHeap<Y>(ptr);
        ptr_ = ptr;
    }

    explicit SharedPtr(ControlBlockStack<T> *ptr) {
        control_block_ = ptr;
        ptr_ = ptr->GetPtr();
    }

    SharedPtr(const SharedPtr &other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        IncreaseCounter();
    }

    SharedPtr(SharedPtr &&other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template<typename Y>
    SharedPtr(const SharedPtr<Y> &other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        IncreaseCounter();
    }

    template<typename Y>
    SharedPtr(SharedPtr<Y> &&other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template<typename Y>
    SharedPtr(const SharedPtr<Y> &other, T *ptr) {
        control_block_ = other.control_block_;
        ptr_ = ptr;
        IncreaseCounter();
    }

    explicit SharedPtr(const WeakPtr<T> &other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }

        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    SharedPtr &operator=(const SharedPtr &other) {
        if (this == &other) {
            return *this;
        }

        DecreaseCounter();

        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        IncreaseCounter();

        return *this;
    }

    SharedPtr &operator=(SharedPtr &&other) {
        if (this == &other) {
            return *this;
        }

        DecreaseCounter();

        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;

        return *this;
    }

    void IncreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }

        control_block_->IncreaseStrongCounter();
    }

    void DecreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }

        control_block_->DecreaseStrongCounter();

        if (UseCount() == 0 && UseCountWeak() == 0) {
            delete control_block_;
        }

        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    size_t UseCount() const {
        if (control_block_ == nullptr) {
            return 0;
        }

        return control_block_->UseCountStrong();
    }

    size_t UseCountWeak() const {
        if (control_block_ == nullptr) {
            return 0;
        }

        return control_block_->UseCountWeak();
    }

    ~SharedPtr() {
        DecreaseCounter();
    }

    void Reset() {
        DecreaseCounter();
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    void Reset(T *ptr) {
        DecreaseCounter();
        control_block_ = new ControlBlockHeap<T>(ptr);
        ptr_ = ptr;
    }

    template<typename Y>
    void Reset(Y *ptr) {
        DecreaseCounter();
        control_block_ = new ControlBlockHeap<Y>(ptr);
        ptr_ = ptr;
    }

    void Swap(SharedPtr &other) {
        std::swap(control_block_, other.control_block_);
        std::swap(ptr_, other.ptr_);
    }

    T *Get() const {
        return ptr_;
    }

    T &operator*() const {
        return *ptr_;
    }

    T *operator->() const {
        return ptr_;
    }

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    ControlBlock *control_block_;
    T *ptr_;
};

template<typename T, typename... Args>
SharedPtr<T> MakeShared(Args &&... args) {
    return SharedPtr<T>(new ControlBlockStack<T>(std::forward<Args>(args)...));
}
