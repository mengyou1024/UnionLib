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
    template <class T, int P = 2, class = std::enable_if_t<std::is_floating_point_v<T>>>
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

} // namespace Union
