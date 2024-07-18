#pragma once

#include "../330/dat.hpp"
#include "../AScanType.hpp"
#include <cstdint>
#include <memory>

namespace Union::__390 {
    class HFDATType : public Union::__330::DATType {
    public:
        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring &fileName);

        virtual QJsonArray   createGateValue(int idx, double soft_gain) const override;
        virtual std::string  getInstrumentName(void) const override;
        virtual QVariantMap  createTechnologicalParameter(int idx) const override final;
        virtual bool         getReportEnable() const override final;
        virtual bool         getDateEnable() const override;
        virtual std::wstring getProbe(int idx) const override;
        virtual std::string  getProbeChipShape(int idx) const override;

    private:
        QString getChanneName(int idx) const;
    };
} // namespace Union::__390
