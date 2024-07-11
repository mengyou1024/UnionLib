#pragma once

#include "../common/AVG.hpp"
#include "../common/DAC.hpp"
#include "../common/FileReaderSelector.hpp"
#include "../common/Gate.hpp"
#include "../common/Performance.hpp"
#include "../common/union_common.hpp"
#include "./Specialization.hpp"
#include <QFileInfo>
#include <QImage>
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
    class AScanIntf : public Union::Utils::ReadIntf {
    public:
        virtual ~AScanIntf() = default;

        ////////////////////////////////////////////////////// 纯虚函数 //////////////////////////////////////////////////////

        /**
         * @brief 读取文件接口
         *
         * @param file 文件输入刘
         * @param file_size 文件的大小
         * @return size_t 已取读的长度
         */
        virtual size_t __Read(std::ifstream &file, size_t file_size) = 0;

        /**
         * @brief 获取A扫图像的大小(连续图像中使用)
         *        eg. 当连续图像有100帧时, 则返回100
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
         * @brief 获取采样延时(mm)
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return double(mm)
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
        virtual double getSurfaceCompentationGain(int idx) const = 0;

        /**
         * @brief 获取抑制
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return int 0-100
         */
        virtual double getSupression(int idx) const = 0;

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
         * @brief 获取探头尺寸
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return [l, w] or [d, 0]
         */
        virtual std::pair<double, double> getProbeSize(int idx) const = 0;

        ////////////////////////////////////////////////////// 带默认实现的虚函数 //////////////////////////////////////////////////////

        /**
         * @brief 获取AVG曲线的表达式
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         *
         * @return 返回函数表达式, 输入参数为DAC曲线的X, 输出参数为DAC曲线的Y
         *         图像中曲线的坐标系X轴范围为: [0~N] 其中N为(A扫图像的点数-1)
         *         图像中曲线的坐标系Y轴范围为: [0~200]
         */
        virtual std::function<std::optional<double>(double)> getAVGLineExpr(int idx) const;

        /**
         * @brief 获取AVG曲线的表达式
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         *
         * @return 返回函数表达式, 输入参数为DAC曲线的X, 输出参数为DAC曲线的Y
         *         图像中曲线的坐标系X轴范围为: [0~N] 其中N为(A扫图像的点数-1)
         *         图像中曲线的坐标系Y轴范围为: [0~200]
         */
        virtual std::function<std::optional<double>(double)> getDACLineExpr(int idx) const;

        /**
         * @brief 回放定时器每一帧的间隔(默认返回40ms)
         *
         * @note 改参数只有在连续图像`getDataSize`返回值大于1时有效
         *
         * @return (ms)
         */
        virtual int getReplayTimerInterval() const;

        /**
         * @brief 创建报表Map
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param soft_gain 软件增益
         * @return QVariantMap
         */
        virtual QVariantMap createReportValueMap(int idx, double soft_gain) const;

        /**
         * @brief 创建波门结果值
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param soft_gain 软件增益
         * @return QJsonArray
         */
        virtual QJsonArray createGateValue(int idx, double soft_gain) const;

        /**
         * @brief 创建工艺参数
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return 工艺参数
         */
        virtual QVariantMap createTechnologicalParameter(int idx) const;

        /**
         * @brief 是否使能报表导出按钮
         *
         * @return bool
         */
        virtual bool getReportEnable() const;

        ////////////////////////////////////////////////////// 不可重写的虚函数 //////////////////////////////////////////////////////

        /**
         * @brief 获取坐标范围
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return [bias, bias+len]
         *         其中bias为`getAxisBias`返回值, len为`getAxisLen`返回值
         */
        virtual std::array<double, 2> getAxisRange(int idx) const final;

        /**
         * @brief 获取波门的计算结果
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param gate_idx 波门序号 0-2
         * @param find_center_if_overflow true:在最高波超过屏幕宽度时寻找中心， false: 寻找第一个最高位置
         * @param enable_supression 使能抑制, 当波高百分比<=`getSupression`时, 返回 std::nullopt
         * @return std::optional<std::tuple<double, uint8_t>>
         *         [pos, max_amp]
         *         其中pos的范围为:0-1
         */
        virtual std::optional<std::tuple<double, uint8_t>> getGateResult(int idx, int gate_idx = 0, bool find_center_if_overflow = false, bool enable_supression = true) const final;

        /**
         * @brief 获取QLineSeries.replace可使用的A扫数据
         *
         * @param index 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @param softGain 软件增益
         * @return QList<QPointF>
         */
        virtual QList<QPointF> getAScanSeriesData(int index = 0, double softGain = 0.0) const final;

        /**
         * @brief 获取近场区的长度
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return 近场区长度(mm)
         */
        virtual double getNearField(int idx) const final;

        /**
         * @brief 判断是否是直探头
         *
         * @param idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return boolean
         */
        virtual bool isStraightBeamProbe(int idx) const final;
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
