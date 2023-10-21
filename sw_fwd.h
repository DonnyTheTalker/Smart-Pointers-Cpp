#pragma once

#include <exception>
#include <memory>

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

class ESFTBase {};

class IControlBlock {
public:
    void IncRefStrong() {
        ++counter_strong_;
        ++counter_total_;
    }
    void DecRefStrong() {
        --counter_strong_;
        --counter_total_;
        if (counter_strong_ == 0) {
            Destroy();
        }
    }

    void IncRefWeak() {
        ++counter_total_;
    }

    void DecRefWeak() {
        --counter_total_;
    }

    size_t RefCount() {
        return counter_strong_;
    }

    size_t TotalCount() {
        return counter_total_;
    }

    virtual ~IControlBlock() {
    }

private:
    virtual void Destroy() = 0;

private:
    size_t counter_strong_ = 0;
    size_t counter_total_ = 0;
};

template <typename T>
class ControlBlockIndirect : public IControlBlock {
public:
    ControlBlockIndirect(T* ptr) : ptr_(ptr) {
    }

    T* GetRef() {
        return ptr_;
    }

private:
    void Destroy() override {
        if (ptr_) {
            if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
                ptr_->ptr_.ForceDestruct();
            }
            delete ptr_;
        }
        ptr_ = nullptr;
    }

private:
    T* ptr_;
};

template <typename T>
class ControlBlockDirect : public IControlBlock {
public:
    template <typename... Args>
    ControlBlockDirect(Args&&... args) {
        ::new (&data_) T(std::forward<Args>(args)...);
    }

    T* GetRef() {
        return reinterpret_cast<T*>(&data_);
    }

private:
    void Destroy() override {
        if constexpr (std::is_convertible_v<T*, ESFTBase*>) {
            (reinterpret_cast<T*>(&data_))->ptr_.ForceDestruct();
        }
        std::destroy_at(std::launder(reinterpret_cast<T*>(&data_)));
    }

private:
    std::aligned_storage_t<sizeof(T), alignof(T)> data_;
};


template <typename T>
class EnableSharedFromThis : public ESFTBase {
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
    SharedPtr<T> SharedFromThis() {
        return SharedPtr<T>(ptr_);
    }
    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr<const T>(ptr_);
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return WeakPtr<T>(ptr_);
    }
    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(ptr_);
    }

private:
    WeakPtr<T> ptr_;
};
