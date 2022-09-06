#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

#include <iface.hpp>
#include <scalar.hpp>
#include <store.hpp>

namespace tensor {

template<typename T, T Default = T{}>
class Vector
    : public ItemStore<T>
    , public std::enable_shared_from_this<Vector<T, Default>>
{

public:

    explicit Vector(const std::size_t height, typename ItemStore<T>::ParentOpt parent = {}) noexcept
        : ItemStore<T>{std::move(parent)}
        , m_height{height}
    {
        assert(m_height > 0);
    }

    ITensor<T>& operator[](const typename ITensor<T>::Idx idx) override
    {
        if (not this->has(idx))
        {
            this->insert(idx, create_child(idx));
        }

        const auto child_itm = this->item(idx);
        const auto child_ptr = std::get<typename ItemStore<T>::Ptr>(child_itm);

        return (*child_ptr);
    }

    void operator=(const T) override
    {
        throw Exception::NoValueAssignment{};
    }

    operator T() override
    {
        throw Exception::NoValueConversion{};
    }

private:

    auto create_child(const typename ITensor<T>::Idx idx) -> typename ItemStore<T>::Ptr
    {
        const auto parent = typename ItemStore<T>::Parent(
        {
            this->shared_from_this(), idx
        });

        if (m_height > 1)
        {
            return std::make_unique<Vector<T, Default>>((m_height - 1), parent);
        }
        else
        {
            return std::make_unique<Scalar<T, Default>>(parent);
        }
    }

    const std::size_t m_height = {};

};

}
