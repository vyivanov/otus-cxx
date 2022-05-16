#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <new>
#include <numeric>
#include <utility>
#include <vector>

#include <boost/format.hpp>

#ifdef LOGGING_ON
#define LOG(BOOST_FORMAT) \
    do { \
        std::cout << (BOOST_FORMAT).str() << std::endl; \
    } while (false)
#endif

namespace mem::pool {

template<typename T, auto N = 1024>
class block {
public:
    static_assert(N > 0, "block pool could not be empty");

    constexpr static auto size   = sizeof(T);
    constexpr static auto amount = N;

    using value_type = T;
    using pointer    = T*;

    template<typename U>
    struct rebind {
        using other = block<U, N>;
    };

#ifdef OPTIMIZE_ALLOCS
    block(): m_mem{init_mem()}, m_idx{init_idx()} {
        ;;;
    }

    virtual ~block() {
       m_idx.clear(); std::free(m_mem);
    }

    // TODO: support copy and move

    block(const block&) = delete;
    block(block&&)      = delete;

    block& operator=(const block&) = delete;
    block& operator=(block&&)      = delete;
#endif

    // O(N)
    block::pointer allocate(const std::size_t num) {
    #ifdef LOGGING_ON
        LOG(boost::format {"%1% | num = %2%"} % (__PRETTY_FUNCTION__) % num);
    #endif
    assert(num == 1);  // TODO: support continuous reservation
    #ifdef OPTIMIZE_ALLOCS
        block::pointer pbegin = nullptr;
        for (auto ptr = m_mem; ptr < (m_mem + block::amount); ++ptr) {
            if (m_idx.at(ptr_to_idx(ptr)) == block::occupation::take) {
                continue;
            }
            pbegin = ptr; break;
        }
        if (not pbegin) {
            throw std::bad_alloc{};
        }
        // TODO: find pend
        assert(m_idx.at(ptr_to_idx(pbegin)) == block::occupation::free);
        m_idx.at(ptr_to_idx(pbegin)) = block::occupation::take;
        return pbegin;
    #else
        return (block::pointer) std::calloc(num, block::size);
    #endif
    }

    // O(N)
    void deallocate(const block::pointer ptr, const std::size_t num) {
    #ifdef LOGGING_ON
        LOG(boost::format {"%1% | ptr = %2%, num = %3%"} % (__PRETTY_FUNCTION__) % ptr % num);
    #endif
    assert(ptr and num);
    #ifdef OPTIMIZE_ALLOCS
        for (auto it = ptr; it < (ptr + num); ++it) {
            assert(m_idx.at(ptr_to_idx(it)) == block::occupation::take);
            m_idx.at(ptr_to_idx(it)) = block::occupation::free;
        }
    #else
        std::free(ptr);
    #endif
    }

    // O(1)
    template<typename U, typename... Args>
    void construct(U* const ptr, Args&&... args) {
    #ifdef LOGGING_ON
        LOG(boost::format {"%1% | ptr = %2%"} % (__PRETTY_FUNCTION__) % ptr);
    #endif
        new (ptr) U(std::forward<Args>(args)...);
    }

    // O(1)
    template<typename U>
    void destroy(U* const ptr) {
    #ifdef LOGGING_ON
        LOG(boost::format {"%1% | ptr = %2%"} % (__PRETTY_FUNCTION__) % ptr);
    #endif
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

    block::pointer const m_mem;
    block::index         m_idx;
#endif
};

static_assert(pool::block<int, 1>::size   == sizeof(int));
static_assert(pool::block<int, 1>::amount == 1);

}
