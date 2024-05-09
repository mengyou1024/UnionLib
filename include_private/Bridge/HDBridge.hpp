#pragma once

#include "../common/common.hpp"
#include <QObject>
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

namespace Union::Bridge::MultiChannelHardwareBridge {

    class HDBridgeIntf {
    public:
        ~HDBridgeIntf() = default;

        /**
         * @brief 打开设备
         * @return true if success
         */
        virtual bool open() = 0;

        /**
         * @brief 判断设备是否是打开状态
         * @return true if is open
         */
        virtual bool isOpen() = 0;

        /**
         * @brief 关闭设备
         * @return true if success
         */
        virtual bool close() = 0;

        /**
         * @brief 判断设备是否存在
         * @return true if exist
         */
        virtual bool isDeviceExist() = 0;

        /**
         * @brief 设置重复频率
         * @param ch 通道号
         * @param freq 重复频率(MHz)
         * @return true if success
         */
        virtual bool setFrequency(int ch, int freq) = 0;

        /**
         * @brief 获取重复频率
         * @param ch 通道号
         * @return 重复频率(MHz)
         */
        virtual int getFrequency(int ch) const = 0;

        /**
         * @brief 设置发射电压
         * @param ch 通道号
         * @param volt 发射电压(V)
         * @return true if success
         */
        virtual bool setVoltage(int ch, int volt) = 0;

        /**
         * @brief 获取发射电压
         * @param ch 通道号
         * @return 发射电压(V)
         */
        virtual int getVoltage(int ch) const = 0;

        /**
         * @brief 设置通道标志
         * @param ch 通道号
         * @param flag 通道标志
         * @return true if success
         */
        virtual bool setChannelFlag(int ch, uint32_t flag) = 0;

        /**
         * @brief 获取通道标志
         * @param ch 通道号
         * @return 通道标志
         */
        virtual uint32_t getChannelFlag(int ch) const = 0;

        /**
         * @brief 设置扫查增量
         * @param ch 通道号
         * @param inc 扫查增量(μs)
         * @return true if success
         */
        virtual bool setScanIncrement(int ch, int inc) = 0;

        /**
         * @brief 获取扫查增量
         * @param ch 通道号
         * @return 扫查增量(μs)
         */
        virtual int getScanIncrement(int ch) const = 0;

        /**
         * @brief 设置LED状态
         * @param ch 通道号
         * @param status LED状态
         * @return true if success
         */
        virtual bool setLedStatus(int ch, int status) = 0;

        /**
         * @brief 获取LED状态
         * @param ch 通道号
         * @return LED状态
         */
        virtual int getLedStatus(int ch) const = 0;

        /**
         * @brief 设置阻尼标志
         * @param ch 通道号
         * @param flag 阻尼标志
         * @return true if success
         */
        virtual bool setDamperFlag(int ch, int flag) = 0;

        /**
         * @brief 获取阻尼标志
         * @param ch 通道号
         * @return 阻尼标志
         */
        virtual int getDamperFlag(int ch) const = 0;

        /**
         * @brief 设置编码器脉冲
         * @param ch 通道号
         * @param pulse 编码器脉冲
         * @return true if success
         */
        virtual bool setEncoderPulse(int ch, int pulse) = 0;

        /**
         * @brief 获取编码器脉冲
         * @param ch 通道号
         * @return 编码器脉冲
         */
        virtual int getEncoderPulse(int ch) const = 0;

        /**
         * @brief 设置声速
         * @param ch 通道号
         * @param velocity 声速(m/s)
         * @return true if success
         */
        virtual bool setSoundVelocity(int ch, double velocity) = 0;

        /**
         * @brief 获取声速
         *
         * @param ch 通道号
         * @return 声速(m/s)
         */
        virtual double getSoundVelocity(int ch) const = 0;

        /**
         * @brief 设置零点偏移
         * @param ch 通道号
         * @param bias_us 零点偏移(μs)
         * @return true if success
         */
        virtual bool setZeroBias(int ch, double bias_us) = 0;

