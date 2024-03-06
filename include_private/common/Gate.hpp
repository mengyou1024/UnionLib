#pragma once

#include <array>
#include <type_traits>

namespace Union::Base {
    // 波门
    struct Gate {
        double pos    = {}; ///< 位置
        double width  = {}; ///< 宽度
        double height = {}; ///< 高度
        bool   enable = {}; ///< 是否启用
    };

} // namespace Union::Base
