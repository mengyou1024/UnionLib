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

    std::function<double(double)> AScanIntf::getLineExpr(int idx, const std::vector<int>& _index, const std::vector<uint8_t>& value, const std::array<double, 2>& r_range, const std::array<double, 2>& v_range, std::function<double(double, double)> func_db_diff) const {
        // TODO: 曲线平滑
        using Union::ValueMap;

        std::vector<double> index;
        for (auto& it : _index) {
            index.emplace_back(ValueMap((double)it, r_range, v_range));
        }

        return [=, this](double val) -> double {
            try {
                val = ValueMap(val, {getAxisBias(idx), getAxisBias(idx) + getAxisLen(idx)}, v_range);
                if (val <= index.at(0)) {
                    return value.at(0);
                } else if (val >= index.back()) {
                    return value.back();
                } else {
                    std::pair<double, double>                first  = {};
                    std::optional<std::pair<double, double>> second = std::nullopt;
                    if (index.size() == 1) {
                        first = {index.at(0), value.at(0)};
                    } else {
                        bool in_range = false;
                        for (int i = 1; std::cmp_less(i, index.size()); i++) {
                            if (val >= index.at(i - 1) && val <= index.at(i)) {
                                first    = {index.at(i - 1), value.at(i - 1)};
                                second   = {index.at(i), value.at(i)};
                                in_range = true;
                            }
                        }
                        if (!in_range) {
                            first  = {index.back(), value.back()};
                            second = std::nullopt;
                        }
                    }

                    auto CalcluateValue = [=](std::pair<double, double> pt = {}) -> double {
                        return Union::CalculateGainOutput(pt.second, func_db_diff(val, pt.first));
                    };

                    if (second.has_value()) {
                        auto value_first   = CalcluateValue(first);
                        auto vallue_second = CalcluateValue(second.value());
                        auto first_rate    = (val - first.first) / (second->first - first.first);
                        auto second_rate   = (second->first - val) / (second->first - first.first);
                        return value_first * second_rate + vallue_second * first_rate;
                    }
                    return CalcluateValue(first);
                }
            } catch (std::exception& e) {
#if __has_include("QtCore")
                qDebug(QLoggingCategory("AScanType")) << "getLineExpr error! msg:" << e.what();
#endif
                return 0;
            }
        };
    }

    std::optional<std::tuple<double, uint8_t>> AScanIntf::getGateResult(int idx, int gate_idx, bool find_center_if_overflow) const {
        const auto& _data = getScanData(idx);
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
        if (pos < 0.0f) {
            return std::nullopt;
        }
        return std::make_tuple(pos, *max);
    }

    std::array<double, 2> AScanIntf::getAxisRange(int idx) const {
        return std::array<double, 2>({getAxisBias(idx), getAxisBias(idx) + getAxisLen(idx)});
    }

} // namespace Union::AScan
