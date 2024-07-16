#pragma once

#include <QImage>

namespace Union::AScan::Special {

    // 钢轨焊缝示意图特化
    class RailWeldDigramSpecial {
    public:
        virtual ~RailWeldDigramSpecial()   = default;
        virtual double getDotX(void) const = 0;
        virtual double getDotY(void) const = 0;
        virtual double getDotZ(void) const = 0;

        /**
         * @brief 示意图的零点在轨底
         * @return bool
         */
        virtual bool zeroPointInFoot() const = 0;
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

        /**
         * @brief 显示摄像头图像
         * @param idx
         * @return
         */
        virtual bool showCameraImage(int idx) const {
            Q_UNUSED(idx)
            return true;
        }
    };

    class CMP000Special {
    public:
        virtual ~CMP000Special() = default;
        /**
         * @brief 特性启用
         * 
         * @return bool 
         */
        virtual bool isSpecialEnabled(int idx) const = 0;

        /**
         * @brief DAC曲线的根数
         * 
         * @param idx 图像序号
         * @return int 
         */
        virtual int getDacLineNumber(int idx) const = 0;

        /**
         * @brief DAC曲线的偏移
         * 
         * @param idx 图像序号
         * @param lineIdx 线条序号
         * @return double 
         */
        virtual double getDACLineBias(int idx, int lineIdx) const = 0;

        /**
         * @brief B波门是失波门类型
         * 
         * @param idx 图像序号
         * @return bool 
         */
        virtual bool gateBIsLostType(int idx) const = 0;
    };
} // namespace Union::AScan::Special
