#pragma once

#include <list>
#include <type_traits>
#include <vector>

namespace printer {

template<typename T>
struct is_container {
    static constexpr bool value = false;
};

template<typename T>
struct is_container<std::list<T>> {
    static constexpr bool value = std::is_integral_v<T>;
};

template<typename T>
struct is_container<std::vector<T>> {
    static constexpr bool value = std::is_integral_v<T>;
};

template<typename T>
inline constexpr bool is_container_v = is_container<T>::value;

}
