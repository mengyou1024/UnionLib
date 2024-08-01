#include "AScanType.hpp"
#include <QLoggingCategory>
#include <QMessageBox>
#include <QString>
#include <cmath>
#include <tuple>

static Q_LOGGING_CATEGORY(TAG, "ASCAN.INTF");

namespace Union::AScan {
    QList<QPointF> AScanIntf::getAScanSeriesData(int index, double softGain) const {
        QList<QPointF> ret;
        const auto     AScanStep = getAxisLen(index) / static_cast<double>(getScanData(index).size());
        for (auto i = 0; std::cmp_less(i, getScanData(index).size()); i++) {
            auto ampValue = getScanData(index)[i] / 2.0;
            if (ampValue < getSupression(index)) {
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
            {QObject::tr("回波抑制"), QString::number(getSupression(idx), 'f', 1) + "%"},
            {QObject::tr("回波延时"), QString::number(KeepDecimals<1>(getSamplingDelay(idx)), 'f', 1) + "mm"},
            {QObject::tr("声程范围"), soundDistance},
            {QObject::tr("声速"), QString::number(KeepDecimals<0>(getSoundVelocity(idx)), 'f', 0) + "m/s"},
            {QObject::tr("距离"), gateValue.at(0).toObject()["dist_c"].toString()},
            {QObject::tr("水平"), gateValue.at(0).toObject()["dist_a"].toString()},
            {QObject::tr("垂直"), gateValue.at(0).toObject()["dist_b"].toString()},
            {QObject::tr("当量"), gateValue.at(0).toObject()["equi"].toString()},
            {QObject::tr("长度"), ""},
            {QObject::tr("高度"), gateValue.at(0).toObject()["amp"].toString()},
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

            b = Union::ValueMap(loc, getAxisRange(idx));
            c = b;

            if (isStraightBeamProbe(idx)) {
                if (getDistanceMode(idx) == Union::AScan::DistanceMode::DistanceMode_X) {
                    auto msg = QObject::tr("当探头为直探头时, 使用声程模式X");
                    qCWarning(TAG).noquote() << msg;
                    std::call_once(m_once_flag, [&msg]() {
                        QMessageBox::warning(nullptr, QObject::tr("警告"), msg);
                    });
                }
            } else {
                if (!(std::abs(getAngle(idx)) > 0.0001)) {
                    auto msg = QObject::tr("当探头为斜探头时, 探头角度近乎为0");
                    qCCritical(TAG).noquote() << msg;
                    std::call_once(m_once_flag, [&msg]() {
                        QMessageBox::warning(nullptr, QObject::tr("警告"), msg);
                    });
                }
                const auto rad = Union::Base::Probe::Degree2Rd(getAngle(idx));
                const auto k   = Union::Base::Probe::Degree2K(getAngle(idx));
                try {
                    switch (getDistanceMode(idx)) {
                        case Union::AScan::DistanceMode::DistanceMode_Y: {
                            b = Union::ValueMap(loc, getAxisRange(idx));
                            a = b.value() * k;
                            c = b.value() / std::cos(rad);
                            break;
                        }
                        case Union::AScan::DistanceMode::DistanceMode_X: {
                            a = Union::ValueMap(loc, getAxisRange(idx));
                            b = b.value() / k;
                            c = a.value() / std::sin(rad);
                            break;
                        }
                        case Union::AScan::DistanceMode::DistanceMode_S: {
                            c = Union::ValueMap(loc, getAxisRange(idx));
                            a = c.value() * std::sin(rad);
                            b = c.value() * std::cos(rad);
                            break;
                        }
                    }
                } catch (std::exception &e) {
                    qCCritical(TAG) << e.what();
                }
                a = a.value_or(0) - getFrontDistance(idx);
            }

            auto workPieceThickness = getWorkPieceThickness(idx);

            if (workPieceThickness.has_value() && (workPieceThickness.value() > 0.0) && b.has_value()) {
                auto numberOfRefraction = static_cast<int>(std::floor(b.value() / workPieceThickness.value()));
                auto isOdd              = numberOfRefraction % 2 == 1;

                if (isOdd) {
                    // 奇数次反射, 深度 = 工件厚度 - (计算值 % 工件厚度)
                    auto b_value = b.value();
                    auto t_value = workPieceThickness.value();
                    b            = workPieceThickness.value() - (std::fmod(b_value, t_value));
                } else {
                    // 偶数次反射, 深度 =  (计算值 % 工件厚度)
                    auto b_value = b.value();
                    auto t_value = workPieceThickness.value();
                    b            = std::fmod(b_value, t_value);
                }
            }

            if (a) {
                _a = QString::number(KeepDecimals<1>(a.value_or(0)), 'f', 1);
            }

            if (b) {
                _b = QString::number(KeepDecimals<1>(b.value_or(0)), 'f', 1);
            }

            if (c) {
                _c = QString::number(KeepDecimals<1>(c.value_or(0)), 'f', 1);
            }

            QString _equi = "-";

            auto _gate_amp = amp / 2.0;
            if (_gate_amp > 100.0) {
                _gate_amp = 100.0;
            }

            _gateValue[i] = {
                {"amp", QString::number(_gate_amp, 'f', 1) + "%"},
                {"dist_a", _a},
                {"dist_b", _b},
                {"dist_c", _c},
                {"equi", _equi},
            };
        }

        return QJsonArray::fromVariantList({_gateValue.at(0), _gateValue.at(1)});
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
            {QObject::tr("K值/角度"), QString::number(Union::Base::Probe::Degree2K(getAngle(idx)), 'f', 2) +
                                          "/" + QString::number(KeepDecimals<1>(getAngle(idx)), 'f', 1) + "°"},
            {QObject::tr("抑制"), QString::number(getSupression(idx), 'f', 1) + "%"},
        };

        auto workPieceThickness = getWorkPieceThickness(idx);
        if (workPieceThickness.has_value()) {
            basicParameter.insert("工件厚度", QString::number(KeepDecimals<1>(workPieceThickness.value()), 'f', 1) + " mm");
        }

        return {
            {QObject::tr("增益参数"), gainPrarameter},
            {QObject::tr("探头信息"), probeParameter},
            {QObject::tr("基本信息"), basicParameter},
        };
    }

