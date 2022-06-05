#pragma once

#include <list>
#include <tuple>
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

static_assert(not is_container_v<int>);
static_assert(not is_container_v<std::list  <double>>);
static_assert(not is_container_v<std::vector<double>>);
static_assert(not is_container_v<std::tuple <int>>);

static_assert(is_container_v<std::list  <int>>);
static_assert(is_container_v<std::vector<int>>);

template<typename... Tp>
struct is_tuple {
    static constexpr bool value = false;
};

template<typename... Tp>
struct is_tuple<std::tuple<Tp...>> {
    static constexpr bool value = std::conjunction_v<std::is_integral<Tp>...>;
};

template<typename... Tp>
inline constexpr bool is_tuple_v = is_tuple<Tp...>::value;

static_assert(not is_tuple_v<int>);
static_assert(not is_tuple_v<std::list  <int>>);
static_assert(not is_tuple_v<std::vector<int>>);
static_assert(not is_tuple_v<std::tuple <int, double>>);

static_assert(is_tuple_v<std::tuple<int, int>>);
static_assert(is_tuple_v<std::tuple<int, uint16_t>>);

}
