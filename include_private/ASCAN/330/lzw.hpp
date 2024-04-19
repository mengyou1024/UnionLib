#pragma once
#include <cstdint>
#include <optional>
#include <vector>

namespace Union::__330 {
    std::optional<std::vector<uint8_t>> lzw_decompress(const uint8_t *data, std::size_t size);
}