    bool AScanIntf::getReportEnable() const {
        return true;
    }

    bool AScanIntf::getDateEnable() const {
        return true;
    }

    std::optional<double> AScanIntf::getWorkPieceThickness(int idx) const {
        (void)idx;
        return std::nullopt;
    }

    double AScanIntf::getNearField(int idx) const {
        auto [l, w] = getProbeSize(idx);
        return Union::CalculateNearField(l, w, getProbeFrequence(idx), getSoundVelocity(idx));
    }

    bool AScanIntf::isStraightBeamProbe(int idx) const {
        return Union::Base::Probe::IsStraightBeamProbe(getProbe(idx));
    }

    std::function<std::optional<double>(double)> AScanIntf::getAVGLineExpr(int idx) const {
        try {
            auto   avg_param     = getAVG(idx);
            double AVG_ECHO_SIZE = getScanData(idx).size();
            if (!avg_param.has_value()) {
                return [](double) -> double { return 0.0; };
            }
            std::vector<double> index_on_dac_view;
            index_on_dac_view.resize(avg_param->index.size());
            std::transform(avg_param->index.begin(), avg_param->index.end(), index_on_dac_view.begin(), [&](double x) -> double {
                return ValueMap(x, {0.0, 250.0}, {0.0, AVG_ECHO_SIZE});
            });
            auto modifyGain = getBaseGain(idx) + getScanGain(idx) + getSurfaceCompentationGain(idx) - avg_param->baseGain - avg_param->biasGain;
            if (avg_param->index.size() == 1) {
                return [=, this](double _val) -> std::optional<double> {
                    auto val             = ValueMap(_val, getAxisRange(idx), {0.0, AVG_ECHO_SIZE});
                    auto three_nearField = 3 * getNearField(idx);
                    if (val < three_nearField) {
                        auto Func = std::bind(Union::EchoDbDiffOfHole, std::placeholders::_1, 2.0, std::placeholders::_2, 2.0);
                        return CalculateGainOutput(avg_param->value.at(0), modifyGain - Func(index_on_dac_view.at(0), three_nearField));
                    } else {
                        auto Func = std::bind(Union::EchoDbDiffOfHole, std::placeholders::_1, 2.0, std::placeholders::_2, 2.0);
                        return CalculateGainOutput(avg_param->value.at(0), modifyGain - Func(index_on_dac_view.at(0), val));
                    }
                };
            }
            std::vector<double> decay = {};
            for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
                auto _decay = (std::log(avg_param->value[i + 1]) - std::log(avg_param->value[i])) / (index_on_dac_view[i + 1] - index_on_dac_view[i]);
                decay.emplace_back(_decay);
            }
            return [=, this](double _val) -> std::optional<double> {
                auto val = ValueMap((double)_val, getAxisRange(idx), {0.0, AVG_ECHO_SIZE});
                if (val < index_on_dac_view.at(0)) {
                    return CalculateGainOutput(avg_param->value.at(0), modifyGain);
                } else {
                    for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
                        if (val < index_on_dac_view[i + 1]) {
                            return CalculateGainOutput(avg_param->value[i] * std::exp(decay[i] * (val - index_on_dac_view[i])), modifyGain);
                        }
                    }
                }
                return std::nullopt;
            };
        } catch (std::exception &e) {
            qCWarning(TAG) << e.what();
            return [](double) -> std::optional<double> {
                return std::nullopt;
            };
        }
    }

    std::function<std::optional<double>(double)> AScanIntf::getDACLineExpr(int idx) const {
        try {
            auto   dac_param     = getDAC(idx);
            double DAC_ECHO_SIZE = getScanData(idx).size();
            if (!dac_param.has_value()) {
                return [](double) -> double { return 0.0; };
            }
            std::vector<double> index_on_dac_view;
            index_on_dac_view.resize(dac_param->index.size());
            std::transform(dac_param->index.begin(), dac_param->index.end(), index_on_dac_view.begin(), [&](double x) -> double {
                return ValueMap(x, {0.0, 100.0}, {0.0, DAC_ECHO_SIZE});
            });
            auto modifyGain = getBaseGain(idx) + getScanGain(idx) - dac_param->baseGain - dac_param->biasGain;
            if (dac_param->index.size() == 1) {
                return [=, this](double _val) -> double {
                    auto val             = ValueMap(_val, getAxisRange(idx), {0.0, DAC_ECHO_SIZE});
                    auto three_nearField = 3 * getNearField(idx);
                    if (val < three_nearField) {
                        return CalculateGainOutput(dac_param->value.at(0), modifyGain + Union::EchoDbDiffOfPlan(index_on_dac_view.at(0), three_nearField));
                    } else {
                        return CalculateGainOutput(dac_param->value.at(0), modifyGain + Union::EchoDbDiffOfPlan(index_on_dac_view.at(0), val));
                    }
                };
            }

            std::vector<double> decay = {};
            for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
                auto _decay = (std::log(dac_param->value.at(i + 1)) - std::log(dac_param->value.at(i))) / (index_on_dac_view.at(i + 1) - index_on_dac_view.at(i));
                decay.emplace_back(_decay);
            }
            return [=, this](double _val) -> std::optional<double> {
                auto val = ValueMap((double)_val, getAxisRange(idx), {0.0, DAC_ECHO_SIZE});
                if (val < index_on_dac_view.at(0)) {
                    return CalculateGainOutput(dac_param->value.at(0), modifyGain);
                } else {
                    for (auto i = 0; std::cmp_less(i, index_on_dac_view.size() - 1); i++) {
                        if (val < index_on_dac_view.at(i + 1)) {
                            return CalculateGainOutput(dac_param->value.at(i) * std::exp(decay.at(i) * (val - index_on_dac_view.at(i))), modifyGain);
                        }
                    }
                }
                return std::nullopt;
            };
        } catch (std::exception &e) {
            qCWarning(TAG) << e.what();
            return [](double) -> std::optional<double> {
                return std::nullopt;
            };
        }
    }

    int AScanIntf::getReplayTimerInterval() const {
        return 40;
    }

    std::optional<std::tuple<double, uint8_t>> AScanIntf::getGateResult(int idx, int gate_idx, bool find_center_if_overflow, bool enable_supression) const {
        const auto           &_data       = getScanData(idx);
        const auto            _gate       = getGate(idx).at(gate_idx);
        std::optional<double> _supression = std::nullopt;
        if (enable_supression) {
            _supression = getSupression(idx);
        }
        return Union::Base::CalculateGateResult(_data, _gate, find_center_if_overflow, _supression);
    }

    std::pair<double, double> AScanIntf::getAxisRange(int idx) const {
        return std::make_pair(getAxisBias(idx), getAxisBias(idx) + getAxisLen(idx));
    }

} // namespace Union::AScan
