#pragma once

#include <core/inc/types.hpp>
#include <fstream>

#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

struct LocalFileReader: ItemReader
{

    static ItemReader::Ptr create(const FileList& file_list, const DataSize& chunk_size);
    LocalFileReader(FileList file_list, DataSize chunk_size) noexcept;

    const ItemList& enumerate() override;

    ItemInfo read_stats(ItemIdx idx) const override;
    ItemData read_chunk(ItemIdx idx) override;

private:

    struct FileInfo
    {
        std::filesystem::path path;
        std::ifstream fptr;
    };

    struct Info
    {
        FileInfo file;
        ItemInfo item;
    };

    using InfoList = std::vector<Info>;

    const FileList m_file_list;
    const DataSize m_chunk_size;

    ItemList m_items;
    InfoList m_infos;

    bool is_inited() const noexcept;
    void init();

};

}
