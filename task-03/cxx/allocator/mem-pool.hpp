#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <numeric>
#include <utility>
#include <vector>

#include <logger.hpp>

// TODO: add tests
// TODO: implement memory extension

namespace mem::pool {

template<typename T, auto N = 1024>
class block {
public:
    static_assert(N > 0, "block pool should not be empty");

    constexpr static auto size   = sizeof(T);
    constexpr static auto amount = N;
    constexpr static auto bytes  = (amount * size);

    using value_type = T;
    using pointer    = T*;

    template<typename U>
    struct rebind {
        using other = block<U, N>;
    };

    block(): m_mem{init_mem()}, m_idx{init_idx()} {
        LOG("this = {}", static_cast<void*>(this));
    }

    block(const block& rhs): block() {
        LOG("this = {}, rhs = {}", static_cast<void*>(this), static_cast<const void*>(&rhs));
        std::memcpy(m_mem, rhs.m_mem, block::bytes);
        m_idx = rhs.m_idx;
    }

    block(block&& rhs) noexcept {
        LOG("this = {}, rhs = {}", static_cast<void*>(this), static_cast<void*>(&rhs));
        m_mem = std::exchange(rhs.m_mem, nullptr);
        m_idx = std::move(rhs.m_idx);
    }

    block& operator=(const block& rhs) {
        LOG("this = {}, rhs = {}", static_cast<void*>(this), static_cast<const void*>(&rhs));
        if (&rhs != this) {
            std::memcpy(m_mem, rhs.m_mem, block::bytes);
            m_idx = rhs.m_idx;
        }
        return (*this);
    }

    block& operator=(block&& rhs) noexcept {
        LOG("this = {}, rhs = {}", static_cast<void*>(this), static_cast<void*>(&rhs));
        if (&rhs != this) {
            std::free(m_mem);
            m_mem = std::exchange(rhs.m_mem, nullptr);
            m_idx = std::move(rhs.m_idx);
        }
        return (*this);
    }

    bool operator==(const block& rhs) noexcept {
        return std::memcmp(m_mem, rhs.m_mem, block::bytes) == 0 and (m_idx == rhs.m_idx);
    }

    bool operator!=(const block& rhs) noexcept {
        return not (*this == rhs);
    }

    virtual ~block() {
        LOG("this = {}", static_cast<void*>(this));
        std::free(m_mem);
    }

    // O(N)
    block::pointer allocate(const std::size_t num) {
        LOG("num = {}", num);
        assert((num == 1) and "block pool permits allocate one element at a time");
        const auto pblock = [this]() -> block::pointer {
            for (auto ptr = m_mem; ptr < (m_mem + block::amount); ++ptr) {
                if (m_idx.at(ptr_to_idx(ptr)) == block::occupation::free) {
                    return ptr;
                }
            }
            return nullptr;
        }();
        if (not pblock) {
            throw std::bad_alloc{};
        }
        m_idx.at(ptr_to_idx(pblock)) = block::occupation::take;
        assert(pblock >= m_mem);
        return pblock;
    }

    // O(1)
    void deallocate(const block::pointer ptr, const std::size_t num) noexcept {
        LOG("ptr = {}, num = {}", static_cast<void*>(ptr), num);
        assert((num == 1) and "block pool permits deallocate one element at a time");
        assert((m_mem <= ptr) and (ptr < (m_mem + block::amount)) and "this memory is not owned by block pool");
        assert(m_idx.at(ptr_to_idx(ptr)) == block::occupation::take);
        m_idx.at(ptr_to_idx(ptr)) = block::occupation::free;
    }

    // O(1)
    template<typename U, typename... Args>
    void construct(U* const ptr, Args&&... args) {
        LOG("ptr = {}", static_cast<void*>(ptr));
        new (ptr) U(std::forward<Args>(args)...);
    }

    // O(1)
    template<typename U>
    void destroy(U* const ptr) noexcept {
        LOG("ptr = {}", static_cast<void*>(ptr));
        ptr->~U();
    }

private:
    enum class occupation : bool { free = true, take = false };
    using index = std::vector<block::occupation>;

    static auto init_mem() {
        return (block::pointer) std::calloc(block::amount, block::size);
    }

    static auto init_idx() {
        return block::index(block::amount, block::occupation::free);
    }

    auto ptr_to_idx(const block::pointer ptr) noexcept {
        assert(ptr >= m_mem);
        return (ptr - m_mem);
    }

    block::pointer m_mem;
    block::index   m_idx;
};

static_assert(pool::block<int, 1>::size   == sizeof(int));
static_assert(pool::block<int, 1>::amount == 1);

}
