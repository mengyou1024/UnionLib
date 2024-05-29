#pragma once

#include "./das.hpp"
#include "./dat.hpp"

namespace Union::__330 {
    struct FAT_330 {
        int                      pos  = {};
        uint32_t                 data = {};
        std::array<uint16_t, 12> name = {};
    };
    namespace SerialSpecial {

        struct FAT_330 {
            int                      pos  = {};
            uint32_t                 data = {};
            std::array<uint16_t, 12> name = {};
            uint32_t :32;
        };

        struct SerialGate {
            GATE_PARA_DAT gate;
            uint32_t :32;
        };

        struct SerialOneFrame {
            SYSTEM_STATUS_DAT systemStatus;
            uint32_t :32;
            CHANNEL_STATUS_DAT channelStatus;
            uint32_t :32;
            CHANNEL_PARAMETER_DAT channelParam;
            uint32_t :32;
            std::array<SerialGate, 2> gatePraram;
            DAC_DAT                   dac;
            uint32_t :32;
            WELD_PARA_DAT weldParam;
            uint32_t :32;
            GatePeakSingleParam gatePeakparam;
            uint32_t :32;
            std::vector<uint8_t> ascanData;
            std::vector<uint8_t> dacData;
            FAT_330              fat;
        };

    } // namespace SerialSpecial

    class Serial_330 : public Union::AScan::AScanIntf, public Union::__330::_330_DAC_C {
        using _My_T = std::vector<SerialSpecial::SerialOneFrame>;

    private:
        Base::Performance m_performance = {}; ///< 五大性能
        _My_T             m_data;
        int               m_fileName_index = 0;
        std::string       m_instrumentName = {}; ///< 仪器名称

    public:
        static std::optional<QString>                   ReadSerialAndSaveFile(const QString& portName, const QString& cachePath);
        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring& fileName);

        virtual size_t                    __Read(std::ifstream& file, size_t file_size) override final;
        virtual int                       getDataSize(void) const override final;
        virtual std::vector<std::wstring> getFileNameList(void) const override final;
        virtual void                      setFileNameIndex(int index) override final;
        virtual Base::Performance         getPerformance(int idx) const override final;
        virtual std::string               getDate(int idx) const override final;
        virtual std::wstring              getProbe(int idx) const override final;
        virtual double                    getProbeFrequence(int idx) const override final;
        virtual std::string               getProbeChipShape(int idx) const override final;
        virtual double                    getAngle(int idx) const override final;
        virtual double                    getSoundVelocity(int idx) const override final;
        virtual double                    getFrontDistance(int idx) const override final;
        virtual double                    getZeroPointBias(int idx) const override final;
        virtual double                    getSamplingDelay(int idx) const override final;
        virtual int                       getChannel(int idx) const override final;
        virtual std::string               getInstrumentName(void) const override final;
        // AScanData
        virtual std::array<Base::Gate, 2>     getGate(int idx) const override final;
        virtual const std::vector<uint8_t>&   getScanData(int idx) const override final;
        virtual double                        getAxisBias(int idx) const override final;
        virtual double                        getAxisLen(int idx) const override final;
        virtual double                        getBaseGain(int idx) const override final;
        virtual double                        getScanGain(int idx) const override final;
        virtual double                        getSurfaceCompentationGain(int idx = 0) const override final;
        virtual int                           getSupression(int idx) const override final;
        virtual Union::AScan::DistanceMode    getDistanceMode(int idx) const override final;
        virtual std::optional<Base::AVG>      getAVG(int idx) const override final;
        virtual std::optional<Base::DAC>      getDAC(int idx) const override final;
        virtual Union::AScan::DAC_Standard    getDACStandard(int idx) const override final;
        virtual std::function<double(double)> getAVGLineExpr(int idx) const override final;
        virtual std::function<double(double)> getDACLineExpr(int idx) const override final;

        virtual QJsonArray createGateValue(int idx, double soft_gain) const override;

        virtual const std::array<QVector<QPointF>, 3>& unResolvedGetDacLines(int idx) const override;
        virtual void                                   setUnResolvedGetDacLines(const std::array<QVector<QPointF>, 3>& dat, int idx) override;

        virtual std::pair<double, double> getProbeSize(int idx) const override final;

        virtual int getReplayTimerInterval() const override final;

    private:
        int                                                    getOption(void) const noexcept;
        double                                                 getUnit(void) const noexcept;
        mutable std::map<int, std::array<QVector<QPointF>, 3>> m_dac_map;
    };

} // namespace Union::__330
