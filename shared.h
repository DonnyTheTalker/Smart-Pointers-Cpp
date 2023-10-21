#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t

template <typename T>
class SharedPtr {
public:
    template <typename U>
    friend class SharedPtr;
    template <typename U>
    friend class WeakPtr;

public:

    SharedPtr() {
    }

    SharedPtr(std::nullptr_t) {
    }

    explicit SharedPtr(T* ptr) {
        control_block_ = new ControlBlockIndirect<T>(ptr);
        control_block_->IncRefStrong();
        ptr_ = ptr;

        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            ptr_->ptr_.control_block_ = control_block_;
            ptr_->ptr_.ptr_ = ptr_;
        }
    }

    explicit SharedPtr(IControlBlock* control_block, T* ptr) {
        control_block_ = control_block;
        control_block_->IncRefStrong();
        ptr_ = ptr;

        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            ptr_->ptr_.control_block_ = control_block_;
            ptr_->ptr_.ptr_ = ptr_;
        }
    }

    template <typename Y>
    explicit SharedPtr(IControlBlock* control_block, Y* ptr) {
        control_block_ = control_block;
        control_block_->IncRefStrong();
        ptr_ = ptr;

        if constexpr (std::is_convertible_v<Y*, ESFTBase*>) {
            ptr_->ptr_.control_block_ = control_block_;
            ptr_->ptr_.ptr_ = ptr_;
        }
    }

    template <typename Y>
    explicit SharedPtr(Y* ptr) {
        control_block_ = new ControlBlockIndirect<Y>(ptr);
        control_block_->IncRefStrong();
        ptr_ = ptr;

        if constexpr (std::is_convertible_v<Y*, ESFTBase*>) {
            ptr_->ptr_.control_block_ = control_block_;
            ptr_->ptr_.ptr_ = ptr_;
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        if (control_block_) {
            control_block_->IncRefStrong();
        }
    }

    SharedPtr(const SharedPtr& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        if (control_block_) {
            control_block_->IncRefStrong();
        }
    }

    template <typename U>
    SharedPtr(SharedPtr<U>&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    SharedPtr(SharedPtr&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        control_block_ = other.control_block_;
        ptr_ = ptr;

        if (control_block_) {
            control_block_->IncRefStrong();
        }
    }

    explicit SharedPtr(const WeakPtr<T>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        if (other.Expired()) {
            throw BadWeakPtr();
        }

        if (control_block_) {
            control_block_->IncRefStrong();
        }
    }

    template <typename U>
    SharedPtr& operator=(const SharedPtr<U>& other) {
        DecRef();
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        if (control_block_) {
            control_block_->IncRefStrong();
        }
        return *this;
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            DecRef();
            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            if (control_block_) {
                control_block_->IncRefStrong();
            }
        }
        return *this;
    }

    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& other) {
        DecRef();

        control_block_ = other.control_block_;
        ptr_ = other.ptr_;

        other.control_block_ = nullptr;
        other.ptr_ = nullptr;

        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (this != &other) {
            DecRef();

            control_block_ = other.control_block_;
            ptr_ = other.ptr_;

            other.control_block_ = nullptr;
            other.ptr_ = nullptr;
        }

        return *this;
    }

    ~SharedPtr() {
        DecRef();
    }

    void Reset() {
        DecRef();
    }

    void Reset(T* ptr) {
        DecRef();

        control_block_ = new ControlBlockIndirect<T>(ptr);
        control_block_->IncRefStrong();
        ptr_ = ptr;
    }

    template <typename U>
    void Reset(U* ptr) {
        DecRef();

        control_block_ = new ControlBlockIndirect<U>(ptr);
        control_block_->IncRefStrong();
        ptr_ = ptr;
    }

    void Swap(SharedPtr& other) {
        std::swap(control_block_, other.control_block_);
        std::swap(ptr_, other.ptr_);
    }

    T* Get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    size_t UseCount() const {
        return control_block_ ? control_block_->RefCount() : 0;
    }
    explicit operator bool() const {
        return control_block_;
    }

    template <typename U>
    bool operator==(const SharedPtr<U>& right) const {
        return control_block_ == right.control_block_ && ptr_ == right.ptr_;
    }

private:
    void DecRef() {
        if (control_block_) {
            control_block_->DecRefStrong();
            if (control_block_->TotalCount() == 0) {
                delete control_block_;
            }
            control_block_ = nullptr;
            ptr_ = nullptr;
        }
    }

private:
    IControlBlock* control_block_ = nullptr;
    T* ptr_ = nullptr;
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    auto block = new ControlBlockDirect<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(block, block->GetRef());
}
