#pragma once

#include "../common/FileReaderSelector.hpp"
#include <cstdint>
#include <fstream>
#include <vector>

namespace Union::TOFD_PE {
    class TofdPeIntf : public Union::Utils::ReadIntf {
    public:
        virtual ~TofdPeIntf() = default;

        /**
         * @brief 文件读取接口
         *
         * @param file 文件输入流
         * @param file_size 文件大小
         * @return 已读取的大小
         */
        virtual size_t __Read(std::ifstream& file, size_t file_size) = 0;

        /**
         * @brief 获取A扫图像的大小
         *
         * @return int
         */
        virtual int getAScanSize(void) const = 0;

        /**
         * @brief 获取TOFD线条数量
         *
         * @return int
         */
        virtual int getTofdLines(void) const = 0;

        /**
         * @brief 获取TOFD数据
         *
         * @return const std::vector<uint8_t>&
         */
        virtual const std::vector<uint8_t>& getTofdData(void) const = 0;

        /**
         * @brief 获取TOFD延时(mm)
         * @return double
         */
        virtual double getTofdDelay(void) const = 0;

        /**
         * @brief 获取TOFD范围(mm)
         *
         * @return double
         */
        virtual double getTofdRange(void) const = 0;

        /**
         * @brief 判断是否有PE数据
         *
         * @return bool
         */
        virtual bool hasPe(void) const = 0;

        /**
         * @brief 获取PE线条数量
         *
         * @return int
         */
        virtual int getPeLines(void) const = 0;

        /**
         * @brief 获取PE数据
         *
         * @return const std::vector<uint8_t>&
         */
        virtual const std::vector<uint8_t>& getPeData(void) const = 0;

        /**
         * @brief 获取PE延时(mm)
         *
         * @return
         */
        virtual double getPeDelay(void) const = 0;

        /**
         * @brief 获取PE范围(mm)
         *
         * @return double
         */
        virtual double getPeRange(void) const = 0;

        /**
         * @brief 获取PE角度
         *
         * @return double
         */
        virtual double getPeAngle(void) const = 0;

        /**
         * @brief 去通波事件
         *
         * @param x,y,w,h 用户框选位置
         */
        virtual void removeThroughWaveEvent(double x, double y, double w, double h) = 0;

        /**
         * @brief 拉直通波事件
         *
         * @param x,y,w,h 用户框选位置
         */
        virtual void pullThroughWaveEvent(double x, double y, double w, double h) = 0;

        /**
         * @brief 备份参数数据
         *
         */
        virtual void backup(void) = 0;

        /**
         * @brief 恢复参数数据
         *
         */
        virtual void rollback(void) = 0;
    };
#if __has_include("QtCore")
    inline static constexpr std::string_view TOFD_PE_I_NAME = "TOFD_PE";
    using TofdPeFileSelector                                = Union::Utils::FileReaderSelector<TofdPeIntf, TOFD_PE_I_NAME>;
#endif
} // namespace Union::TOFD_PE
