#include "AScanType.hpp"
#include <map>
#include <tuple>

namespace Union::AScan {

    std::optional<std::tuple<double, uint8_t>> AScanData::GetGateResult(int gateIndex, bool find_center_if_overflow) const {
        if (std::cmp_greater(gateIndex, this->gate.size())) {
            return std::nullopt;
        }
        const auto& _data = this->ascan;
        const auto& _gate = this->gate[gateIndex];
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

    std::array<double, 2> AScanData::GetAxisRange() const {
        return std::array<double, 2>({axisBias, axisBias + axisLen});
    }

    std::function<double(double)> AScanData::getAVGLineExpr() const {
        auto func = std::bind(Union::EchoDbDiffOfHole, std::placeholders::_1, 2.0, std::placeholders::_2, 2.0);
        return getLineExpr(avg->index, avg->value, {0.0, 160.0}, {0.0, 520.0}, func);
    }

    std::function<double(double)> AScanData::getDACLineExpr() const {
        return getLineExpr(dac->index, dac->value, {0.0, 106.59}, {0.0, 520.0});
    }

    std::function<double(double)> AScanData::getLineExpr(const std::vector<int>&               _index,
                                                         const std::vector<uint8_t>&           value,
                                                         const std::array<double, 2>&          r_range,
                                                         const std::array<double, 2>&          v_range,
                                                         std::function<double(double, double)> func_db_diff) const {
        // TODO: 曲线平滑
        using Union::ValueMap;

        std::vector<double> index;
        for (auto& it : _index) {
            index.emplace_back(ValueMap((double)it, r_range, v_range));
        }

        return [=, this](double val) -> double {
            val = ValueMap(val, {axisBias, axisBias + axisLen}, v_range);
            if (val <= index[0]) {
                return value.at(0);
            } else {
                std::pair<double, double>                first  = {};
                std::optional<std::pair<double, double>> second = std::nullopt;
                if (index.size() == 1) {
                    first = {index[0], value[0]};
                } else {
                    bool in_range = false;
                    for (int i = 1; std::cmp_less(i, index.size()); i++) {
                        if (val >= index[i - 1] && val <= index[i]) {
                            first    = {index[i - 1], value[i - 1]};
                            second   = {index[i], value[i]};
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
        };
    }

#if __has_include("QtCore")
    QJsonObject AScanType::GetBoardParamJSON(const AScanType& val) {
        QJsonObject ret;
        ret.insert("probe", QString::fromStdWString(val.probe));
        ret.insert("angle", val.angle);
        ret.insert("soundVelocity", val.soundVelocity);
        QJsonObject performance;
        performance.insert("horizontalLinearity", val.performance.horizontalLinearity);
        performance.insert("verticalLinearity", val.performance.verticalLinearity);
        performance.insert("resolution", val.performance.resolution);
        performance.insert("dynamicRange", val.performance.dynamicRange);
        performance.insert("sensitivity", val.performance.sensitivity);
        ret.insert("performance", performance);
        return ret;
    }

    QList<QPointF> AScanType::GetAScanSeriesData(const AScanData& data, double softGain) {
        QList<QPointF> ret;
        const auto     AScanStep = data.axisLen / static_cast<double>(data.ascan.size());
        for (auto i = 0; std::cmp_less(i, data.ascan.size()); i++) {
            auto ampValue = data.ascan[i] / 2.0;
            if (ampValue <= data.suppression) {
                ampValue = 0;
            }
            ret.append({data.axisBias + i * AScanStep, CalculateGainOutput(ampValue, softGain)});
        }
        return ret;
    }

    bool AScanFileSelector::RegistReader(std::string_view file_suffix, std::string_view describe, const AScanFileSelector::AFS_RFUNC& func) {
        auto temp = std::make_tuple(func, std::string(describe));
        return data.try_emplace(std::string(file_suffix), temp).second;
    }

    const std::optional<AScanFileSelector::AFS_DTYPE> AScanFileSelector::Get(const std::string& file_suffix) {
        const auto ret = data.find(file_suffix);
        if (ret == data.end()) {
            return std::nullopt;
        } else {
            return ret->second;
        }
    }

    QJsonArray AScanFileSelector::GetFileNameFilter() {
        QJsonArray ret;
        for (auto& [key, val] : data) {
            auto& [func, describe] = val;
            ret.append(QString("%1 (%2)").arg(QString::fromStdString(describe), QString::fromStdString(key).toLower()));
        }
        return ret;
    }

    QJsonArray AScanFileSelector::GetFileListModelNameFilter() {
        QJsonArray obj;
        for (auto& [key, val] : data) {
            static QRegularExpression reg(R"((\*\.\w+))");
            auto                      ret = reg.globalMatch(QString::fromStdString(key), 0);
            while (ret.hasNext()) {
                QRegularExpressionMatch match = ret.next();
                obj.append(match.captured(1).toLower());
            }
        }
        return obj;
    }

    QJsonObject AScanFileSelector::GetUINameMap() {
        QJsonObject obj;
        for (auto& [key, val] : data) {
            static QRegularExpression reg(R"((\.\w+))");
            auto                      ret = reg.globalMatch(QString::fromStdString(key), 0);
            while (ret.hasNext()) {
                QRegularExpressionMatch match = ret.next();
                obj.insert(match.captured(1).toLower(), "AScan");
            }
        }
        return obj;
    }

    const std::optional<AScanFileSelector::AFS_RFUNC> AScanFileSelector::GetReadFunction(const std::wstring& fileName) {
        QFileInfo file(QString::fromStdWString(fileName));
        for (auto& [key, val] : data) {
            if (QString::fromStdString(key).toLower().contains(file.suffix().toLower())) {
                const auto& [func, describe] = val;
                return func;
            }
        }
        return std::nullopt;
    }

    QJsonObject AScanType::GetBoardParamJSON(void) const {
        QJsonObject ret;
        ret.insert("probe", QString::fromStdWString(probe));
        ret.insert("angle", angle);
        ret.insert("soundVelocity", soundVelocity);
        QJsonObject _performance;
        _performance.insert("horizontalLinearity", this->performance.horizontalLinearity);
        _performance.insert("verticalLinearity", this->performance.verticalLinearity);
        _performance.insert("resolution", this->performance.resolution);
        _performance.insert("dynamicRange", this->performance.dynamicRange);
        _performance.insert("sensitivity", this->performance.sensitivity);
        ret.insert("performance", _performance);
        return ret;
    }

    QList<QPointF> AScanType::GetAScanSeriesData(int index, double softGain) const {
        if (std::cmp_greater(index, this->data.size())) {
            throw std::runtime_error("AScanType::GetAScanSeriesData: index out of range");
        }
        const auto&    aScanData = this->data[index];
        QList<QPointF> ret;
        const auto     AScanStep = aScanData.axisLen / static_cast<double>(aScanData.ascan.size());
        for (auto i = 0; std::cmp_less(i, aScanData.ascan.size()); i++) {
            ret.append({aScanData.axisBias + i * AScanStep, CalculateGainOutput(aScanData.ascan[i] / 2.0, softGain)});
        }
        return ret;
    }
#endif

} // namespace Union::AScan
