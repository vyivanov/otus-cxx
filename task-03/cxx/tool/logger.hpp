#pragma once

#include <iostream>
#include <fmt/format.h>

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
