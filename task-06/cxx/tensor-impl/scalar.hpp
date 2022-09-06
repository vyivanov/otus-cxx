#pragma once

#include <memory>
#include <optional>
#include <variant>

#include <iface.hpp>
#include <store.hpp>

namespace tensor {

template<typename T, T Default = T{}>
class Scalar: public ItemStore<T>
{

public:

    explicit Scalar(typename ItemStore<T>::ParentOpt parent = {}) noexcept
        : ItemStore<T>{std::move(parent)}
    {
        this->insert(SCALAR_IDX, Default);
    }

    ITensor<T>& operator[](const typename ITensor<T>::Idx) override
    {
        throw Exception::NoIndexSubscription{};
    }

    void operator=(const T val) override
    {
        if ((val == Default) and this->parent().has_value())
        {
            this->remove(SCALAR_IDX);
        }
        else
        {
            this->insert(SCALAR_IDX, val);
        }
    }

    operator T() override
    {
        const auto scalar_itm = this->item(SCALAR_IDX);
        const auto scalar_val = std::get<T>(scalar_itm);

        if ((scalar_val == Default) and this->parent().has_value())
        {
            this->remove(SCALAR_IDX);
        }

        return scalar_val;
    }

private:

    static constexpr typename ITensor<T>::Idx SCALAR_IDX{0};
};

}
