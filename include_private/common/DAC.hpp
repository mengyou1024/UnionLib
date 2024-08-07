#pragma once

#include "./union_common.hpp"
#include <functional>
#include <vector>

namespace Union::Base {
    class DAC {
    public:
        using _T_PAIR_D = std::pair<double, double>;

        double              baseGain         = {}; ///< 基础增益
        double              biasGain         = {}; ///< 补偿增益
        int                 gate             = {};
        bool                isSubline        = {};
        std::vector<double> index            = {}; ///< 索引
        std::vector<double> value            = {}; ///< 值
        int                 samplingDepth    = {}; ///< 采样深度
        double              decimationFactor = {}; ///< 采样因子
        _T_PAIR_D           samplingAxis     = {}; ///< 采样时的坐标轴
    };

    struct DAC_Standard {
        double rlBias = -4.0;
        double slBias = -12.0;
        double elBias = -18.0;
    };
} // namespace Union::Base

namespace Union::AScan {
    using DAC_Standard = Union::Base::DAC_Standard;
}
