#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

#include <type-helper.hpp>

// TODO: add tuple support

namespace printer {

/**
 * @brief Print IP from integer-like input
 *
 * @param[in] ip Value to be printed
 * @return std::string
 */
template<typename T,
         typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
auto print_ip(const T& ip) -> std::string {
    constexpr auto N = sizeof(T);
    using I = decltype(sizeof(T));

    uint64_t    inp = ip;
    std::string out = {};

    for (I it{}; it < N; ++it) {
        if (it > 0) {
            out = std::to_string(inp & 0xFF) + '.' + out;
        } else {
            out = std::to_string(inp & 0xFF);
        }
        inp = (inp >> 8);
    }

    std::cout << out << std::endl;
    return out;
}

/**
 * @brief Print IP from container-like input
 *
 * @param[in] ip Value to be printed
 * @return std::string
 */
template<typename T,
         typename std::enable_if_t<is_container_v<T>, bool> = true>
auto print_ip(const T& ip) -> std::string {
    std::string out = {};
    for (const auto& octet: ip) {
        out = out + std::to_string(octet) + '.';
    }
    out.pop_back();
    std::cout << out << std::endl;
    return out;
}

/**
 * @brief Print IP from string-like input
 *
 * @param[in] ip Value to be printed
 * @return std::string
 */
auto print_ip(const std::string_view ip) -> std::string {
    std::cout << ip << std::endl;
    return std::string{ip};
}

}
