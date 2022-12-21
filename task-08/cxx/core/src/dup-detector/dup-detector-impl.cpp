#include <core/inc/dup-detector/dup-detector-impl.hpp>

#include <memory>
#include <core/inc/types.hpp>

#include <core/inc/data-hasher/data-hasher-iface.hpp>
#include <core/inc/dup-detector/dup-detector-iface.hpp>
#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

auto DupDetectorImpl::create(const ItemReader::Ptr& reader, const DataHasher::Ptr& hasher) -> DupDetector::Ptr
{
    return std::make_unique<DupDetectorImpl>(reader, hasher);
}

DupDetectorImpl::DupDetectorImpl(ItemReader::Ptr reader, DataHasher::Ptr hasher) noexcept
{
    static_cast<void>(reader);
    static_cast<void>(hasher);
}

auto DupDetectorImpl::detect() noexcept -> void
{

}

auto DupDetectorImpl::result() const noexcept -> const ItemDups&
{
    static auto stub = ItemDups{};

    return stub;
}

}
