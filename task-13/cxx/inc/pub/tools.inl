#pragma once

#include <cassert>
#include <cstddef>

template<typename Type, typename Tag>
FlatBuffer<Type, Tag>::FlatBuffer(const size_t n)
{
    assert(n);
    m_buffer.resize(n, Type{});
}

template<typename Type, typename Tag>
bool FlatBuffer<Type, Tag>::is_empty() const noexcept
{
    return size() == 0;
}

template<typename Type, typename Tag>
size_t FlatBuffer<Type, Tag>::size() const noexcept
{
    return m_buffer.size();
}

template<typename Type, typename Tag>
Type& FlatBuffer<Type, Tag>::at(const size_t idx) noexcept
{
    assert(idx < size());
    return m_buffer[idx];
}

template<typename Type, typename Tag>
const Type& FlatBuffer<Type, Tag>::at(const size_t idx) const noexcept
{
    assert(idx < size());
    return m_buffer[idx];
}

template<typename Type, typename Tag>
Type* FlatBuffer<Type, Tag>::ptr() noexcept
{
    return m_buffer.data();
}

template<typename Type, typename Tag>
const Type* FlatBuffer<Type, Tag>::ptr() const noexcept
{
    return m_buffer.data();
}

template<typename Type, typename Tag>
SquareBuffer<Type, Tag>::SquareBuffer(const size_t row_n, const size_t col_n)
    : m_row_n(row_n)
    , m_col_n(col_n)
{
    assert(m_row_n and m_col_n);
    m_buffer.resize((m_row_n * m_col_n), Type{});
}

template<typename Type, typename Tag>
bool SquareBuffer<Type, Tag>::is_empty() const noexcept
{
    return size() == 0;
}

template<typename Type, typename Tag>
size_t SquareBuffer<Type, Tag>::size() const noexcept
{
    return m_buffer.size();
}

template<typename Type, typename Tag>
size_t SquareBuffer<Type, Tag>::rows() const noexcept
{
    return m_row_n;
}

template<typename Type, typename Tag>
size_t SquareBuffer<Type, Tag>::cols() const noexcept
{
    return m_col_n;
}

template<typename Type, typename Tag>
Type& SquareBuffer<Type, Tag>::at(const size_t row, const size_t col) noexcept
{
    return m_buffer[to_idx(row, col)];
}

template<typename Type, typename Tag>
const Type& SquareBuffer<Type, Tag>::at(const size_t row, const size_t col) const noexcept
{
    return m_buffer[to_idx(row, col)];
}

template<typename Type, typename Tag>
Type* SquareBuffer<Type, Tag>::ptr() noexcept
{
    return m_buffer.data();
}

template<typename Type, typename Tag>
const Type* SquareBuffer<Type, Tag>::ptr() const noexcept
{
    return m_buffer.data();
}

template<typename Type, typename Tag>
size_t SquareBuffer<Type, Tag>::to_idx(const size_t row, const size_t col) const noexcept
{
    assert((row < m_row_n) and (col < m_col_n));
    return (row * m_col_n + col);
}
