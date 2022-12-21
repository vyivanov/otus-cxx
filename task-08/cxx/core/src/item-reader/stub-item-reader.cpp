#include <core/inc/item-reader/stub-item-reader.hpp>

#include <memory>
#include <core/inc/types.hpp>

#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

auto StubItemReader::create(const DataStub& data_set, const DataSize& chunk_size) -> ItemReader::Ptr
{
    return std::make_unique<StubItemReader>(data_set, chunk_size);
}

StubItemReader::StubItemReader(DataStub data_set, DataSize chunk_size)
{
    static_cast<void>(data_set);
    static_cast<void>(chunk_size);
}

auto StubItemReader::enumerate() -> const ItemList&
{
    static auto stub = ItemList{};

    return stub;
}

auto StubItemReader::read_stats(const ItemIdx idx) const -> ItemInfo
{
    static_cast<void>(idx);

    return ItemInfo{};
}

auto StubItemReader::read_chunk(const ItemIdx idx) -> ItemData
{
    static_cast<void>(idx);

    return ItemData{};
}

}
