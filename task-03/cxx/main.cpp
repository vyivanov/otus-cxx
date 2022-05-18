#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <utility>

#include <fmt/format.h>
#include <mem-pool.hpp>

template<typename T>
using comparator = std::less<T>;

template<typename T, auto N>
using allocator = mem::pool::block<std::pair<T, T>, N>;

template<typename T, auto N>
using map = std::map<T, T, ::comparator<T>, ::allocator<T, N>>;

// TODO: move factorial into tool component

template<auto N>
constexpr auto factorial() {
    static_assert(N <= 12);
    return N * factorial<N-1>();
}

template<>
constexpr auto factorial<0>() {
    return 1;
}

static_assert(::factorial<5>() == 120);

auto factorial(const std::uint8_t n) noexcept {
    assert(n <= 12);
    if (not n) {
        return 1;
    }
    return n * factorial(n-1);
}

void evaluate_allocator() noexcept
{
    {
        auto a_1 = mem::pool::block<int>{};
        auto a_2 = mem::pool::block<int>{a_1};

        (void) a_2;
    }

    {
        auto a_1 = mem::pool::block<int>{};
        auto a_2 = mem::pool::block<int>{std::move(a_1)};

        (void) a_2;
    }

    {
        auto a_1 = mem::pool::block<int>{};
        auto a_2 = mem::pool::block<int>{};

        a_1 = a_2;
        a_1 = a_1;
    }

    {
        auto a_1 = mem::pool::block<int>{};
        auto a_2 = mem::pool::block<int>{};

        a_1 = std::move(a_2);
        a_1 = std::move(a_1);
    }

    auto test = ::map<int, 10>{};

    test.insert({0, ::factorial<0>()});
    test.insert({1, ::factorial<1>()});
    test.insert({2, ::factorial<2>()});
    test.insert({3, ::factorial<3>()});
    test.insert({4, ::factorial<4>()});
    test.insert({5, ::factorial<5>()});
    test.insert({6, ::factorial<6>()});
    test.insert({7, ::factorial<7>()});
    test.insert({8, ::factorial<8>()});
    test.insert({9, ::factorial<9>()});

    assert(test.size() == 10);

    for (const auto& [key, val]: test) {
        assert(::factorial(key) == val);
        std::cout
            << fmt::format("{} {}" "\n", key, val);
    }

    test.erase(0);
    test.erase(5);
    test.erase(9);

    assert(test.size() == (10-3));

    test.clear();

    assert(test.size() == 0);
}

int main()
{
    ::evaluate_allocator();
}