        /**
         * @brief 获取零点偏移
         *
         * @param ch 通道号
         * @return double 零点偏移(μs)
         */
        virtual double getZeroBias(int ch) const = 0;

        /**
         * @brief 设置脉冲宽度
         * @param ch 通道号
         * @param width 脉冲宽度(μs)
         * @return true if success
         */
        virtual bool setPulseWidth(int ch, int width_us) = 0;

        /**
         * @brief 获取脉冲宽度
         *
         * @param ch 通道号
         * @return 脉冲宽度(μs)
         */
        virtual double getPulseWidth(int ch) const = 0;

        /**
         * @brief 设置延时
         * @param ch 通道号
         * @param delay 延时(μs)
         * @return true if success
         */
        virtual bool setDelay(int ch, int delay_us) = 0;

        /**
         * @brief 获取延时
         *
         * @param ch 通道号
         * @return 延时(μs)
         */
        virtual double getDelay(int ch) const = 0;

        /**
         * @brief 设置采样深度
         * @param ch 通道号
         * @param depth 采样深度
         * @return true if success
         */
        virtual bool setSampleDepth(int ch, int depth) = 0;

        /**
         * @brief 获取采样深度
         *
         * @param ch 通道号
         * @return 采样深度
         */
        virtual double getSampleDepth(int ch) const = 0;

        /**
         * @brief 设置采样因子
         * @param ch 通道号
         * @param factor 采样因子
         * @return true if success
         */
        virtual bool setSampleFactor(int ch, int factor) = 0;

        /**
         * @brief 获取采样因子
         *
         * @param ch 通道号
         * @return 采样因子
         */
        virtual int getSampleFactor(int ch) const = 0;

        /**
         * @brief 设置增益
         * @param ch 通道号
         * @param gain 增益
         * @return true if success
         */
        virtual bool setGain(int ch, double gain) = 0;

        /**
         * @brief 获取增益
         *
         * @param ch 通道号
         * @return 增益
         */
        virtual double getGain(int ch) const = 0;

        /**
         * @brief 设置滤波器
         * @param ch 通道号
         * @param filter_index 滤波器索引
         * @return true if success
         */
        virtual bool setFilter(int ch, int filter_index) = 0;

        /**
         * @brief 获取滤波器
         *
         * @param ch 通道号
         * @return 滤波器索引
         */
        virtual int getFilter(int ch) const = 0;

        /**
         * @brief 设置检波方式
         * @param ch 通道号
         * @param demodu_index 检波方式索引
         * @return true if success
         */
        virtual bool setDemodu(int ch, int demodu_index) = 0;

        /**
         * @brief 获取检波方式
         *
         * @param ch 通道号
         * @return 检波方式索引
         */
        virtual int getDemodu(int ch) const = 0;

        /**
         * @brief 设置相位反转
         * @param ch 通道号
         * @param enable 是否开启
         * @return true if success
         */
        virtual bool setPhaseReverse(int ch, bool enable) = 0;

        /**
         * @brief 获取相位反转
         *
         * @param ch 通道号
         * @return 是否开启
         */
        virtual bool getPhaseReverse(int ch) const = 0;

        /**
         * @brief 设置波门信息
         * @param ch 通道号
         * @param idx 波门索引
         * @param info 波门信息
         * @return true if success
         */
        virtual bool setGateInfo(int ch, int idx, const Union::Base::Gate &gate) = 0;

        /**
         * @brief 获取波门信息
         *
         * @param ch 通道号
         * @param idx 波门索引
         * @return 波门信息
         */
        virtual const Union::Base::Gate &getGateInfo(int ch, int idx) const = 0;

        /**
         * @brief 同步参数至板卡
         * @return true if success
         */
        virtual bool sync2Board(void) const = 0;

