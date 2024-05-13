#pragma once

#include <QImage>

namespace Union::AScan::Special {

    // 钢轨焊缝示意图特化
    class RailWeldDigramSpecial {
    public:
        virtual ~RailWeldDigramSpecial() = default;
    };

    // 摄像头图像特化
    class CameraImageSpecial {
    public:
        virtual ~CameraImageSpecial() = default;
        /**
         * @brief 获取摄像头图像
         *
         * @param idx idx 图像序号, 仅在连续图像`getDataSize`返回值大于1时有效
         * @return QImage
         */
        virtual QImage getCameraImage(int idx) const = 0;
    };
} // namespace Union::AScan::Special