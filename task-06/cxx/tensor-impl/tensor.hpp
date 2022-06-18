#pragma once

#include <cstddef>
#include <memory>

#include <iface.hpp>
#include <vector.hpp>
#include <scalar.hpp>

namespace tensor {

template<typename T, T Default = T{}, std::size_t Rank = 3>
class Tensor final: public ITensor<T>
{

public:

    std::size_t size() const override
    {
        return m_root->size();
    }

    ITensor<T>& operator[](const typename ITensor<T>::Idx idx) override
    {
        return m_root->operator[](idx);
    }

    void operator=(const T val) override
    {
        return m_root->operator=(val);
    }

    operator T() override
    {
        return m_root->operator T();
    }

private:

    constexpr auto initialize() -> typename ITensor<T>::Ptr
    {
        if constexpr (Rank > 0)
        {
            return std::make_unique<Vector<T, Default>>(Rank);
        }
        else
        {
            return std::make_unique<Scalar<T, Default>>();
        }
    }

    const typename ITensor<T>::Ptr m_root = initialize();

};

}
