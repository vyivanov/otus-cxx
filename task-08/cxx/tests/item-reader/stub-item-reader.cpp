#include <cassert>
#include <gtest/gtest.h>

#include <core/inc/item-reader/stub-item-reader.hpp>

TEST(invalid_initialization, dataset_is_empty)
{
    EXPECT_THROW(ddt::StubItemReader::create({}, 64), std::length_error);
}

TEST(invalid_initialization, dataset_has_single_item)
{
    EXPECT_THROW(ddt::StubItemReader::create({{}}, 64), std::length_error);
}

TEST(invalid_initialization, chunk_size_is_zero)
{
    EXPECT_THROW(ddt::StubItemReader::create({{},{}}, 0), std::length_error);
}

TEST(valid_initialization, dataset_has_empty_items)
{
    EXPECT_NO_THROW(ddt::StubItemReader::create({{},{}}, 64));
}

TEST(valid_initialization, dataset_has_two_items)
{
    EXPECT_NO_THROW(ddt::StubItemReader::create({{0,1},{0,1}}, 64));
}

TEST(enumeration, dataset_has_empty_items)
{
    const auto dataset = ddt::DataStub
    {
        {}, {}, {}, {}, {}
    };

    const auto reader = ddt::StubItemReader::create(dataset, 8);

    EXPECT_EQ(reader->enumerate().size(), dataset.size());
}

TEST(enumeration, dataset_has_mixed_items)
{
    const auto dataset = ddt::DataStub
    {
        {0, 1, 2, 3}, {}, {0, 1, 2, 3}, {}, {0, 1, 2, 3}
    };

    const auto reader = ddt::StubItemReader::create(dataset, 8);

    EXPECT_EQ(reader->enumerate().size(), dataset.size());
}

TEST(statistics, initialization)
{
    const auto dataset = ddt::DataStub
    {
        {},
        {0},
        {0, 1},
        {0, 1, 2},
        {0, 1, 2, 3},
        {0, 1, 2, 3, 4},
    };

    const auto reader = ddt::StubItemReader::create(dataset, 8);

    for (const auto item: reader->enumerate())
    {
        EXPECT_EQ(reader->read_stats(item).orig_size, dataset.at(item).size());
        EXPECT_EQ(reader->read_stats(item).orig_size, reader->read_stats(item).rest_size);
    }
}

TEST(statistics, invalid_item)
{
    const auto dataset = ddt::DataStub
    {
        {}, {}, {}
    };

    const auto reader = ddt::StubItemReader::create(dataset, 1);

    EXPECT_THROW(reader->read_stats(ddt::ItemIdx{100}), std::out_of_range);
}

TEST(reading, invalid_item)
{
    const auto dataset = ddt::DataStub
    {
        {}, {}, {}
    };

    const auto reader = ddt::StubItemReader::create(dataset, 1);

    EXPECT_THROW(reader->read_chunk(ddt::ItemIdx{100}), std::out_of_range);
}

TEST(reading, empty_items)
{
    const auto dataset = ddt::DataStub
    {
        {}, {}, {}
    };

    const auto reader = ddt::StubItemReader::create(dataset, 1);

    for (const auto item: reader->enumerate())
    {
        EXPECT_THROW(reader->read_chunk(item), std::underflow_error);
    }
}

TEST(reading, big_chunk_size)
{
    const auto dataset = ddt::DataStub
    {
        {0, 1, 2, 3}, {}
    };

    const auto item       = ddt::ItemIdx{0};
    const auto item_size  = dataset.at(item).size();
    const auto chunk_size = ddt::DataSize{2 * item_size};

    assert(chunk_size > item_size);

    const auto reader = ddt::StubItemReader::create(dataset, chunk_size);
    const auto chunk  = reader->read_chunk(item);

    EXPECT_EQ(chunk.size(), item_size);
    EXPECT_EQ(reader->read_stats(item).rest_size, 0);

    for (decltype(chunk.size()) idx{}; idx < chunk.size(); ++idx)
    {
        if (idx < item_size)
        {
            EXPECT_EQ(chunk.at(idx), dataset.at(item).at(idx));
        }
        else
        {
            EXPECT_EQ(chunk.at(idx), 0);
        }
    }
}

TEST(reading, mixed_size_items)
{
    const auto chunk_size = ddt::DataSize{256};

    const auto dataset = ddt::DataStub
    {
        ddt::ItemData(100 * chunk_size + (chunk_size - 0), 1),
        ddt::ItemData(200 * chunk_size + (chunk_size - 1), 2),
    };

    const auto reader = ddt::StubItemReader::create(dataset, chunk_size);

    for (const auto item: reader->enumerate())
    {
        while (reader->read_stats(item).rest_size)
        {
            const auto chunk = reader->read_chunk(item);
            EXPECT_LE(chunk.size(), chunk_size);
            EXPECT_TRUE(std::all_of(chunk.cbegin(), chunk.cend(),
                [fill = (item + 1)](const auto& byte) { return byte == fill; })
            );
        }
    }
}
