#pragma once

#include "./common.hpp"
#include <functional>
#include <vector>

namespace Union::Base {
    class DAC {
    public:
        double              baseGain         = {}; ///< 基础增益
        double              biasGain         = {}; ///< 补偿增益
        int                 gate             = {};
        bool                isSubline        = {};
        std::vector<double> index            = {}; ///< 索引
        std::vector<double> value            = {}; ///< 值
        int                 samplingDepth    = {}; ///< 采样深度
        double              decimationFactor = {}; ///< 采样因子
    };
} // namespace Union::Base
