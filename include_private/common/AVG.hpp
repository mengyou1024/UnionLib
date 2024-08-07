#pragma once

#include "./DAC.hpp"
#include <functional>
#include <vector>

namespace Union::Base {
    class AVG : public DAC {
    public:
        using _T_PAIR_D              = std::pair<double, double>;
        double    equivalent         = {}; ///< 当量
        double    diameter           = {}; ///< 直径
        double    reflectorDianmeter = {}; ///< 反射体直径
        double    reflectorMaxDepth  = {}; ///< 反射体最大深度
        int       samplingDepth      = {}; ///< 采样深度
        double    decimationFactor   = {}; ///< 采样因子
        double    scanGain           = {}; ///< 扫查增益
        _T_PAIR_D samplingAxis       = {}; ///< 采样时的坐标轴
    };
} // namespace Union::Base
