
#pragma once

#include <iostream>
#include <iterator>
#include <optional>

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace esb {

union iff_tag {
    uint32_t val;
    char     ascii[4];
};

struct iff_chunk {
    iff_tag    id;
    uint32_t   size;
    std::byte* data;
};

const iff_tag TAG_FORM = {{'MROF'}};

template <typename ChunkT>
class chunk_iterator {
public:
    typedef chunk_iterator<ChunkT>  self_type;
    typedef ptrdiff_t               difference_type;
    typedef ChunkT                  value_type;
    typedef ChunkT&                 reference;
    typedef ChunkT*                 pointer;
    typedef std::input_iterator_tag iterator_categor;

    chunk_iterator(std::byte* data, uint32_t size) : data_{data}, size_{size} {
        if (size_ > 0) {
            value_ = create_chunk_from_raw(data_, size_);
        }
    }

    chunk_iterator& operator++() {
        data_ += value_->size + sizeof(value_->id) + sizeof(value_->size);
        size_ -= value_->size + sizeof(value_->id) + sizeof(value_->size);

        if (size_ > 0) {
            value_ = create_chunk_from_raw(data_, size_);
        } else {
            value_.reset();
        }

        return *this;
    }
    chunk_iterator operator++(int) {
        data_ += value_->size + sizeof(value_->id) + sizeof(value_->size);
        size_ -= value_->size + sizeof(value_->id) + sizeof(value_->size);

        if (size_ > 0) {
            value_ = create_chunk_from_raw(data_, size_);
        } else {
            value_.reset();
        }

        return *this;
    }

    bool operator!=(chunk_iterator<ChunkT>& other) {
        return data_ != other.data_ && size_ != other.size_;
    }

    reference operator*() { return *value_; }
    pointer   operator->() { return &(*value_); }

private:
    std::byte*                                 data_;
    uint32_t                                   size_;
    std::optional<std::remove_const_t<ChunkT>> value_;
};

inline chunk_iterator<iff_chunk> begin(iff_chunk& c) {
    return chunk_iterator<iff_chunk>(c.data + sizeof(iff_tag), c.size - sizeof(iff_tag));
}

inline chunk_iterator<const iff_chunk> begin(const iff_chunk& c) {
    return chunk_iterator<const iff_chunk>(c.data + sizeof(iff_tag), c.size - sizeof(iff_tag));
}

inline chunk_iterator<iff_chunk> end(iff_chunk& c) {
    return chunk_iterator<iff_chunk>(c.data + c.size, 0);
}

inline chunk_iterator<const iff_chunk> end(const iff_chunk& c) {
    return chunk_iterator<const iff_chunk>(c.data + c.size, 0);
}

inline iff_chunk create_chunk_from_raw(std::byte* data, uint64_t size) {
    iff_chunk chunk;

    assert(size >= sizeof(chunk.id) + sizeof(chunk.size));

    chunk.id   = {{*reinterpret_cast<uint32_t*>(data)}};
    chunk.size = _byteswap_ulong(*reinterpret_cast<uint32_t*>(data + sizeof(chunk.id)));

    assert(chunk.size <= size - sizeof(chunk.id) - sizeof(chunk.size));

    chunk.data = data + sizeof(chunk.id) + sizeof(chunk.size);

    return chunk;
}

inline bool is_form_chunk(const iff_chunk& chunk) { return chunk.id.val == TAG_FORM.val; }

inline iff_tag get_form_type(const iff_chunk& chunk) {
    assert(is_form_chunk(chunk));

    return {{*reinterpret_cast<uint32_t*>(chunk.data)}};
}

inline std::optional<iff_chunk> get_first_child(const iff_chunk& chunk) {
    std::optional<iff_chunk> child;

    auto& b = begin(chunk);
    if (b != end(chunk)) {
        child = *b;
    }

    return child;
}

inline std::ostream& operator<<(std::ostream& os, const iff_tag& tag) {
    os << tag.ascii[0] << tag.ascii[1] << tag.ascii[2] << tag.ascii[3];
    return os;
}

}  // namespace esb
