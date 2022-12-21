#pragma once

#include <memory>
#include <core/inc/types.hpp>

namespace ddt {

struct ItemReader
{

    using Ptr = std::shared_ptr<ItemReader>;

    virtual const ItemList& enumerate() = 0;

    virtual ItemInfo read_stats(ItemIdx idx) const = 0;
    virtual ItemData read_chunk(ItemIdx idx) = 0;

    virtual ~ItemReader() = default;

};

}