        /**
         * @brief 通道参数复制参数
         * @param src 复制源
         * @param dist 复制目标列表
         * @param max_gate_number 最大波门数量
         */
        virtual void paramCopy(int src, std::vector<int> dist, int max_gate_number) {
            auto freq          = getFrequency(src);
            auto voltage       = getVoltage(src);
            auto channelFlag   = getChannelFlag(src);
            auto scanIncrement = getScanIncrement(src);
            auto ledStatus     = getLedStatus(src);
            auto damperFlag    = getDamperFlag(src);
            auto encoderPulse  = getEncoderPulse(src);
            auto soundVelocity = getSoundVelocity(src);
            auto zeroBias      = getZeroBias(src);
            auto pulseWidth    = getPulseWidth(src);
            auto delay         = getDelay(src);
            auto sampleDepth   = getSampleDepth(src);
            auto sampleFactor  = getSampleFactor(src);
            auto gain          = getGain(src);
            auto filter        = getFilter(src);
            auto demodu        = getDemodu(src);
            auto phaseReverse  = getPhaseReverse(src);

            std::vector<Union::Base::Gate> gate(static_cast<size_t>(max_gate_number));
            for (int i = 0; i < max_gate_number; i++) {
                gate.at(i) = getGateInfo(src, i);
            }

            for (auto i : dist) {
                setFrequency(i, freq);
                setVoltage(i, voltage);
                setChannelFlag(i, channelFlag);
                setScanIncrement(i, scanIncrement);
                setLedStatus(i, ledStatus);
                setDamperFlag(i, damperFlag);
                setEncoderPulse(i, encoderPulse);
                setSoundVelocity(i, soundVelocity);
                setZeroBias(i, zeroBias);
                setPulseWidth(i, pulseWidth);
                setDelay(i, delay);
                setSampleDepth(i, sampleDepth);
                setSampleFactor(i, sampleFactor);
                setGain(i, gain);
                setFilter(i, filter);
                setDemodu(i, demodu);
                setPhaseReverse(i, phaseReverse);
                for (int j = 0; j < max_gate_number; j++) {
                    setGateInfo(i, j, gate[i]);
                }
            }
            sync2Board();
        }
    };

    // template <int CH_N, int GATE_M, const std::string_view &BRIDGE_NAME>
    // class HDBrdgeTemplate : public QObject, public HDBridgeIntf {
    //     Q_OBJECT
    // private:
    //     using CH_F = std::array<double, CH_N>;
    //     using CH_I = std::array<int, CH_N>;
    //     using CH_U = std::array<uint32_t, CH_N>;
    //     using CH_B = std::array<bool, CH_N>;
    //     template <typename T>
    //     using CH_T = std::array<T, CH_N>;

    //     using _GateType = CH_T<std::array<Union::Base::Gate, GATE_M>>;

    //     int       m_frequency     = 0; ///< 重复频率
    //     int       m_voltage       = 0; ///< 发射电压
    //     uint32_t  m_channelFlag   = 0; ///< 通道标志
    //     int       m_scanIncrement = 0; ///< 扫查增量
    //     int       m_ledStatus     = 0; ///< LED状态
    //     int       m_damperFlag    = 0; ///< 阻尼标志
    //     int       m_encoderPulse  = 0; ///< 编码器脉冲
    //     CH_F      m_soundVelocity = 0; ///< 声速
    //     CH_F      m_zeroBias      = 0; ///< 零点偏移(μs)
    //     CH_F      m_pulseWidth    = 0; ///< 脉冲宽度
    //     CH_F      m_delay         = 0; ///< 延时(μs)
    //     CH_F      m_sampleDepth   = 0; ///< 采样深度
    //     CH_I      m_sampleFactor  = 0; ///< 采样因子
    //     CH_F      m_gain          = 0; ///< 增益
    //     CH_I      m_filter        = 0; ///< 滤波
    //     CH_I      m_demodu        = 0; ///< 检波方式
    //     CH_B      m_phaseReverse  = 0; ///< 相位翻转
    //     _GateType m_gate          = 0; /// 波门信息

    //     bool m_isValid = false; ///< 是否有效
    // public:
    //     virtual ~HDBrdgeIntf() = default;

    //     int getFrequency(int idx) const override final {
    //         (void)idx;
    //         return m_frequency;
    //     }
    // };
} // namespace Union::Bridge::MultiChannelHardwareBridge
