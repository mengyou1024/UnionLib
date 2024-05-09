#pragma once

#include "../common/AVG.hpp"
#include "../common/DAC.hpp"
#include "../common/FileReaderSelector.hpp"
#include "../common/Gate.hpp"
#include "../common/Performance.hpp"
#include "../common/common.hpp"
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QMap>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QString>
#include <QVariant>
#include <QtCore>
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace Union::AScan {
    enum DistanceMode {
        DistanceMode_Y,
        DistanceMode_X,
        DistanceMode_S,
    };

    struct DAC_Standard {
        double rlBias = -4.0;
        double slBias = -12.0;
        double elBias = -18.0;
    };

    class AScanIntf : public Union::Utils::ReadIntf {
    public:
        virtual ~AScanIntf() = default;

        /**
         * @brief 读取文件接口
         *
         * @param file 文件输入刘
         * @param file_size 文件的大小
         * @return size_t 已取读的长度
         */
        virtual size_t __Read(std::ifstream &file, size_t file_size) = 0;

        /**
         * @brief 获取A扫图像的大小
         *
         * @return int
         */
        virtual int getDataSize(void) const = 0;

        /**
         * @brief 获取文件列表
         *        连续图像可能有多个文件，依次返回，如果连续图像仅为一个文件，则可返回空vector
         *        目前仅在330(.dat)中使用 @ref DATType
         *
         * @return std::vecotr<std::wstring>
         */
        virtual std::vector<std::wstring> getFileNameList(void) const = 0;

        /**
         * @brief 设置文件名索引
         *        控制当前文件的索引，如果该连续文件只存储了一个文件可以什么也不做
         *        目前仅在330(.dat)中使用 @ref DATType
         *
         * @param index 文件索引序号
         */
        virtual void setFileNameIndex(int index) = 0;

        // AScanType
        /**
         * @brief 获取仪器的五大性能
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return Base::Performance
         */
        virtual Base::Performance getPerformance(int idx) const = 0;

        /**
         * @brief 获取日期
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return std::string
         */
        virtual std::string getDate(int idx) const = 0;

        /**
         * @brief 获取探头
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return std::wstring
         */
        virtual std::wstring getProbe(int idx) const = 0;

        /**
         * @brief 获取探头频率
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double
         */
        virtual double getProbeFrequence(int idx) const = 0;

        /**
         * @brief 获取探头形状
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return std::string
         */
        virtual std::string getProbeChipShape(int idx) const = 0;

        /**
         * @brief 获取探头角度
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double
         */
        virtual double getAngle(int idx) const = 0;

        /**
         * @brief 获取声速(m/s)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(m/s)
         */
        virtual double getSoundVelocity(int idx) const = 0;

        /**
         * @brief 获取前沿(mm)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(mm)
         */
        virtual double getFrontDistance(int idx) const = 0;

        /**
         * @brief 获取零点(μs)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(μs)
         */
        virtual double getZeroPointBias(int idx) const = 0;

        /**
         * @brief 获取采样延时(μs)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(μs)
         */
        virtual double getSamplingDelay(int idx) const = 0;

        /**
         * @brief 获取通道号
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return int -1表示没有通道信息
         */
        virtual int getChannel(int idx) const = 0;

        /**
         * @brief 获取仪器名称
         *
         * @return std::string
         */
        virtual std::string getInstrumentName(void) const = 0;
        // AScanData

        /**
         * @brief 获取波门
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return std::array<Base::Gate, 2>
         */
        virtual std::array<Base::Gate, 2> getGate(int idx) const = 0;

        /**
         * @brief 获取A扫数据
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return const std::vector<uint8_t> &
         */
        virtual const std::vector<uint8_t> &getScanData(int idx) const = 0;

        /**
         * @brief 获取坐标轴的起始偏移(mm)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(mm)
         */
        virtual double getAxisBias(int idx) const = 0;

        /**
         * @brief 获取坐标轴长度(mm)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(mm)
         */
        virtual double getAxisLen(int idx) const = 0;

        /**
         * @brief 获取增益(dB)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(dB)
         */
        virtual double getBaseGain(int idx) const = 0;

        /**
         * @brief 获取扫查增益(dB)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(dB)
         */
        virtual double getScanGain(int idx) const = 0;

        /**
         * @brief 获取表面补偿增益(dB)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(dB)
         */
        virtual double getSurfaceCompentationGain(int idx = 0) const = 0;

        /**
         * @brief 获取抑制
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return int 0-100
         */
        virtual int getSupression(int idx) const = 0;

        /**
         * @brief 获取声程模式
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return DistanceMode
         */
        virtual DistanceMode getDistanceMode(int idx) const = 0;

        /**
         * @brief 获取AVG参数
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return Base::AVG
         */
        virtual std::optional<Base::AVG> getAVG(int idx) const = 0;

        /**
         * @brief 获取DAC参数
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return Base::DAC
         */
        virtual std::optional<Base::DAC> getDAC(int idx) const = 0;

        /**
         * @brief 获取DAC曲线的标准
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return DAC_Standard
         */
        virtual DAC_Standard getDACStandard(int idx) const = 0;

        /**
         * @brief 获取AVG曲线的表达式
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         *
         * @return 返回函数表达式, 输入参数为DAC曲线的X, 输出参数为DAC曲线的Y
         *         图像中曲线的坐标系X轴范围为: [0~N] 其中N为(A扫图像的点数-1)
         *         图像中曲线的坐标系Y轴范围为: [0~200]
         */
        virtual std::function<double(double)> getAVGLineExpr(int idx) const = 0;

        /**
         * @brief 获取AVG曲线的表达式
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         *
         * @return 返回函数表达式, 输入参数为DAC曲线的X, 输出参数为DAC曲线的Y
         *         图像中曲线的坐标系X轴范围为: [0~N] 其中N为(A扫图像的点数-1)
         *         图像中曲线的坐标系Y轴范围为: [0~200]
         */
        virtual std::function<double(double)> getDACLineExpr(int idx) const = 0;

        virtual std::function<double(double)> getLineExpr(int idx, const std::vector<int> &_index, const std::vector<uint8_t> &value,
                                                          const std::array<double, 2>          &r_range      = {0.0, 105.0} /*TODO: 声程计算*/,
                                                          const std::array<double, 2>          &v_range      = {0, 520.0},
                                                          std::function<double(double, double)> func_db_diff = Union::EchoDbDiffOfPlan) const final;
        /**
         * @brief 获取波门的计算结果
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param gate_idx 波门序号 0-2
         * @param find_center_if_overflow true:在最高波超过屏幕宽度时寻找中心， false: 寻找第一个最高位置
         * @return std::optional<std::tuple<double, uint8_t>>
         *         [pos, max_amp]
         *         其中pos的范围为:0-1
         */
        virtual std::optional<std::tuple<double, uint8_t>> getGateResult(int idx, int gate_idx = 0, bool find_center_if_overflow = true) const final;

        /**
         * @brief 获取坐标范围
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return [bias, bias+len]
         *         其中bias为`getAxisBias`返回值, len为`getAxisLen`返回值
         */
        virtual std::array<double, 2> getAxisRange(int idx) const final;

        /**
         * @brief 获取QLineSeries.replace可使用的A扫数据
         *
         * @param index 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param softGain 软件增益
         * @return QList<QPointF>
         */
        virtual QList<QPointF> getAScanSeriesData(int index = 0, double softGain = 0.0) const final;

        /**
         * @brief 创建报表Map
         *
         * @param idx idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param soft_gain 软件增益
         * @return QVariantMap
         */
        virtual QVariantMap createReportValueMap(int idx, double soft_gain) const {
            auto    gateValue     = createGateValue(idx, soft_gain);
            auto    sensitivity   = QString("%1+%2+%3dB").arg(QString::number(getBaseGain(idx), 'f', 1), QString::number(getScanGain(idx), 'f', 1), QString::number(getSurfaceCompentationGain(idx), 'f', 1));
            QString soundDistance = QString::number(getAxisLen(idx), 'f', 1) + "mm";
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
                {QObject::tr("前沿"), QString::number(getFrontDistance(idx), 'f', 1) + "mm"},
                {QObject::tr("探头K值"), QString::number(Union::Base::Probe::Degree2K(getAngle(idx)), 'f', 2)},
                {QObject::tr("频率"), QString::number(getProbeFrequence(idx), 'f', 2) + "MHz"},
                {QObject::tr("折射角"), QString::number(getAngle(idx), 'f', 1) + "°"},
                {QObject::tr("零点"), QString::number(getZeroPointBias(idx), 'f', 2) + "μs"},
                {QObject::tr("仪器型号"), QString::fromStdString(getInstrumentName())},
                {QObject::tr("灵敏度"), sensitivity},
                {QObject::tr("回波抑制"), QString::number(getSupression(idx)) + "%"},
                {QObject::tr("回波延时"), QString::number(getSamplingDelay(idx), 'f', 1) + "mm"},
                {QObject::tr("声程范围"), soundDistance},
                {QObject::tr("声速"), QString::number(getSoundVelocity(idx), 'f', 0) + "m/s"},
                {QObject::tr("距离"), gateValue[0].toObject()["dist_c"].toString()},
                {QObject::tr("水平"), gateValue[0].toObject()["dist_a"].toString()},
                {QObject::tr("垂直"), gateValue[0].toObject()["dist_b"].toString()},
                {QObject::tr("当量"), gateValue[0].toObject()["equi"].toString()},
                {QObject::tr("长度"), ""},
                {QObject::tr("高度"), ""},
                {QObject::tr("等级"), ""},
                {QObject::tr("探伤结果"), ""},
                {QObject::tr("探伤标准"), ""},
                {QObject::tr("探伤人员"), ""},
                {QObject::tr("负责人员"), ""},
                {QObject::tr("备注"), ""},
            };
            return vmp;
        }

        /**
         * @brief 创建波门结果值
         *
         * @param idx idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param soft_gain 软件增益
         * @return QJsonArray
         */
        virtual QJsonArray createGateValue(int idx, double soft_gain) const {
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
                            throw std::runtime_error("The Angle of refraction cannot be less than 0 in X path mode");
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
                    _a = QString::number(a.value(), 'f', 1);
                }

                if (b) {
                    _b = QString::number(b.value(), 'f', 1);
                }

                if (c) {
                    _c = QString::number(c.value(), 'f', 1);
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
                    _equi   = QString::asprintf("SL%+.1fdB", Union::CalculatedGain(slValue, r_amp));
                } else if (getAVG(idx) && b.has_value()) {
                    auto r_amp      = Union::CalculateGainOutput(_amp, getSurfaceCompentationGain(idx));
                    auto lineExpr   = getAVGLineExpr(idx);
                    auto slValue    = lineExpr((b.value() - getAxisBias(idx)) / getAxisLen(idx) * getScanData(idx).size());
                    auto modifyGain = getBaseGain(idx) + getScanGain(idx) +
                                      getSurfaceCompentationGain(idx) - getAVG(idx)->baseGain +
                                      getDACStandard(idx).slBias;
                    slValue = Union::CalculateGainOutput(slValue, modifyGain);
                    _equi   = QString::asprintf("Φ%+.1fdB", Union::CalculatedGain(slValue, r_amp));
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

        /**
         * @brief 创建工艺参数
         *
         * @param idx idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return 工艺参数
         */
        virtual QVariantMap createTechnologicalParameter(int idx) const {
            QVariantMap gainPrarameter = {
                {QObject::tr("基本增益"), QString::number(getBaseGain(idx),                'f', 1) + " dB"},
                {QObject::tr("扫查增益"), QString::number(getScanGain(idx),                'f', 1) + " dB"},
                {QObject::tr("表面补偿"), QString::number(getSurfaceCompentationGain(idx), 'f', 1) + " dB"},
            };

            QVariantMap probeParameter = {
                {QObject::tr("探头类型"), QString::fromStdWString(getProbe(idx))},
                {QObject::tr("探头频率"), QString::number(getProbeFrequence(idx), 'f', 1) + " MHz"},
                {QObject::tr("晶片尺寸"), QString::fromStdString(getProbeChipShape(idx))},
            };

            QVariantMap basicParameter = {
                {QObject::tr("声程"), QString::number(getAxisLen(idx), 'f', 1) + " mm"},
                {QObject::tr("前沿"), QString::number(getFrontDistance(idx), 'f', 1) + " mm"},
                {QObject::tr("零点"), QString::number(getZeroPointBias(idx), 'f', 2) + " μs"},
                {QObject::tr("延时"), QString::number(getSamplingDelay(idx), 'f', 1) + " mm"},
                {QObject::tr("声速"), QString::number(getSoundVelocity(idx), 'f', 0) + " m/s"},
                {QObject::tr("通道"), QString::number(getChannel(idx))},
                {QObject::tr("K值"), QString::number(Union::Base::Probe::Degree2K(getAngle(idx)), 'f', 2)},
                {QObject::tr("抑制"), QString::number(getSupression(idx)) + "%"},
                {QObject::tr("角度"), QString::number(getAngle(idx), 'f', 1) + "°"},
            };
            return {
                {QObject::tr("增益参数"), gainPrarameter},
                {QObject::tr("探头信息"), probeParameter},
                {QObject::tr("基本信息"), basicParameter},
            };
        }
    };

    /**
     * @brief A扫类型的UI名称
     *
     */
    inline static constexpr std::string_view ASCAN_I_NAME = "AScan";

    /**
     * @brief A扫类型的文件选择器
     *
     */
    using AScanFileSelector = Union::Utils::FileReaderSelector<AScanIntf, ASCAN_I_NAME>;

} // namespace Union::AScan
