
#include "lzw.hpp"
#include "_lzw.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace Union::__330 {
    std::optional<std::vector<uint8_t>> lzw_decompress(const uint8_t *data, std::size_t size) {
        std::vector<uint8_t> ret;
        ret.resize(8192 * 8);
        int decompressed_size = 0;
        Union::Temp::Unresovled ::__decode(const_cast<uint8_t *>(data), static_cast<int>(size), ret.data(), &decompressed_size);
        ret.resize(decompressed_size);
        return ret;
    }
} // namespace Union::__330
