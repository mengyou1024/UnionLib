#include "AScanType.hpp"
#include <QLoggingCategory>
#include <tuple>

#ifndef QT_DEBUG
static Q_LOGGING_CATEGORY(TAG, "ASCAN.INTF");
#endif

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

    QVariantMap AScanIntf::createReportValueMap(int idx, double soft_gain) const {
        auto gateValue   = createGateValue(idx, soft_gain);
        auto sensitivity = QString("%1+%2+%3dB")
                               .arg(QString::number(KeepDecimals<1>(getBaseGain(idx)), 'f', 1))
                               .arg(QString::number(KeepDecimals<1>(getScanGain(idx)), 'f', 1))
                               .arg(QString::number(KeepDecimals<1>(getSurfaceCompentationGain(idx)), 'f', 1));
        QString soundDistance = QString::number(KeepDecimals<1>(getAxisLen(idx)), 'f', 1) + "mm";
        switch (getDistanceMode(idx)) {
            case Union::AScan::DistanceMode::DistanceMode_S:
                soundDistance += "(S)";
                break;
            case Union::AScan::DistanceMode::DistanceMode_Y:
                soundDistance += "(Y)";
                break;
            case Union::AScan::DistanceMode::DistanceMode_X:
                soundDistance += "(X)";
                break;
            default:
                soundDistance += "(N)";
                break;
        }

        QVariantMap vmp = {
            {QObject::tr("检测单位"), ""},
            {QObject::tr("报告编写"), ""},
            {QObject::tr("委托单位"), ""},
            {QObject::tr("检测日期"), QString::fromStdString(getDate(idx))},
            {QObject::tr("名称"), ""},
            {QObject::tr("编号"), ""},
            {QObject::tr("表面热处理"), ""},
            {QObject::tr("材质"), ""},
            {QObject::tr("表面粗糙度"), ""},
            {QObject::tr("探头型号"), ""},
            {QObject::tr("晶片尺寸"), QString::fromStdString(getProbeChipShape(idx))},
            {QObject::tr("探头类型"), QString::fromStdWString(getProbe(idx))},
            {QObject::tr("前沿"), QString::number(KeepDecimals<1>(getFrontDistance(idx)), 'f', 1) + "mm"},
            {QObject::tr("探头K值"), QString::number(KeepDecimals(Union::Base::Probe::Degree2K(getAngle(idx))), 'f', 2)},
            {QObject::tr("频率"), QString::number(KeepDecimals(getProbeFrequence(idx)), 'f', 2) + "MHz"},
            {QObject::tr("折射角"), QString::number(KeepDecimals<1>(getAngle(idx)), 'f', 1) + "°"},
            {QObject::tr("零点"), QString::number(KeepDecimals(getZeroPointBias(idx)), 'f', 2) + "μs"},
            {QObject::tr("仪器型号"), QString::fromStdString(getInstrumentName())},
            {QObject::tr("灵敏度"), sensitivity},
            {QObject::tr("回波抑制"), QString::number(getSupression(idx)) + "%"},
            {QObject::tr("回波延时"), QString::number(KeepDecimals<1>(getSamplingDelay(idx)), 'f', 1) + "mm"},
            {QObject::tr("声程范围"), soundDistance},
            {QObject::tr("声速"), QString::number(KeepDecimals<0>(getSoundVelocity(idx)), 'f', 0) + "m/s"},
            {QObject::tr("距离"), gateValue[0].toObject()["dist_c"].toString()},
            {QObject::tr("水平"), gateValue[0].toObject()["dist_a"].toString()},
            {QObject::tr("垂直"), gateValue[0].toObject()["dist_b"].toString()},
            {QObject::tr("当量"), gateValue[0].toObject()["equi"].toString()},
            {QObject::tr("长度"), ""},
            {QObject::tr("高度"), gateValue[0].toObject()["amp"].toString()},
            {QObject::tr("等级"), ""},
            {QObject::tr("探伤结果"), ""},
            {QObject::tr("探伤标准"), ""},
            {QObject::tr("探伤人员"), ""},
            {QObject::tr("负责人员"), ""},
            {QObject::tr("备注"), ""},
        };
        return vmp;
    }

    QJsonArray AScanIntf::createGateValue(int idx, double soft_gain) const {
        std::array<QVariantMap, 2> _gateValue = {};
        for (auto i = 0; std::cmp_less(i, _gateValue.size()); i++) {
            _gateValue[i] = {
                {"amp",    "-"},
                {"dist_a", "-"},
                {"dist_b", "-"},
                {"dist_c", "-"},
                {"equi",   "-"},
            };
        }
        for (auto i = 0; std::cmp_less(i, _gateValue.size()); i++) {
            auto info = getGateResult(idx, i);
            if (!info) {
                continue;
            }
            auto [loc, _amp] = info.value();
            double amp       = _amp;
            amp              = Union::CalculateGainOutput(amp, soft_gain);
            if (amp > 255.0) {
                amp = 255.0;
            }
            QString               _a = "-";
            QString               _b = "-";
            QString               _c = "-";
            std::optional<double> a  = std::nullopt;
            std::optional<double> b  = std::nullopt;
            std::optional<double> c  = std::nullopt;

            switch (getDistanceMode(idx)) {
                case Union::AScan::DistanceMode::DistanceMode_Y: {
                    b = Union::ValueMap(loc, getAxisRange(idx));
                    if (std::abs(getAngle(idx)) > 0.0001) {
                        a = b.value() / Union::Base::Probe::Degree2K(getAngle(idx));
                        c = b.value() / std::sin(Union::Base::Probe::Degree2Rd(getAngle(idx)));
                    } else {
                        a = 0;
                        c = b;
                    }
                    break;
                }
                case Union::AScan::DistanceMode::DistanceMode_X: {
                    a = Union::ValueMap(loc, getAxisRange(idx));
                    if (std::abs(getAngle(idx)) > 0.0001) {
                        b = Union::Base::Probe::Degree2K(getAngle(idx)) * b.value();
                        c = b.value() / std::cos(Union::Base::Probe::Degree2Rd(getAngle(idx)));
                    } else {
                        constexpr auto msg = "The Angle of refraction cannot be less than 0 in X path mode";
#if defined(QT_DEBUG)
                        qFatal(msg);
#else
                        qCritical(TAG) << msg;
#endif
                    }
                    break;
                }
                case Union::AScan::DistanceMode::DistanceMode_S: {
                    c = Union::ValueMap(loc, getAxisRange(idx));
                    if (std::abs(getAngle(idx)) > 0.0001) {
                        a = c.value() * std::cos(Union::Base::Probe::Degree2Rd(getAngle(idx)));
                        b = c.value() * std::sin(Union::Base::Probe::Degree2Rd(getAngle(idx)));
                    } else {
                        a = 0;
                        b = c;
                    }
                    break;
                }
            }

            if (a) {
                _a = QString::number(KeepDecimals<1>(a.value()), 'f', 1);
            }

            if (b) {
                _b = QString::number(KeepDecimals<1>(b.value()), 'f', 1);
            }

            if (c) {
                _c = QString::number(KeepDecimals<1>(c.value()), 'f', 1);
            }

            QString _equi = "-";
            if (getDAC(idx) && b.has_value()) {
                auto r_amp      = Union::CalculateGainOutput(_amp, getSurfaceCompentationGain(idx));
                auto lineExpr   = getDACLineExpr(idx);
                auto slValue    = lineExpr((b.value() - getAxisBias(idx)) / getAxisLen(idx) * getScanData(idx).size());
                auto modifyGain = getBaseGain(idx) + getScanGain(idx) +
                                  getSurfaceCompentationGain(idx) - getDAC(idx)->baseGain +
                                  getDACStandard(idx).slBias;
                slValue = Union::CalculateGainOutput(slValue, modifyGain);
                _equi   = QString::asprintf("SL%+.1fdB", KeepDecimals<1>(Union::CalculatedGain(slValue, r_amp)));
            } else if (getAVG(idx) && b.has_value()) {
                auto r_amp      = Union::CalculateGainOutput(_amp, getSurfaceCompentationGain(idx));
                auto lineExpr   = getAVGLineExpr(idx);
                auto slValue    = lineExpr((b.value() - getAxisBias(idx)) / getAxisLen(idx) * getScanData(idx).size());
                auto modifyGain = getBaseGain(idx) + getScanGain(idx) +
                                  getSurfaceCompentationGain(idx) - getAVG(idx)->baseGain +
                                  getDACStandard(idx).slBias;
                slValue = Union::CalculateGainOutput(slValue, modifyGain);
                _equi   = QString::asprintf("Φ%+.1fdB", KeepDecimals<1>(Union::CalculatedGain(slValue, r_amp)));
            }

            auto _gate_amp = amp / 2.0;
            if (_gate_amp > 100.0) {
                _gate_amp = 100.0;
            }

            _gateValue[i] = {
                {"amp", QString::number(_gate_amp, 'f', 1)},
                {"dist_a", _a},
                {"dist_b", _b},
                {"dist_c", _c},
                {"equi", _equi},
            };
        }

        return QJsonArray::fromVariantList({_gateValue[0], _gateValue[1]});
    }

    QVariantMap AScanIntf::createTechnologicalParameter(int idx) const {
        QVariantMap gainPrarameter = {
            {QObject::tr("基本增益"), QString::number(KeepDecimals<1>(getBaseGain(idx)),                'f', 1) + " dB"},
            {QObject::tr("扫查增益"), QString::number(KeepDecimals<1>(getScanGain(idx)),                'f', 1) + " dB"},
            {QObject::tr("表面补偿"), QString::number(KeepDecimals<1>(getSurfaceCompentationGain(idx)), 'f', 1) + " dB"},
        };

        QVariantMap probeParameter = {
            {QObject::tr("探头类型"), QString::fromStdWString(getProbe(idx))},
            {QObject::tr("探头频率"), QString::number(KeepDecimals<1>(getProbeFrequence(idx)), 'f', 1) + " MHz"},
            {QObject::tr("晶片尺寸"), QString::fromStdString(getProbeChipShape(idx))},
        };

        QVariantMap basicParameter = {
            {QObject::tr("声程"), QString::number(KeepDecimals<1>(getAxisLen(idx)), 'f', 1) + " mm"},
            {QObject::tr("前沿"), QString::number(KeepDecimals<1>(getFrontDistance(idx)), 'f', 1) + " mm"},
            {QObject::tr("零点"), QString::number(KeepDecimals(getZeroPointBias(idx)), 'f', 2) + " μs"},
            {QObject::tr("延时"), QString::number(KeepDecimals<1>(getSamplingDelay(idx)), 'f', 1) + " mm"},
            {QObject::tr("声速"), QString::number(KeepDecimals<0>(getSoundVelocity(idx)), 'f', 0) + " m/s"},
            {QObject::tr("通道"), QString::number(getChannel(idx))},
            {QObject::tr("K值"), QString::number(Union::Base::Probe::Degree2K(getAngle(idx)), 'f', 2)},
            {QObject::tr("抑制"), QString::number(getSupression(idx)) + "%"},
            {QObject::tr("角度"), QString::number(KeepDecimals<1>(getAngle(idx)), 'f', 1) + "°"},
        };
        return {
            {QObject::tr("增益参数"), gainPrarameter},
            {QObject::tr("探头信息"), probeParameter},
            {QObject::tr("基本信息"), basicParameter},
        };
    }

    double AScanIntf::getNearField(int idx) const {
        auto [l, w] = getProbeSize(idx);
        return Union::CalculateNearField(l, w, getProbeFrequence(idx), getSoundVelocity(idx));
    }

    std::function<double(double)> AScanIntf::getAVGLineExpr(int idx) const {
        auto avg_param = getAVG(idx);
        if (!avg_param.has_value()) {
            return [](double) -> double { return 0.0; };
        }
        std::vector<double> index_on_dac_view;
        index_on_dac_view.resize(avg_param->index.size());
        std::transform(avg_param->index.begin(), avg_param->index.end(), index_on_dac_view.begin(), [&](int x) -> double {
            return ValueMap((double)x, {0.0, 160.0}, {0.0, 520.0});
        });
        auto modifyGain = getBaseGain(idx) + getScanGain(idx) + getSurfaceCompentationGain(idx) - getAVG(idx)->baseGain + getAVG(idx)->biasGain;
        if (avg_param->index.size() == 1) {
            return [=, this](double _val) -> double {
                auto val = ValueMap((double)_val, getAxisRange(idx), {0.0, 520.0});
                if (val < index_on_dac_view[0]) {
                    return CalculateGainOutput(avg_param->value[0], modifyGain);
                } else {
                    auto Func = std::bind(Union::EchoDbDiffOfHole, std::placeholders::_1, 2.0, std::placeholders::_2, 2.0);
                    return CalculateGainOutput(avg_param->value[0], modifyGain - Func(index_on_dac_view[0], val));
                }
            };
        }
        std::vector<double> decay = {};
        for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
            auto _decay = (std::log(avg_param->value[i + 1]) - std::log(avg_param->value[i])) / (index_on_dac_view[i + 1] - index_on_dac_view[i]);
            decay.emplace_back(_decay);
        }
        return [=, this](double _val) -> double {
            auto val = ValueMap((double)_val, getAxisRange(idx), {0.0, 520.0});
            if (val < index_on_dac_view[0]) {
                return CalculateGainOutput(avg_param->value[0], modifyGain);
            } else {
                for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
                    if (val >= index_on_dac_view[i]) {
                        return CalculateGainOutput(avg_param->value[i] * std::exp(decay[i] * (val - index_on_dac_view[i])), modifyGain);
                    }
                }
            }
            return 0.0;
        };
    }

    std::function<double(double)> AScanIntf::getDACLineExpr(int idx) const {
        auto dac_param = getDAC(idx);
        if (!dac_param.has_value()) {
            return [](double) -> double { return 0.0; };
        }
        std::vector<double> index_on_dac_view;
        index_on_dac_view.resize(dac_param->index.size());
        std::transform(dac_param->index.begin(), dac_param->index.end(), index_on_dac_view.begin(), [&](int x) -> double {
            return ValueMap((double)x, {0.0, 100.0}, {0.0, 520.0});
        });
        auto modifyGain = getBaseGain(idx) + getScanGain(idx) - getDAC(idx)->baseGain + getDAC(idx)->biasGain;
        if (dac_param->index.size() == 1) {
            return [=, this](double _val) -> double {
                auto val = ValueMap((double)_val, getAxisRange(idx), {0.0, 520.0});
                if (val < index_on_dac_view[0]) {
                    return CalculateGainOutput(dac_param->value[0], modifyGain);
                } else {
                    return CalculateGainOutput(dac_param->value[0], modifyGain + Union::EchoDbDiffOfPlan(index_on_dac_view[0], val));
                }
            };
        }
        std::vector<double> decay = {};
        for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
            auto _decay = (std::log(dac_param->value[i + 1]) - std::log(dac_param->value[i])) / (index_on_dac_view[i + 1] - index_on_dac_view[i]);
            decay.emplace_back(_decay);
        }
        return [=, this](double _val) -> double {
            auto val = ValueMap((double)_val, getAxisRange(idx), {0.0, 520.0});
            if (val < index_on_dac_view[0]) {
                return CalculateGainOutput(dac_param->value[0], modifyGain);
            } else {
                for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
                    if (val >= index_on_dac_view[i]) {
                        return CalculateGainOutput(dac_param->value[i] * std::exp(decay[i] * (val - index_on_dac_view[i])), modifyGain);
                    }
                }
            }
            return 0.0;
        };
    }

    int AScanIntf::getReplayTimerInterval() const {
        return 40;
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
        if (enable_supression && ((*max) <= (2.0 * (getSupression(idx))))) {
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
