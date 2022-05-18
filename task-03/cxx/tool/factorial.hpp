#pragma once

#include <cassert>

namespace tool {

template<typename T>
constexpr auto factorial(const T inp) noexcept {
    assert((0 <= inp) and (inp <= 12));
    if (not inp) {
        return 1;
    }
    const auto out = inp * factorial(inp - 1);
    return out;
}

static_assert(factorial(5) == 120);

}
