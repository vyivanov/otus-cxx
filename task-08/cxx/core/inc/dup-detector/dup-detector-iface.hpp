#pragma once

#include <memory>
#include <core/inc/types.hpp>

namespace ddt {

struct DupDetector
{

    using Ptr = std::shared_ptr<DupDetector>;

    virtual void detect() noexcept = 0;
    virtual const ItemDups& result() const noexcept = 0;

    virtual ~DupDetector() = default;

};

}
