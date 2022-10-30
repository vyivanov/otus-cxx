#include <core/inc/item-reader/stub-item-reader.hpp>

#include <algorithm>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <core/inc/types.hpp>

#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

auto StubItemReader::create(const DataStub& data_set, const DataSize& chunk_size) -> ItemReader::Ptr
{
    return std::make_unique<StubItemReader>(data_set, chunk_size);
}

StubItemReader::StubItemReader(DataStub data_set, DataSize chunk_size): m_chunk_size(chunk_size)
{
    if (data_set.size() < 2 or chunk_size == 0)
    {
        throw std::length_error{std::string{}};
    }

    m_dataset.reserve(data_set.size());
    m_item_list.reserve(data_set.size());

    auto idx = ItemIdx{0};
    for (const auto& data: data_set)
    {
        m_dataset.insert({idx, {data, {.orig_size = data.size(), .rest_size = data.size()}}});
        m_item_list.push_back(idx);

        ++idx;
    }

    assert(data_set.size() == m_dataset.size());
    assert(data_set.size() == m_item_list.size());
}

auto StubItemReader::enumerate() -> const ItemList&
{
    return m_item_list;
}

auto StubItemReader::read_stats(const ItemIdx idx) const -> ItemInfo
{
    const auto& [data, info] = m_dataset.at(idx);
    return info;
}

auto StubItemReader::read_chunk(const ItemIdx idx) -> ItemData
{
    auto& [data, info] = m_dataset.at(idx);

    if (info.rest_size == 0)
    {
        throw std::underflow_error{std::string{}};
    }

    assert(info.rest_size > 0);
    assert(info.rest_size <= info.orig_size);

    const auto idx_begin = info.orig_size - info.rest_size;
    const auto idx_end   = std::min(idx_begin + m_chunk_size, info.orig_size);

    assert(idx_end > idx_begin);
    assert(idx_end - idx_begin <= m_chunk_size);

    auto chunk = ItemData(idx_end - idx_begin);
    std::copy(data.cbegin() + static_cast<decltype(data.cbegin())::difference_type>(idx_begin),
              data.cbegin() + static_cast<decltype(data.cbegin())::difference_type>(idx_end),
              chunk.begin());

    assert(chunk.size() <= info.rest_size);
    info.rest_size -= chunk.size();

    return chunk;
}

}
