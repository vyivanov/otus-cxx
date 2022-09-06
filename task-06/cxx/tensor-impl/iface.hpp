#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>

namespace tensor {

struct Exception: std::exception
{
    struct NoIndexSubscription: std::invalid_argument
    {
        NoIndexSubscription()
            : std::invalid_argument{"scalar does not support subscription by index"}
        {}
    };

    struct NoValueAssignment: std::invalid_argument
    {
        NoValueAssignment()
            : std::invalid_argument{"vector does not support assignment from value"}
        {}
    };

    struct NoValueConversion: std::invalid_argument
    {
        NoValueConversion()
            : std::invalid_argument{"vector does not support conversion to value"}
        {}
    };
};

template<typename T>
class ITensor
{

public:

    using Idx = std::size_t;
    using Ptr = std::shared_ptr<ITensor<T>>;

    virtual std::size_t size() const = 0;
    virtual ITensor<T>& operator[](Idx) = 0;
    virtual void operator=(T) = 0;
    virtual operator T() = 0;

};

}
