#pragma once

#include <vector>
#include <functional>
#include "./DAC.hpp"

namespace Union::Base {
    class AVG:public DAC {
    public:
        double               equivalent         = {}; ///< 当量
        double               diameter           = {}; ///< 直径
        double               reflectorDianmeter = {}; ///< 反射体直径
        double               reflectorMaxDepth  = {}; ///< 反射体最大深度
        int                  samplingDepth      = {}; ///< 采样深度
        double               decimationFactor   = {}; ///< 采样因子
        double               scanGain           = {}; ///< 扫查增益
    };
} // namespace Union::Base
