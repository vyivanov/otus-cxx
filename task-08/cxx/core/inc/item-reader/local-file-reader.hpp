#pragma once

#include <core/inc/types.hpp>

#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

struct LocalFileReader: ItemReader
{

    static ItemReader::Ptr create(const FileList& file_list, const DataSize& chunk_size);
    LocalFileReader(FileList file_list, DataSize chunk_size) noexcept;

    const ItemList& enumerate() override;

    ItemInfo read_stats(ItemIdx idx) const override;
    ItemData read_chunk(ItemIdx idx) override;

};

}
