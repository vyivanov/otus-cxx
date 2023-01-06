#pragma once

#include <cstddef>
#include <vector>
#include <type_traits>

namespace Inference::Tools {

template<typename Type, typename Tag>
class FlatBuffer final {
    static_assert(std::is_arithmetic<Type>::value);
    static_assert(std::is_class<Tag>::value);

public:
    explicit FlatBuffer(size_t n);

    [[nodiscard]] bool is_empty() const noexcept;
    [[nodiscard]] size_t size() const noexcept;

    [[nodiscard]] Type& at(size_t idx) noexcept;
    [[nodiscard]] const Type& at(size_t idx) const noexcept;

    [[nodiscard]] Type* ptr() noexcept;
    [[nodiscard]] const Type* ptr() const noexcept;

private:
    std::vector<Type> m_buffer = {};
};

template<typename Type, typename Tag>
class SquareBuffer final {
    static_assert(std::is_arithmetic<Type>::value);
    static_assert(std::is_class<Tag>::value);

public:
    SquareBuffer(size_t row_n, size_t col_n);

    [[nodiscard]] bool is_empty() const noexcept;
    [[nodiscard]] size_t size() const noexcept;

    [[nodiscard]] size_t rows() const noexcept;
    [[nodiscard]] size_t cols() const noexcept;

    [[nodiscard]] Type& at(size_t row, size_t col) noexcept;
    [[nodiscard]] const Type& at(size_t row, size_t col) const noexcept;

    [[nodiscard]] Type* ptr() noexcept;
    [[nodiscard]] const Type* ptr() const noexcept;

private:
    [[nodiscard]] size_t to_idx(size_t row, size_t col) const noexcept;

    const size_t m_row_n = {};
    const size_t m_col_n = {};

    std::vector<Type> m_buffer = {};
};

}

#include "tools.inl"
