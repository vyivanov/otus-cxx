#include <core/inc/item-reader/local-file-reader.hpp>

#include <memory>
#include <core/inc/types.hpp>

#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

auto LocalFileReader::create(const FileList& file_list, const DataSize& chunk_size) -> ItemReader::Ptr
{
    return std::make_unique<LocalFileReader>(file_list, chunk_size);
}

LocalFileReader::LocalFileReader(FileList file_list, DataSize chunk_size) noexcept
{
    static_cast<void>(file_list);
    static_cast<void>(chunk_size);
}

auto LocalFileReader::enumerate() -> const ItemList&
{
    static auto stub = ItemList{};

    return stub;
}

auto LocalFileReader::read_stats(const ItemIdx idx) const -> ItemInfo
{
    static_cast<void>(idx);

    return ItemInfo{};
}

auto LocalFileReader::read_chunk(const ItemIdx idx) -> ItemData
{
    static_cast<void>(idx);

    return ItemData{};
}

}
