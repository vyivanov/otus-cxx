#pragma once

#include <core/inc/types.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

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

private:

    const ItemReader::Ptr m_reader;
    const DataHasher::Ptr m_hasher;

    bool m_done;
    mutable ItemDups m_cache;

    struct Group
    {
        using Idx = std::optional<HashData>;
        using Map = std::unordered_multimap<Idx, ItemIdx>;
        using Ptr = std::unique_ptr<Map>;

        struct Item
        {
            using ItBegin = Map::const_iterator;
            using ItEnd   = Map::const_iterator;
        };
    };

    Group::Ptr m_group_map;
    Group::Ptr init() noexcept;

    using ProcessCb = std::function<void(const Group::Item::ItBegin&, const Group::Item::ItEnd&)>;
    std::size_t for_each_group(const ProcessCb& process = {}) const noexcept;

};

}
