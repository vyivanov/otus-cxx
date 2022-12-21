#pragma once

#include <memory>
#include <core/inc/types.hpp>

namespace ddt {

struct DataHasher
{

    using Ptr = std::shared_ptr<DataHasher>;

    virtual HashData combine(const ItemData& chunk, const HashData& seed) = 0;

    virtual ~DataHasher() = default;

};

}
