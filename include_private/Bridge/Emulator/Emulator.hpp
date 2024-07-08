#pragma once

#include "../HDBridge.hpp"

namespace Union::Bridge::MultiChannelHardwareBridge {
    class Emulator : public HDBridgeIntf {
    public:
        explicit Emulator(int channel_number = 12, int gate_number = 2);
        virtual ~Emulator();

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
        int m_channel_number = 12;
        int m_gate_number    = 2;

        virtual std::shared_ptr<ScanData> readOneFrame(void) override;
    };
} // namespace Union::Bridge::MultiChannelHardwareBridge
