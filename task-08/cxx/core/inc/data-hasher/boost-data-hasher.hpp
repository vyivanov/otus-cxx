#pragma once

#include <core/inc/data-hasher/data-hasher-iface.hpp>

namespace ddt {

struct BoostDataHasher: DataHasher
{

    static DataHasher::Ptr create();

    HashData combine(const ItemData& chunk, const HashData& seed) override;

};

}
