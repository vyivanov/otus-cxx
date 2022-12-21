#pragma once

#include <core/inc/types.hpp>

#include <core/inc/data-hasher/data-hasher-iface.hpp>
#include <core/inc/dup-detector/dup-detector-iface.hpp>
#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

struct DupDetectorImpl: DupDetector
{

    static DupDetector::Ptr create(const ItemReader::Ptr& reader, const DataHasher::Ptr& hasher);
    DupDetectorImpl(ItemReader::Ptr reader, DataHasher::Ptr hasher) noexcept;

    void detect() noexcept override;
    const ItemDups& result() const noexcept override;

};

}
