#pragma once

#include "sw_fwd.h"  // Forward declaration


template <typename T>
class WeakPtr {
public:
    template <typename U>
    friend class SharedPtr;
    template <typename U>
    friend class WeakPtr;

    template <typename U>
    friend class ControlBlockDirect;
    template <typename U>
    friend class ControlBlockIndirect;

public:

    WeakPtr() {
    }

    template <typename U>
    WeakPtr(const WeakPtr<U>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        if (control_block_) {
            control_block_->IncRefWeak();
        }
    }

    WeakPtr(const WeakPtr& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        if (control_block_) {
            control_block_->IncRefWeak();
        }
    }

    template <typename U>
    WeakPtr(WeakPtr<U>&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    WeakPtr(WeakPtr&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename U>
    WeakPtr(const SharedPtr<U>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        if (control_block_) {
            control_block_->IncRefWeak();
        }
    }

    WeakPtr(const SharedPtr<T>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        if (control_block_) {
            control_block_->IncRefWeak();
        }
    }

    template <typename U>
    WeakPtr& operator=(const WeakPtr<U>& other) {
        DecRef();
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        if (control_block_) {
            control_block_->IncRefWeak();
        }

        return *this;
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            DecRef();
            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            if (control_block_) {
                control_block_->IncRefWeak();
            }
        }

        return *this;
    }

    template <typename U>
    WeakPtr& operator=(WeakPtr<U>&& other) {
        DecRef();

        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;

        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (this != &other) {
            DecRef();

            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            other.control_block_ = nullptr;
            other.ptr_ = nullptr;
        }

        return *this;
    }

    ~WeakPtr() {
        DecRef();
    }

    void Reset() {
        DecRef();
    }
    void Swap(WeakPtr& other) {
        std::swap(control_block_, other.control_block_);
        std::swap(ptr_, other.ptr_);
    }

    size_t UseCount() const {
        return control_block_ ? control_block_->RefCount() : 0;
    }
    bool Expired() const {
        return UseCount() == 0;
    }
    SharedPtr<T> Lock() const {
        return Expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
    }

private:
    void DecRef() {
        if (control_block_) {
            control_block_->DecRefWeak();
            if (control_block_->TotalCount() == 0) {
                delete control_block_;
            }
            control_block_ = nullptr;
            ptr_ = nullptr;
        }
    }

private:
    void ForceDestruct() {
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

private:
    IControlBlock* control_block_ = nullptr;
    T* ptr_ = nullptr;
};
