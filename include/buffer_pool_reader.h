#pragma once

#include <data_reader.h>
#include <atomic>
#include <istream>
#include <mutex>
#include <vector>

namespace marchiver {

/**
    Reads data from provided \c std::istream, maintaining pool of reusable buffers.
    Relies on shared_from_this to track it's own lifetime.
*/
class BufferPoolReader: public DataReader, public std::enable_shared_from_this<BufferPoolReader>
{
public:
    /**
        \param stream Input stream to read from, lifetime managed by the caller.
        \param chunk_size Size of each buffer.
        \param buffers_count Initial count of pre-allocated buffers, default is 0.
    */
    BufferPoolReader(std::istream& stream, size_t chunk_size, size_t buffers_count = 0);

    std::unique_ptr<uint8_t[], std::function<void (uint8_t*)>> GetData(size_t& data_size) override;

    size_t ChunkSize() const override;

    size_t Capacity() const;
    size_t Size() const;

private:
    struct BufferDeleter;

    std::unique_ptr<uint8_t[], std::function<void (uint8_t*)>> GetBuffer();
    std::unique_ptr<uint8_t[]> TryGetBuffer();
    std::unique_ptr<uint8_t[]> CreateBuffer();
    void CollectBuffer(uint8_t* buffer);

    std::istream& stream_;
    size_t chunk_size_;
    std::vector<std::unique_ptr<uint8_t[]>> buffer_pool_;
    mutable std::mutex mux_;
    std::atomic_size_t capacity_;
};

}
