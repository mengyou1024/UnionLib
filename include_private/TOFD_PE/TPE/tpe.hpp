#include "../TofdPeType.hpp"

#include <memory>
#include <optional>

namespace Union::TOFD_PE::TPE {

    struct SubData {
        uint32_t             baseGain;  ///< 增益
        uint32_t             range;     ///< 声程范围
        uint32_t             frequency; ///< 探头频率
        int32_t              angle;     ///< 一位小数点
        uint16_t             crystal_w;
        uint16_t             crystal_l;
        uint32_t             forward; ///< 探头前沿
        uint32_t             offset;  ///< 零点
        int32_t              line;
        int32_t              derect;
        std::vector<uint8_t> data;
    };

    struct Data {
        uint32_t               pulseMode;  ///< 脉冲模式
        uint32_t               repeatRate; ///< 重复频率
        int32_t                scanMode;   ///< 扫查模式 0: 平行扫查 1: 非平行扫查
        int32_t                coderType;  ///< 编码器类型 0: 编码器 1: 时间
        float                  tofdFreq;   ///< TOFD频率
        float                  encValue;   ///<
        int32_t                thickness;  ///< 厚度
        int32_t                pcs;        ///<
        uint32_t               baseGain;   ///< 基本增益
        uint32_t               range;      ///< 声程
        uint32_t               delay;      ///< 延时
        uint32_t               frequency;  ///< 探头频率 一位小数点
        int32_t                angle;      ///< 角度
        uint16_t               crystal_w;
        uint16_t               crystal_l;
        uint32_t               forward;   ///< 前沿
        uint32_t               offset;    ///< 零点
        int32_t                line;      ///< 曲线条数
        int32_t                groove;    ///< 坡口形式
        int32_t                average;   ///< 平均次数
        int32_t                weldWidth; ///< 焊缝宽度
        int32_t                bandMode;  ///< 滤波频带
        uint32_t               flag;
        std::vector<uint8_t>   data;
        std::optional<SubData> subData;
    };

    class TpeType : public TofdPeIntf {
        Data m_data = {};

        std::optional<Data> m_data_bak = std::nullopt;

    public:
        static std::unique_ptr<TofdPeIntf> FromFile(const std::wstring& fileName);

        virtual size_t __Read(std::ifstream& file, size_t file_size) override final;

        virtual int getAScanSize(void) const override final;

        virtual int                         getTofdLines(void) const override final;
        virtual const std::vector<uint8_t>& getTofdData(void) const override final;
        virtual double                      getTofdDelay(void) const override final;
        virtual double                      getTofdRange(void) const override final;

        virtual bool                        hasPe(void) const override final;
        virtual int                         getPeLines(void) const override final;
        virtual const std::vector<uint8_t>& getPeData(void) const override final;
        virtual double                      getPeRange(void) const override final;
        virtual double                      getPeDelay(void) const override final;
        virtual double                      getPeAngle(void) const override final;

        virtual void removeThroughWaveEvent(double x, double y, double w, double h) override final;
        virtual void pullThroughWaveEvent(double x, double y, double w, double h) override final;
        virtual void backup(void) override final;
        virtual void rollback(void) override final;

        virtual const QJsonObject getTofdParam(void) const override;
        virtual const QJsonObject getPeParam(void) const override;
        virtual const QJsonObject getTofdSystemParam(void) const override;
        virtual const QJsonObject getTofdWorkPieceParam(void) const override;
        virtual const QJsonObject getTofdChannelParam(void) const override;
        virtual const QJsonObject getPeWorkPieceParam(void) const override;
        virtual QString           getEmitVoltage(void) const override;
        virtual int               getRepeatFreq(void) const override;
        virtual QString           getScanMode(void) const override;
        virtual QString           getTriggerMode(void) const override;
        virtual double            getScanIncrement(void) const override;
        virtual double            getThicknessOfWorkPiece(void) const override;
        virtual double            getTofdWeldWidth(void) const override;
        virtual QString           getGrooveType(void) const override;
        virtual double            getTofdGain(void) const override;
        virtual QString           getFilterBand(void) const override;
        virtual QString           getDetectionMode(void) const override;
        virtual QString           getTofdProbeType(void) const override;
        virtual QString           getTofdProbeSize(void) const override;
        virtual double            getTofdProbeFreq(void) const override;
        virtual double            getTofdAngle(void) const override;
        virtual double            getTofdProbeFront(void) const override;
        virtual double            getTofdZeroPoint(void) const override;
        virtual double            getPeGain(void) const override;
        virtual double            getPeProbeFront(void) const override;
        virtual double            getPeZeroPoint(void) const override;
        virtual double            getPeProbeFreq(void) const override;
        virtual QString           getPeCrystalPlate(void) const override;
        virtual double            getPCS(void) const override;
    };
} // namespace Union::TOFD_PE::TPE
