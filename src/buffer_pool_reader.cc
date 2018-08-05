#include <buffer_pool_reader.h>

namespace marchiver {

struct BufferPoolReader::BufferDeleter
{
    std::weak_ptr<BufferPoolReader> weak_reader;

    void operator()(uint8_t* buffer)
    {
        if (auto reader = weak_reader.lock()) {
            reader->CollectBuffer(buffer);
        } else {
            delete[] buffer;
        }
    }
};

BufferPoolReader::BufferPoolReader(std::istream& stream, size_t chunk_size, size_t buffers_count)
: stream_(stream), chunk_size_(chunk_size), capacity_(0)
{
    if (buffers_count > 0) {
        buffer_pool_.reserve(buffers_count);
        for (int i = 0; i < buffers_count; ++i) {
            buffer_pool_.emplace_back(CreateBuffer());
        }
    }
}

std::unique_ptr<uint8_t[], std::function<void (uint8_t*)>> BufferPoolReader::GetData(size_t& data_size)
{
    if (stream_.eof()) {
        data_size = 0;
        return nullptr;
    }
    auto buffer = GetBuffer();
    stream_.read(reinterpret_cast<char*>(buffer.get()), chunk_size_);
    data_size = stream_.gcount();
    if (data_size) return buffer;
    return nullptr;
}

size_t BufferPoolReader::ChunkSize() const
{
    return chunk_size_;
}

size_t BufferPoolReader::Capacity() const
{
    return capacity_;
}

size_t BufferPoolReader::Size() const
{
    std::lock_guard<std::mutex> guard(mux_);
    return buffer_pool_.size();
}

std::unique_ptr<uint8_t[], std::function<void (uint8_t*)>> BufferPoolReader::GetBuffer()
{
    BufferDeleter del {weak_from_this()};
    auto buffer = TryGetBuffer();
    if (!buffer) buffer = CreateBuffer();
    return {buffer.release(), del};
}

std::unique_ptr<uint8_t[]> BufferPoolReader::TryGetBuffer()
{
    std::lock_guard<std::mutex> guard(mux_);
    if (buffer_pool_.size()) {
        auto buffer = std::move(buffer_pool_.back());
        buffer_pool_.pop_back();
        return buffer;
    }
    return nullptr;
}

std::unique_ptr<uint8_t[]> BufferPoolReader::CreateBuffer()
{
    ++capacity_;
    return std::make_unique<uint8_t[]>(chunk_size_);
}

void BufferPoolReader::CollectBuffer(uint8_t* buffer)
{
    std::lock_guard<std::mutex> guard(mux_);
    buffer_pool_.emplace_back(buffer);
}

}
