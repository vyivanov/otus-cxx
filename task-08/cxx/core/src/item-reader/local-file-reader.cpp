#include <core/inc/item-reader/local-file-reader.hpp>

#include <cassert>
#include <core/inc/types.hpp>
#include <memory>
#include <type_traits>

#include <core/inc/item-reader/item-reader-iface.hpp>

namespace ddt {

auto LocalFileReader::create(const FileList& file_list, const DataSize& chunk_size) -> ItemReader::Ptr
{
    return std::make_unique<LocalFileReader>(file_list, chunk_size);
}

LocalFileReader::LocalFileReader(FileList file_list, DataSize chunk_size) noexcept
    : m_file_list(std::move(file_list))
    , m_chunk_size(chunk_size)
{
    assert(m_file_list.size() > 1);
    assert(m_chunk_size > 0);
}

auto LocalFileReader::enumerate() -> const ItemList&
{
    if (not is_inited())
    {
        init();
    }

    return m_items;
}

auto LocalFileReader::read_stats(const ItemIdx idx) const -> ItemInfo
{
    if (not is_inited())
    {
        throw std::logic_error(std::string{});
    }

    return m_infos.at(idx).item;
}

auto LocalFileReader::read_chunk(const ItemIdx idx) -> ItemData
{
    if (not is_inited())
    {
        throw std::logic_error(std::string{});
    }

    auto& file = m_infos.at(idx).file;
    auto& item = m_infos.at(idx).item;

    if (not item.rest_size)
    {
        throw std::underflow_error(std::string{file.path});
    }

    assert(item.rest_size > 0);
    assert(item.rest_size <= item.orig_size);

    auto chunk = ItemData(std::min(m_chunk_size, item.rest_size));

    using char_ptr_t  = std::add_pointer<decltype(file.fptr)::char_type>::type;
    using data_size_t = std::streamsize;

    file.fptr.read(reinterpret_cast<char_ptr_t>(chunk.data()), static_cast<data_size_t>(chunk.size()));
    assert(file.fptr.gcount() == static_cast<data_size_t>(chunk.size()));

    assert(chunk.size() <= item.rest_size);
    item.rest_size -= chunk.size();

    return chunk;
}

auto LocalFileReader::is_inited() const noexcept -> bool
{
    if (m_items.empty())
    {
        assert(m_infos.empty());
        return false;
    }
    else
    {
        assert(not m_infos.empty());
        return true;
    }
}

auto LocalFileReader::init() -> void
{
    assert(not is_inited());

    const auto length = m_file_list.size();

    m_items.reserve(length);
    m_infos.reserve(length);

    for (auto idx = ItemIdx{0}; idx < length; ++idx)
    {
        auto path = m_file_list.at(idx);

        assert(std::filesystem::exists(path));
        assert(std::filesystem::is_regular_file(path));

        auto size = std::filesystem::file_size(path);
        auto fptr = std::ifstream(path, std::ios::binary);

        if (not fptr.good())
        {
            throw std::ios::failure{std::string{path}};
        }

        m_items.push_back(idx);
        m_infos.push_back(
            {
                .file { .path = std::move(path), .fptr = std::move(fptr) },
                .item { .orig_size = size, .rest_size = size },
            }
        );
    }

    assert(m_items.size() == length);
    assert(m_infos.size() == length);
}

}
