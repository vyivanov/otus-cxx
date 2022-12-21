#include <core/inc/data-hasher/boost-data-hasher.hpp>

#include <memory>
#include <core/inc/types.hpp>

#include <core/inc/data-hasher/data-hasher-iface.hpp>

namespace ddt {

auto BoostDataHasher::create() -> DataHasher::Ptr
{
    return std::make_unique<BoostDataHasher>();
}

auto BoostDataHasher::combine(const ItemData& chunk, const HashData& seed) -> HashData
{
    static_cast<void>(chunk);
    static_cast<void>(seed);

    return HashData{};
}

}
