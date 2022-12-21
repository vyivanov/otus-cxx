#include <core/inc/data-hasher/boost-data-hasher.hpp>

#include <memory>
#include <core/inc/types.hpp>

#include <boost/functional/hash.hpp>

#include <core/inc/data-hasher/data-hasher-iface.hpp>

namespace ddt {

auto BoostDataHasher::create() -> DataHasher::Ptr
{
    return std::make_unique<BoostDataHasher>();
}

auto BoostDataHasher::combine(const ItemData& chunk, const HashData& seed) -> HashData
{
    if (chunk.empty())
    {
        throw std::length_error{std::string{}};
    }

    auto mutable_seed = seed;
    boost::hash_range(mutable_seed, chunk.cbegin(), chunk.cend());

    return mutable_seed;
}

}
