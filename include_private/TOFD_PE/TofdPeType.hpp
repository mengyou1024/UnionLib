#pragma once

#include "../common/FileReaderSelector.hpp"
#include <QJsonObject>
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
         * @return double (mm)
         */
        virtual double getPeDelay(void) const = 0;

        /**
         * @brief 获取PE范围(mm)
         *
         * @return double (mm)
         */
        virtual double getPeRange(void) const = 0;

        /**
         * @brief 获取PE角度
         *
         * @return double (°)
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

        /**
         * @brief 获取Tofd参数
         * @return const QJsonObject
         */
        virtual const QJsonObject getTofdParam(void) const = 0;

        /**
         * @brief 获取Pe参数
         * @return const QJsonObject
         */
        virtual const QJsonObject getPeParam(void) const = 0;

        /**
         * @brief 获取TOFD系统参数
         * @return QJsonObject
         */
        virtual const QJsonObject getTofdSystemParam(void) const = 0;

        /**
         * @brief 获取TOFD工件参数
         * @return QJsonObject
         */
        virtual const QJsonObject getTofdWorkPieceParam(void) const = 0;

        /**
         * @brief 获取TOFD通道参数
         * @return QJsonObject
         */
        virtual const QJsonObject getTofdChannelParam(void) const = 0;

        /**
         * @brief 获取Pe工件参数
         * @return QJsonObject
         */
        virtual const QJsonObject getPeWorkPieceParam(void) const = 0;

        /**
         * @brief 获取发射电压
         * @return QString
         */
        virtual QString getEmitVoltage(void) const = 0;

        /**
         * @brief 获取重复频率
         * @return int (Hz)
         */
        virtual int getRepeatFreq(void) const = 0;

        /**
         * @brief 获取扫查方式
         * @return QString
         */
        virtual QString getScanMode(void) const = 0;

        /**
         * @brief 获取触发方式
         * @return QString
         */
        virtual QString getTriggerMode(void) const = 0;

        /**
         * @brief 获取扫查增量
         * @return double (mm)
         */
        virtual double getScanIncrement(void) const = 0;

        /**
         * @brief 获取工件厚度
         * @return double (mm)
         */
        virtual double getThicknessOfWorkPiece(void) const = 0;

        /**
         * @brief 获取焊缝宽度
         * @return double (mm)
         */
        virtual double getTofdWeldWidth(void) const = 0;

        /**
         * @brief 获取坡口类型
         * @return QString
         */
        virtual QString getGrooveType(void) const = 0;

        /**
         * @brief 获取TOFD增益
         * @return double
         */
        virtual double getTofdGain(void) const = 0;

        /**
         * @brief 获取滤波频带
         * @return QString
         */
        virtual QString getFilterBand(void) const = 0;

        /**
         * @brief 获取检波方式
         * @return QString
         */
        virtual QString getDetectionMode(void) const = 0;

        /**
         * @brief 获取探头类型
         * @return QString
         */
        virtual QString getTofdProbeType(void) const = 0;

        /**
         * @brief 获取探头尺寸
         * @return QString
         */
        virtual QString getTofdProbeSize(void) const = 0;

        /**
         * @brief 获取探头频率
         * @return double (MHz)
         */
        virtual double getTofdProbeFreq(void) const = 0;

        /**
         * @brief 获取TOFD角度
         * @return double (°)
         */
        virtual double getTofdAngle(void) const = 0;

        /**
         * @brief 获取TOFD前沿
         * @return double (mm)
         */
        virtual double getTofdProbeFront(void) const = 0;

        /**
         * @brief 获取TOFD零点
         * @return double (μs)
         */
        virtual double getTofdZeroPoint(void) const = 0;

        /**
         * @brief 获取PE增益
         * @return double (dB)
         */
        virtual double getPeGain(void) const = 0;

        /**
         * @brief 获取PE探头前沿
         * @return double (mm)
         */
        virtual double getPeProbeFront(void) const = 0;

        /**
         * @brief 获取PE零点
         * @return double(us)
         */
        virtual double getPeZeroPoint(void) const = 0;

        /**
         * @brief 获取PE频率
         * @return double (MHz)
         */
        virtual double getPeProbeFreq(void) const = 0;

        /**
         * @brief 获取Pe晶片
         * @return QString
         */
        virtual QString getPeCrystalPlate(void) const = 0;

        /**
         * @brief 获取PCS
         * @return double (mm)
         */
        virtual double getPCS(void) const = 0;

        /**
         * @brief 毫米转微秒
         * @param mm
         * @param speed_m_per_s 声速m/s
         * @return us
         */
        static double mm2us(double mm, double speed_m_per_s = 5900) {
            return mm * 2000 / speed_m_per_s;
        }

        /**
         * @brief 微秒转毫米
         * @param us
         * @param speed_m_per_s 声速m/s
         * @return mm
         */
        static double us2mm(double us, double speed_m_per_s = 5900) {
            return us * speed_m_per_s / 2000;
        }
    };

    inline static constexpr std::string_view TOFD_PE_I_NAME = "TOFD_PE";
    using TofdPeFileSelector                                = Union::Utils::FileReaderSelector<TofdPeIntf, TOFD_PE_I_NAME>;
} // namespace Union::TOFD_PE
