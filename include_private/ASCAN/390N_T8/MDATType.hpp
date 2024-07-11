
#pragma once

#include "../AScanType.hpp"
#include "./_MDATType.hpp"

namespace Union::__390N_T8::MDATType {
    using namespace Union::AScan;

    class UnType : public Union::AScan::AScanIntf,public Union::AScan::Special::CameraImageSpecial {
    public:
        using _Body_T = std::tuple<AScanData, ChannelParam, std::shared_ptr<DACParam>, std::shared_ptr<AVGParam>, std::shared_ptr<Performance>, std::shared_ptr<CameraData>>;
        using _Data_T = std::pair<InstrumentBaseInfo, std::vector<_Body_T>>;

        _Data_T                   m_data         = {};
        std::vector<std::wstring> m_fileNameList = {};

        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring &fileName);

        virtual size_t                      __Read(std::ifstream &file, size_t file_size) override;
        virtual int                         getDataSize(void) const override;
        virtual std::vector<std::wstring>   getFileNameList(void) const override;
        virtual void                        setFileNameIndex(int index) override;
        virtual Base::Performance           getPerformance(int idx) const override;
        virtual std::string                 getDate(int idx) const override;
        virtual std::wstring                getProbe(int idx) const override;
        virtual double                      getProbeFrequence(int idx) const override;
        virtual std::string                 getProbeChipShape(int idx) const override;
        virtual double                      getAngle(int idx) const override;
        virtual double                      getSoundVelocity(int idx) const override;
        virtual double                      getFrontDistance(int idx) const override;
        virtual double                      getZeroPointBias(int idx) const override;
        virtual double                      getSamplingDelay(int idx) const override;
        virtual int                         getChannel(int idx) const override;
        virtual std::string                 getInstrumentName(void) const override;
        virtual std::array<Base::Gate, 2>   getGate(int idx) const override;
        virtual const std::vector<uint8_t> &getScanData(int idx) const override;
        virtual double                      getAxisBias(int idx) const override;
        virtual double                      getAxisLen(int idx) const override;
        virtual double                      getBaseGain(int idx) const override;
        virtual double                      getScanGain(int idx) const override;
        virtual double                      getSurfaceCompentationGain(int idx) const override;
        virtual double                      getSupression(int idx) const override;
        virtual DistanceMode                getDistanceMode(int idx) const override;
        virtual std::optional<Base::AVG>    getAVG(int idx) const override;
        virtual std::optional<Base::DAC>    getDAC(int idx) const override;
        virtual DAC_Standard                getDACStandard(int idx) const override;
        virtual std::pair<double, double>   getProbeSize(int idx) const override;

        virtual int getReplayTimerInterval() const override;

        virtual QJsonArray createGateValue(int idx, double soft_gain) const override final;

        virtual bool   showCameraImage(int idx) const override;
        virtual QImage getCameraImage(int idx) const override;

    private:
        QString getDacEquivalent(int idx, int gate_idx) const;
        QString getAvgEquivalent(int idx, int gate_idx) const;
    };
} // namespace Union::__390N_T8::MDATType
