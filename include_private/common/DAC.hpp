#pragma once

#include "./common.hpp"
#include <functional>
#include <vector>

namespace Union::Base {
    class DAC {
    private:
        double mBias  = {};
        double mLen   = {};
        int    mCount = {};

    public:
        DAC(double bias = 0.0, double len = 250.0, int count = 520) :
        mBias(bias),
        mLen(len),
        mCount(count) {}
        double               baseGain         = {}; ///< 基础增益
        double               biasGain         = {}; ///< 补偿增益
        int                  gate             = {};
        bool                 isSubline        = {};
        std::vector<int>     index            = {}; ///< 索引
        std::vector<uint8_t> value            = {}; ///< 值
        int                  samplingDepth    = {}; ///< 采样深度
        double               decimationFactor = {}; ///< 采样因子
    };
} // namespace Union::Base
