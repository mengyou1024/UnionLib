#pragma once

#include <type_traits>

namespace Union::Base {
    // 仪器性能
    struct Performance {
        double horizontalLinearity = {}; ///< 水平线性
        double verticalLinearity   = {}; ///< 垂直线性
        double resolution          = {}; ///< 分辨力
        double dynamicRange        = {}; ///< 动态范围
        double sensitivity         = {}; ///< 灵敏度余量
    };
} // namespace Union::Base
