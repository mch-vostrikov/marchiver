#pragma once

#include <memory>
#include <functional>

namespace marchiver {

/**
    Provides data to compress. Not required to be thread-safe. Allows usage of custom deleter for returned buffer for flexibility.
*/
class DataReader
{
public:
    using DefaultDeleter = std::default_delete<uint8_t[]>;
    /**
        Attempts to retrieve next data chunk.
        \param[out] data_size Chunk size that was read, zero means that no data is available.
        \returns \c std::unique_ptr containing data that was actualy read, nullptr means that no data is available.
    */
    virtual std::unique_ptr<uint8_t[], std::function<void (uint8_t*)>> GetData(size_t& data_size) = 0;

    /**
        Returns maximum (default) chunk size supported by the reader.
    */
    virtual size_t ChunkSize() const = 0;

    virtual ~DataReader() = default;
};

}
