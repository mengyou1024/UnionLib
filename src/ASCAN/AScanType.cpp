#include "AScanType.hpp"
#include <tuple>

namespace Union::AScan {
    QList<QPointF> AScanIntf::getAScanSeriesData(int index, double softGain) const {
        QList<QPointF> ret;
        const auto     AScanStep = getAxisLen(index) / static_cast<double>(getScanData(index).size());
        for (auto i = 0; std::cmp_less(i, getScanData(index).size()); i++) {
            auto ampValue = getScanData(index)[i] / 2.0;
            if (ampValue <= getSupression(index)) {
                ampValue = 0;
            }
            ret.append({getAxisBias(index) + i * AScanStep, CalculateGainOutput(ampValue, softGain)});
        }
        return ret;
    }

    std::optional<std::tuple<double, uint8_t>> AScanIntf::getGateResult(int idx, int gate_idx, bool find_center_if_overflow, bool enable_supression) const {
        const auto &_data = getScanData(idx);
        const auto  _gate = getGate(idx).at(gate_idx);
        auto        start = _gate.pos;
        double      end   = (double)(_gate.pos + _gate.width);
        if (_data.size() < 100 || _gate.width <= 0.001 || std::abs(start - 1.0) < 0.00001 || _gate.pos < 0.0 || end > 1.0 || !_gate.enable) {
            return std::nullopt;
        }
        auto left  = _data.begin() + static_cast<int64_t>((double)_data.size() * (double)_gate.pos);
        auto right = _data.begin() + static_cast<int64_t>((double)_data.size() * (double)(_gate.pos + _gate.width));
        auto max   = std::max_element(left, right);

        if (find_center_if_overflow && *max == 255) {
            std::vector<decltype(left)> minMaxVec = {};
            for (auto &_left = left, &_right = right; _left != _right; _left++) {
                if (*_left == 255) {
                    minMaxVec.push_back(_left);
                }
            }
            max = minMaxVec[0] + minMaxVec.size() / 2;
        }
        auto pos = ((double)std::distance(_data.begin(), max) / (double)_data.size());
        if (enable_supression && *max <= 255 * getSupression(idx)) {
            return std::nullopt;
        }
        if (pos < 0.0f) {
            return std::nullopt;
        }
        return std::make_tuple(pos, *max);
    }

    std::array<double, 2> AScanIntf::getAxisRange(int idx) const {
        return std::array<double, 2>({getAxisBias(idx), getAxisBias(idx) + getAxisLen(idx)});
    }

} // namespace Union::AScan
