#pragma once

#include "sw_fwd.h"
#include "shared.h"

template <typename T>
class WeakPtr {
public:
    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;

public:

    WeakPtr() {
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    WeakPtr(const WeakPtr& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    template <typename Y>
    WeakPtr(const WeakPtr<Y>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        IncreaseCounter();
    }

    WeakPtr(WeakPtr&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename Y>
    WeakPtr(WeakPtr<Y>&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    WeakPtr(const SharedPtr<T>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }

        DecreaseCounter();

        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        IncreaseCounter();

        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
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

    ~WeakPtr() {
        DecreaseCounter();
    }

    void Reset() {
        DecreaseCounter();
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    void Swap(WeakPtr& other) {
        std::swap(control_block_, other.control_block_);
        std::swap(ptr_, other.ptr_);
    }

    void IncreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }

        control_block_->IncreaseWeakCounter();
    }

    void DecreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }

        control_block_->DecreaseWeakCounter();

        if (control_block_->UseCountWeak() == 0 && control_block_->UseCountStrong() == 0) {
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

    bool Expired() const {
        return UseCount() == 0;
    }

    SharedPtr<T> Lock() const {
        if (Expired()) {
            return SharedPtr<T>();
        }
        return SharedPtr<T>(*this);
    }

private:
    ControlBlock* control_block_;
    T* ptr_;
};
