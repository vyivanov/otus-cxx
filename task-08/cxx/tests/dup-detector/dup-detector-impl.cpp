#include <cassert>
#include <gtest/gtest.h>

#include <core/inc/data-hasher/boost-data-hasher.hpp>
#include <core/inc/dup-detector/dup-detector-impl.hpp>
#include <core/inc/item-reader/stub-item-reader.hpp>

using ddt::DataSize;
using ddt::DataStub;
using ddt::ItemData;
using ddt::ItemDups;
using ddt::ItemList;
using ddt::ItemIdx;
using ddt::HashData;

using ddt::BoostDataHasher;
using ddt::DupDetector;
using ddt::DupDetectorImpl;
using ddt::StubItemReader;

struct base: ::testing::Test
{

    auto initialize(const DataStub& data_set)
    {
        const auto reader = StubItemReader::create(data_set, 1);
        const auto hasher = BoostDataHasher::create();

        m_duplicates = DupDetectorImpl::create(reader, hasher);
    }

    auto detect() noexcept
    {
        assert(m_duplicates);
        m_duplicates->detect();
    }

    auto& result() noexcept
    {
        assert(m_duplicates);
        return m_duplicates->result();
    }

    DupDetector::Ptr m_duplicates{};

};

TEST_F(base, result_before_and_after_detection)
{
    const auto data = DataStub{{},{}};

    initialize(data);
    EXPECT_FALSE(result().has_value());

    detect();
    EXPECT_TRUE(result().has_value());
}

TEST_F(base, all_items_are_empty)
{
    const auto data = DataStub{{},{},{},{},{}};

    initialize(data);
    detect();

    EXPECT_EQ(result()->size(), 1);
    EXPECT_EQ(result()->at(0).size(), data.size());
}

TEST_F(base, all_items_are_equal)
{
    const auto data = DataStub{{1,2,3}, {1,2,3}, {1,2,3}};

    initialize(data);
    detect();

    EXPECT_EQ(result()->size(), 1);
    EXPECT_EQ(result()->at(0).size(), data.size());
}

TEST_F(base, all_items_are_different)
{
    const auto data = DataStub{{1,2,3}, {4,5,6}, {7,8,9}};

    initialize(data);
    detect();

    EXPECT_EQ(result()->size(), data.size());

    for (const auto& group: result().value())
    {
        EXPECT_EQ(group.size(), 1);
    }
}

TEST_F(base, several_groups_with_several_items)
{
    const auto data = DataStub{
        {1,2,3}     ,   // ItemIdx 0, group A
        {}          ,   // ItemIdx 1, group B
        {1,2,3,4,5} ,   // ItemIdx 2, group C
        {1,2,3}     ,   // ItemIdx 3, group A
        {}          ,   // ItemIdx 4, group B
        {1,2,3}     ,   // ItemIdx 5, group A
        {0,2,3}     ,   // ItemIdx 6, group D
        {1,2,3}     ,   // ItemIdx 7, group A
        {}          ,   // ItemIdx 8, group B
        {1,2,3,4,5} ,   // ItemIdx 9, group C
    };

    const auto group_A = ItemList{ItemIdx{0}, ItemIdx{3}, ItemIdx{5}, ItemIdx{7}};
    const auto group_B = ItemList{ItemIdx{1}, ItemIdx{4}, ItemIdx{8}};
    const auto group_C = ItemList{ItemIdx{2}, ItemIdx{9}};
    const auto group_D = ItemList{ItemIdx{6}};

    const auto data_dups = ItemDups{{group_A, group_B, group_C, group_D}};

    initialize(data);
    detect();

    EXPECT_EQ(result()->size(), data_dups->size());

    for (const auto& test_group: result().value())
    {
        for (const auto& data_group: data_dups.value())
        {
            if (test_group.size() == data_group.size())
            {
                for (const auto& test_item: test_group)
                {
                    EXPECT_TRUE(std::any_of(data_group.cbegin(), data_group.cend(),
                        [test_item](const auto data_item) { return data_item == test_item; }));
                }
            }
        }
    }
}
