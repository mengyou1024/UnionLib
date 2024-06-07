#include "Gate.hpp"

namespace Union::Base {
    GateResult CalculateGateResult(
        const std::vector<uint8_t> &data,
        const Gate                 &gate,
        bool                        find_center_if_overflow,
        const std::optional<int>   &supression,
        uint8_t                     view_max) {
        // empty line
        auto   start = gate.pos;
        double end   = (double)(gate.pos + gate.width);
        if (data.size() < 100 || gate.width <= 0.001 || std::abs(start - 1.0) < 0.00001 || gate.pos < 0.0 || end > 1.0 || !gate.enable) {
            return std::nullopt;
        }
        auto left  = data.begin() + static_cast<int64_t>((double)data.size() * (double)gate.pos);
        auto right = data.begin() + static_cast<int64_t>((double)data.size() * (double)(gate.pos + gate.width));
        auto max   = std::max_element(left, right);

        if (find_center_if_overflow && *max == 255) {
            std::vector<decltype(left)> minMaxVec = {};
            for (auto &_left = left, &_right = right; _left != _right; _left++) {
                if (*_left == 255) {
                    minMaxVec.push_back(_left);
                }
            }
            max = minMaxVec.at(0) + minMaxVec.size() / 2;
        }
        auto pos = ((double)std::distance(data.begin(), max) / (double)data.size());
        if (supression.has_value()) {
            if ((*max) < ((view_max / 100.0) * (supression.value()))) {
                return std::nullopt;
            }
        }
        if (pos < 0.0f) {
            return std::nullopt;
        }
        return std::make_tuple(pos, *max);
    }
} // namespace Union::Base