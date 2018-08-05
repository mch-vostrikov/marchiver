#include <buffer_pool_reader.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <cstring>
#include <unordered_set>
#include <string>
#include <sstream>
#include <vector>

using marchiver::BufferPoolReader;

TEST(BufferPoolReader, SequentialRead)
{
    std::string data = "qwerty";
    std::istringstream data_stream {data};
    auto reader = std::make_shared<BufferPoolReader>(data_stream, 1);

    size_t data_size;
    auto data_it = data.begin();
    for (auto buff = reader->GetData(data_size);
        data_size > 0 && data_it != data.end();
        buff = reader->GetData(data_size), ++data_it)
    {
        ASSERT_EQ(data_size, 1);
        EXPECT_EQ(buff[0], *data_it);
    }
    ASSERT_EQ(data_it, data.end());
}

TEST(BufferPoolReader, ReadSize)
{
    size_t chunk_size = 4;
    std::string data = "qwerty";
    size_t remaining_size = data.size() % chunk_size;
    std::istringstream data_stream {data};
    auto reader = std::make_shared<BufferPoolReader>(data_stream, chunk_size);

    size_t data_size;
    auto buff = reader->GetData(data_size);
    ASSERT_EQ(data_size, chunk_size);
    EXPECT_EQ(memcmp(buff.get(), data.data(), data_size), 0);
    buff = reader->GetData(data_size);
    ASSERT_EQ(data_size, remaining_size);
    EXPECT_EQ(memcmp(buff.get(), data.data() + chunk_size, data_size), 0);
}

TEST(BufferPoolReader, Capacity1)
{
    std::unordered_set<uint8_t*> buffers;
    std::string data = "qwertyqwerty12345";
    std::istringstream data_stream {data};
    auto reader = std::make_shared<BufferPoolReader>(data_stream, 1);

    EXPECT_EQ(reader->Capacity(), 0);
    for (int i = 0; i < data.size(); ++i) {
        size_t data_size;
        auto buff = reader->GetData(data_size);
        buffers.insert(buff.get());
        buff.reset();
        EXPECT_EQ(reader->Capacity(), 1);
    }
    ASSERT_EQ(buffers.size(), 1) << "single buffer must have been reused";
}

TEST(BufferPoolReader, Capacity2)
{
    std::unordered_set<uint8_t*> buffers;
    std::string data = "qwertyqwerty12345";
    std::istringstream data_stream {data};
    auto reader = std::make_shared<BufferPoolReader>(data_stream, 1, 2);

    EXPECT_EQ(reader->Capacity(), 2);
    for (int i = 0; i < data.size() / 2; ++i) {
        size_t data_size;
        auto buff1 = reader->GetData(data_size);
        auto buff2 = reader->GetData(data_size);
        buffers.insert(buff1.get());
        buffers.insert(buff2.get());
        buff1.reset();
        buff2.reset();
        EXPECT_EQ(reader->Capacity(), 2);
    }
    ASSERT_EQ(buffers.size(), 2) << "two preallocated buffers must have been reused";
}

TEST(BufferPoolReader, Capacity3)
{
    size_t initial_capacity = 5;
    std::vector<std::unique_ptr<uint8_t[], std::function<void (uint8_t*)>>> buffers;
    std::string data = "qwertyqwerty12345";
    std::istringstream data_stream {data};
    auto reader = std::make_shared<BufferPoolReader>(data_stream, 1, initial_capacity);

    EXPECT_EQ(reader->Capacity(), initial_capacity);
    EXPECT_EQ(reader->Size(), initial_capacity);
    for (int i = 0; i < data.size(); ++i) {
        size_t data_size;
        buffers.emplace_back(reader->GetData(data_size));
        EXPECT_EQ(reader->Capacity(), std::max(static_cast<int>(initial_capacity), i + 1))
            << "use initial buffers while any";
        EXPECT_EQ(reader->Size(), std::max(static_cast<int>(initial_capacity - i - 1), 0))
            << "initial buffers are getting depleted";
    }
    EXPECT_EQ(reader->Capacity(), data.size());
    EXPECT_EQ(reader->Size(), 0) << "all buffers are held";
    buffers.clear();
    EXPECT_EQ(reader->Size(), data.size()) << "all buffers are returned";
}
