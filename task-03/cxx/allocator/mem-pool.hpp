#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <numeric>
#include <utility>
#include <vector>

#include <fmt/format.h>

// TODO: add tests
// TODO: implement memory extension
// TODO: move LOG into tool component

#ifdef LOGGING_ON
#define LOG(FORMAT, ...) \
    do { \
        const auto pfx = fmt::format("{} at {}:{}", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
        const auto msg = fmt::format(FORMAT, ##__VA_ARGS__); \
        std::cout \
            << pfx << " | " + msg \
            << std::endl; \
    } while (false)
#else
#define LOG(FORMAT, ...) \
    do { \
        ;;;;; \
    } while (false)
#endif

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

#ifdef OPTIMIZE_ALLOCS
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

    virtual ~block() {
        LOG("this = {}", static_cast<void*>(this));
        std::free(m_mem);
    }
#endif

    // O(N)
    block::pointer allocate(const std::size_t num) {
        LOG("num = {}", num);
        assert((num == 1) and "block pool permits allocate one element at a time");
    #ifdef OPTIMIZE_ALLOCS
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
    #else
        return (block::pointer) std::calloc(1, block::size);
    #endif
    }

    // O(1)
    void deallocate(const block::pointer ptr, const std::size_t num) noexcept {
        LOG("ptr = {}, num = {}", static_cast<void*>(ptr), num);
        assert((num == 1) and "block pool permits deallocate one element at a time");
    #ifdef OPTIMIZE_ALLOCS
        assert((m_mem <= ptr) and (ptr < (m_mem + block::amount)) and "this memory is not owned by block pool");
        assert(m_idx.at(ptr_to_idx(ptr)) == block::occupation::take);
        m_idx.at(ptr_to_idx(ptr)) = block::occupation::free;
    #else
        std::free(ptr);
    #endif
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

#ifdef OPTIMIZE_ALLOCS
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
#endif
};

static_assert(pool::block<int, 1>::size   == sizeof(int));
static_assert(pool::block<int, 1>::amount == 1);

}
