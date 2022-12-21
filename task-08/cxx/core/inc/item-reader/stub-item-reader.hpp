#pragma once

#include <core/inc/types.hpp>

#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

struct StubItemReader: ItemReader
{

    static ItemReader::Ptr create(const DataStub& data_set, const DataSize& chunk_size);
    StubItemReader(DataStub data_set, DataSize chunk_size);

    const ItemList& enumerate() override;

    ItemInfo read_stats(ItemIdx idx) const override;
    ItemData read_chunk(ItemIdx idx) override;

};

}
