#pragma once

#include "../HDBridge.hpp"

namespace Union::Bridge::MultiChannelHardwareBridge {
    class _12CH_USB : public HDBridgeIntf {
    public:
        explicit _12CH_USB(int gate_number = 2, int type = 0);
        virtual ~_12CH_USB();

        virtual bool open() override;

        virtual bool isOpen() override;

        virtual bool close() override;

        virtual bool isDeviceExist() override;

        virtual int getChannelNumber() const override;

        virtual int getGateNumber() const override;

        virtual void loadDefaultConfig() override;

        virtual bool setFrequency(int freq) override;

        virtual bool setVoltage(int volt) override;

        virtual const QVector<QString> &getVoltageTable() const override;

        virtual bool setChannelFlag(uint32_t flag) override;

        virtual bool setDamperFlag(int flag) override;

        virtual bool setSoundVelocity(int ch, double velocity) override;

        virtual bool setZeroBias(int ch, double bias_us) override;

        virtual bool setPulseWidth(int ch, double width_us) override;

        virtual bool setDelay(int ch, double delay_us) override;

        virtual bool setSampleDepth(int ch, double depth) override;

        virtual bool setSampleFactor(int ch, int factor) override;

        virtual bool setGain(int ch, double gain) override;

        virtual bool setFilter(int ch, int filter_index) override;

        virtual const QVector<QString> &getFilterTable() const override;

        virtual bool setDemodu(int ch, int demodu_index) override;

        virtual const QVector<QString> &getDemoduTable() const override;

        virtual bool setPhaseReverse(int ch, bool enable) override;

        virtual bool sync2Board(void) const override;

    private:
        int m_type        = 0;
        int m_gate_number = 2;

        std::shared_ptr<ScanData> readOneFrame(void) ;
    };
} // namespace Union::Bridge::MultiChannelHardwareBridge
