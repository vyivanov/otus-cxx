#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace ddt {

using ItemIdx  = std::uint64_t;
using ItemList = std::vector<ItemIdx>;
using ItemDups = std::optional<std::vector<ItemList>>;
using ItemData = std::vector<std::uint8_t>;
using DataSize = std::size_t;
using HashData = std::size_t;

struct ItemInfo
{
    DataSize orig_size;
    DataSize rest_size;
};

}
