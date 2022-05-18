#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <utility>

#include <fmt/format.h>

#include <mem-pool.hpp>
#include <factorial.hpp>

template<typename T>
using comparator = std::less<T>;

template<typename T, auto N>
using allocator = mem::pool::block<std::pair<T, T>, N>;

template<typename T, auto N>
using map = std::map<T, T, ::comparator<T>, ::allocator<T, N>>;

void evaluate_allocator() noexcept
{
    {
        auto a_1 = mem::pool::block<int>{};
        auto a_2 = mem::pool::block<int>{a_1};

        assert(a_1 == a_2);

        a_2.allocate(1);

        assert(a_1 != a_2);

        a_1 = a_2;

        assert(a_1 == a_2);
    }

    {
        auto a_1 = mem::pool::block<int>{};
        auto a_2 = mem::pool::block<int>{std::move(a_1)};

        (void) a_2;
    }

    {
        auto a_1 = mem::pool::block<int>{};
        auto a_2 = mem::pool::block<int>{};

        a_2 = std::move(a_1);
    }

    auto test = ::map<int, 10>{};

    test.insert({0, tool::factorial(0)});
    test.insert({1, tool::factorial(1)});
    test.insert({2, tool::factorial(2)});
    test.insert({3, tool::factorial(3)});
    test.insert({4, tool::factorial(4)});
    test.insert({5, tool::factorial(5)});
    test.insert({6, tool::factorial(6)});
    test.insert({7, tool::factorial(7)});
    test.insert({8, tool::factorial(8)});
    test.insert({9, tool::factorial(9)});

    assert(test.size() == 10);

    for (const auto& [key, val]: test) {
        assert(tool::factorial(key) == val);
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
