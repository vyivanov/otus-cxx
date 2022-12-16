#include <core/inc/dup-detector/dup-detector-impl.hpp>

#include <algorithm>
#include <cassert>
#include <core/inc/types.hpp>
#include <iterator>
#include <memory>
#include <optional>
#include <unordered_set>
#include <utility>

#include <core/inc/data-hasher/data-hasher-iface.hpp>
#include <core/inc/dup-detector/dup-detector-iface.hpp>
#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

auto DupDetectorImpl::create(const ItemReader::Ptr& reader, const DataHasher::Ptr& hasher) -> DupDetector::Ptr
{
    return std::make_unique<DupDetectorImpl>(reader, hasher);
}

DupDetectorImpl::DupDetectorImpl(ItemReader::Ptr reader, DataHasher::Ptr hasher) noexcept
    : m_reader{std::move(reader)}
    , m_hasher{std::move(hasher)}
    , m_done(false)
    , m_cache(std::nullopt)
    , m_group_map(nullptr)
{
    assert(m_reader and m_hasher);  // TODO: Replace assertion with gsl::not_null
    assert(m_reader->enumerate().size() > 1);

    assert(not m_done);
    assert(not m_cache);
    assert(not m_group_map);
}

auto DupDetectorImpl::detect() noexcept -> void
{
    if (m_done)
    {
        return;
    }

    m_group_map = init();
    assert(m_group_map);

    while (not m_done)
    {
        auto iteration = std::make_unique<
            decltype(m_group_map)::element_type>(m_group_map->size());

        auto done_all_items  = true;
        auto done_all_groups = true;

        for_each_group([&, this](const auto& item_begin, const auto& item_end)
        {
            if (std::distance(item_begin, item_end) > 1)
            {
                done_all_groups = false;

                std::transform(item_begin, item_end, std::inserter((*iteration), (*iteration).end()),
                [&, this](const auto& pair)
                {
                    const auto& [group, item] = pair;

                    if (m_reader->read_stats(item).rest_size)
                    {
                        done_all_items = false;

                        assert(group.has_value());
                        const Group::Idx new_group = m_hasher->combine(m_reader->read_chunk(item), group.value());

                        return std::make_pair(new_group, item);
                    }
                    else
                    {
                        return std::make_pair(group, item);
                    }
                });
            }
            else
            {
                iteration->emplace(std::make_pair(item_begin->first, item_begin->second));
            }
        });

        m_group_map = std::move(iteration);
        m_done = (done_all_items or done_all_groups);
    }
}

auto DupDetectorImpl::result() const noexcept -> const ItemDups&
{
    if (not m_done or m_cache)
    {
        return m_cache;
    }

    assert(m_done and not m_cache);
    m_cache = ItemDups::value_type(/* FIXME: Try to reserve space */);

    for_each_group([this](const auto& item_begin, const auto& item_end)
    {
        auto items = ItemList(std::distance(item_begin, item_end));
        std::transform(item_begin, item_end, items.begin(), [](const auto& pair){ return pair.second; });

        m_cache->push_back(std::move(items));
    });

    return m_cache;
}

auto DupDetectorImpl::init() noexcept -> DupDetectorImpl::Group::Ptr
{
    assert(m_reader and m_hasher);

    auto group_map = std::make_unique<
        decltype(init())::element_type>(m_reader->enumerate().size());

    for (const auto& item: m_reader->enumerate())
    {
        const auto group = [this, &item]() -> Group::Idx
        {
            if (m_reader->read_stats(item).orig_size)
            {
                return m_hasher->combine(m_reader->read_chunk(item), HashData{0});
            }
            else
            {
                return std::nullopt;
            }
        }();

        group_map->emplace(std::make_pair(group, item));
    }

    assert(group_map->size() == m_reader->enumerate().size());
    return group_map;
}

auto DupDetectorImpl::for_each_group(const ProcessCb& process) const noexcept -> std::size_t
{
    assert(m_group_map);

    auto tracer = std::unordered_set<
        decltype(m_group_map)::element_type::key_type>(m_group_map->size());

    for (const auto& [group, item]: (*m_group_map))
    {
        if (tracer.find(group) == tracer.cend())
        {
            const auto [item_begin, item_end] = m_group_map->equal_range(group);
            assert(std::distance(item_begin, item_end) > 0);

            if (process)
            {
                process(item_begin, item_end);
            }

            [[maybe_unused]]
            const auto [it, ok] = tracer.insert(group);
            assert(ok);
        }
    }

    assert(tracer.size() <= m_group_map->size());
    return tracer.size();
}

}
