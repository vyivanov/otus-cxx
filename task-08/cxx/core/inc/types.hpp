#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <vector>

namespace ddt {

using ItemIdx  = std::uint64_t;
using ItemList = std::vector<ItemIdx>;
using ItemDups = std::optional<std::vector<ItemList>>;
using ItemData = std::vector<std::uint8_t>;
using DataSize = std::size_t;
using HashData = std::size_t;

using DataStub = std::vector<ItemData>;
using FileList = std::vector<std::filesystem::path>;
using HashData = std::size_t;
using HashFunc = std::function<HashData(HashData, const ItemData&)>;

struct ItemInfo
{
    DataSize orig_size;
    DataSize rest_size;
};

}
