#pragma once

#include "./common/AVG.hpp"
#include "./common/DAC.hpp"
#include "./common/Gate.hpp"
#include "./common/Performance.hpp"
#include "./common/common.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#if __has_include("QtCore")
    #include <QFileInfo>
    #include <QJsonArray>
    #include <QJsonObject>
    #include <QLoggingCategory>
    #include <QRegularExpression>
    #include <QRegularExpressionMatch>
    #include <QRegularExpressionMatchIterator>
    #include <QString>
    #include <QtCore>
#endif

#if __has_include("NumCpp/Polynomial/Poly1d.hpp")
    #include <NumCpp/Polynomial/Poly1d.hpp>
#endif

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

    struct AScanData {
        std::array<Base::Gate, 2> gate                    = {}; ///< 波门
        std::vector<uint8_t>      ascan                   = {}; ///< A扫数据
        double                    axisBias                = {}; ///< 坐标轴偏移
        double                    axisLen                 = {}; ///< 坐标轴长度
        double                    baseGain                = {}; ///< 基本增益
        double                    scanGain                = {}; ///< 扫查增益
        double                    surfaceCompentationGain = {}; ///< 表面补偿增益
        int                       suppression             = {}; ///< 抑制
        DistanceMode              distanceMode            = {}; ///< 声程模式
        std::optional<Base::AVG>  avg                     = {}; ///< AVG曲线
        std::optional<Base::DAC>  dac                     = {}; ///< DAC曲线
        DAC_Standard              std                     = {}; ///< DAC标准

        /**
         * @brief 获取波门的计算结果
         *
         * @param gateIndex 波们索引
         * @param find_center_if_overflow 最高波溢出时寻找中心位置
         *
         * @return optional([最高回波位置(0-1.0), 回波强度(0-255)])
         */
        std::optional<std::tuple<double, uint8_t>> GetGateResult(int gateIndex = 0, bool find_center_if_overflow = true) const {
            if (gateIndex > this->gate.size()) {
                return std::nullopt;
            }
            const auto &data  = ascan;
            const auto &gate  = this->gate[gateIndex];
            auto        start = gate.pos;
            double      end   = (double)(gate.pos + gate.width);
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
                max = minMaxVec[0] + minMaxVec.size() / 2;
            }
            auto pos = ((double)std::distance(data.begin(), max) / (double)data.size());
            if (pos < 0.0f) {
                return std::nullopt;
            }
            return std::make_tuple(pos, *max);
        }

        /**
         * @brief 获取坐标轴的生产范围
         *
         * @return [bias, bias + len]
         */
        std::array<double, 2> GetAxisRange(void) const {
            return std::array<double, 2>({axisBias, axisBias + axisLen});
        }

        std::function<double(double)> getAVGLineExpr(void) const {
            auto func = std::bind(Union::EchoDbDiffOfHole, std::placeholders::_1, 2.0, std::placeholders::_2, 2.0);
            return getLineExpr(avg->index, avg->value, {0.0, 160.0}, {0.0, 520.0}, func);
        }

        std::function<double(double)> getDACLineExpr(void) const {
            return getLineExpr(dac->index, dac->value, {0.0, 106.59}, {0.0, 520.0});
        }

    private:
        std::function<double(double)> getLineExpr(const std::vector<int> &_index, const std::vector<uint8_t> &value,
                                                  const std::array<double, 2>          &r_range      = {0.0, 105.0} /*TODO: 声程计算*/,
                                                  const std::array<double, 2>          &v_range      = {0, 520.0},
                                                  std::function<double(double, double)> func_db_diff = Union::EchoDbDiffOfPlan) const {
            // TODO: 曲线平滑
            using Union::ValueMap;
            auto TAG = QLoggingCategory("Union");

            std::vector<double> index;
            for (auto &it : _index) {
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
                        for (int i = 1; i < index.size(); i++) {
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

                    auto CalcluateValue = [=, this](std::pair<double, double> pt) -> double {
                        auto delta_db = func_db_diff(val, pt.first);
                        return Union::CalculateGainOutput(pt.second, delta_db);
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
    };

    struct AScanType {
        std::vector<AScanData> data           = {};
        Base::Performance      performance    = {}; ///< 性能
        std::string            time           = {}; ///< 时间
        std::wstring           probe          = {}; ///< 探头
        double                 probeFrequence = {}; ///< 探头频率
        std::string            probeChipShape = {}; ///< 探头晶片尺寸
        double                 angle          = {}; ///< 角度
        double                 soundVelocity  = {}; ///< 声速
        double                 frontDistance  = {}; ///< 前沿
        double                 zeroPointBias  = {}; ///< 零点偏移
        double                 samplingDelay  = {}; ///< 采样延迟
        int                    channel        = {}; ///< 通道
        std::string            instrumentName = {}; ///< 仪器型号
        #if __has_include("QtCore")
            static QJsonObject    GetBoardParamJSON(const AScanType &val);
            static QList<QPointF> GetAScanSeriesData(const AScanData &data, double softGain = 0.0);
            QJsonObject    GetBoardParamJSON(void) const;
            QList<QPointF> GetAScanSeriesData(int index = 0, double softGain = 0.0) const;
        #endif
    };



    template <class T>
    std::optional<AScanType> CONVERT_TO_STANDARD_ASCAN_TYPE(const T &data) {
#if __has_include("QtCore")
        qDebug() << "type:" << typeid(decltype(data)).name() << "convert error";
#endif
        return std::nullopt;
    }

#if __has_include("QtCore")
    class AScanFileSelector {
    public:
        /**
         * @brief A扫文件选择器数据类型
         * [reader, ui_name, description]
         */
        using AFS_RFUNC = std::function<std::optional<AScanType>(const std::wstring &)>;
        using AFS_DTYPE = std::tuple<AFS_RFUNC, std::string>;

        AScanFileSelector()                                     = delete;
        AScanFileSelector(const AScanFileSelector &)            = delete;
        AScanFileSelector(AScanFileSelector &&)                 = delete;
        AScanFileSelector &operator=(const AScanFileSelector &) = delete;
        AScanFileSelector &operator=(AScanFileSelector &&)      = delete;

        static bool                           RegistReader(std::string_view file_suffix, std::string_view describe, const AFS_RFUNC &func);
        static const std::optional<AFS_DTYPE> Get(const std::string &file_suffix);

        static QJsonArray                     GetFileNameFilter();
        static QJsonArray                     GetFileListModelNameFilter();
        static QJsonObject                    GetUINameMap();
        static const std::optional<AFS_RFUNC> GetReadFunction(const std::wstring &fileName);

    private:
        inline static std::map<std::string, AFS_DTYPE> data = {};
    };

    template <class T = double>
    QList<QPointF> Func2PointList(std::function<T(T)> f, T start, T end, T step) {
        QList<QPointF> ret;
        for (T i = start; i <= end; i += step) {
            ret.emplace_back(QPointF{i, f(i)});
        }
        return ret;
    }
#endif
} // namespace Union::AScan
