#pragma once

#include "./Gate.hpp"
#include "./Performance.hpp"
#include "./Probe.hpp"
#include <cmath>
#include <functional>
#include <type_traits>

namespace Union {
    /**
     * @brief 计算增益
     * @param input 输出
     * @param output 输入
     *
     * @return 增益
     */
    double CalculatedGain(double input, double output);

    /**
     * @brief 计算增益输出
     * @param input 输入
     * @param gain 增益
     *
     * @return 输出
     */
    double CalculateGainOutput(double input, double gain);

    /**
     * @brief 计算增益输入
     * @param output 增益后的输出
     * @param gain 增益
     *
     * @return 输入
     */
    double CalculateGainInput(double output, double gain);

    /**
     * @brief 小数保留精度
     * @param value x
     *
     * @return y
     */
    template <int P = 2, class T = double, class = std::enable_if_t<std::is_floating_point_v<T>>>
    T KeepDecimals(T value) {
        const auto _P = static_cast<double>(std::pow(10.0, P));
        return std::round(static_cast<double>(value) * _P) / _P;
    }

    /**
     * @brief 值映射
     * @param val 值
     * @param dist 目标坐标系
     * @param raw 原始坐标系
     *
     * @return 映射值
     */
    double ValueMap(double val, const std::array<double, 2>& dist, const std::array<double, 2>& raw = {0.0, 1.0});

    /**
     * @brief 不同距离的大平底面回波dB差
     * @param x1 底面深度1
     * @param x2 底面深度2
     *
     * @return DB(a1) - DB(a2)
     */
    double EchoDbDiffOfPlan(double x1, double x2);

    /**
     * @brief 不同距离不同大小的平底孔回波dB差
     * @param x1 孔1深度
     * @param d1 孔1直径
     * @param x2 孔2深度
     * @param d2 孔2直径
     *
     * @return DB(a1) - DB(a2)
     */
    double EchoDbDiffOfHole(double x1, double d1, double x2, double d2);

    /**
     * @brief 同距离大平底面和平底孔的db差
     * @param lambda 声束波长
     * @param x 距离
     * @param d 孔直径
     *
     * @return DB(P) - DB(H)
     */
    double EchoDbDiffOfPlanAndHole(double lambda, double x, double d);

    template <class T>
    T ByteSwap(T x) {
        if constexpr (sizeof(T) == 1) {
            return x;
        } else if constexpr (sizeof(T) == 2) {
            return (x >> 8) | (x << 8);
        } else if constexpr (sizeof(T) == 4) {
            return (x >> 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x << 24);
        }
    }

    /**
     * @brief 时间转换为距离
     * @param time_us 时间(μs)
     * @param velocity_m_per_s 声速(m/s)
     * @param coef 系数，如果是经过反射则为2.0(默认)，如果要计算实际距离则为1.0
     * @return 距离(mm)
     */
    constexpr double Time2Distance(double time_us, double velocity_m_per_s, double coef = 2.0) {
        return velocity_m_per_s * time_us / 1000.0 / coef;
    }

    /**
     * @brief 距离转换为时间
     * @param distance_mm 距离(mm)
     * @param velocity_m_per_s 声速(m/s)
     * @param coef 系数，如果是经过反射则为2.0(默认)，如果要计算实际距离则为1.0
     * @return 时间(μs)
     */
    constexpr double Distance2Time(double distance_mm, double velocity_m_per_s, double coef = 2.0) {
        return distance_mm * 1000.0 * coef / velocity_m_per_s;
    }

    /**
     * @brief 汉字区位码转UTF-8字符串
     * @param code 汉字区位码
     * @return UTF-8字符串
     */
    QString LocationCodeToUTF8(const std::vector<uint16_t>& code);

    /**
     * @brief 毫米转微秒
     * @param mm
     * @param speed_m_per_s 声速m/s
     * @return us
     */
    constexpr double mm2us(double mm, double speed_m_per_s = 5900) {
        return mm * 2000 / speed_m_per_s;
    }

    /**
     * @brief 微秒转毫米
     * @param us
     * @param speed_m_per_s 声速m/s
     * @return mm
     */
    constexpr double us2mm(double us, double speed_m_per_s = 5900) {
        return us * speed_m_per_s / 2000;
    }

    /**
     * @brief 计算近场区的长度
     * @param l_or_d 探头长度或直径(mm)
     * @param w_or_zero 探头宽度或0(mm)
     * @param probe_freq 探头频率(MHz)
     * @param speed 声速(m/s)
     * @return
     */
    double CalculateNearField(double l_or_d, double w_or_zero, double probe_freq, double speed);
} // namespace Union
