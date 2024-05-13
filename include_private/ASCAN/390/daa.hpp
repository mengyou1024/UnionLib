#pragma once

#include "../330/das.hpp"
#include "../AScanType.hpp"
#include <cstdint>
#include <memory>

namespace Union::__390 {

    struct _390Extra {
        int                      num11;
        int                      num12;
        int                      num13;
        std::array<uint16_t, 20> num14;
        std::array<uint16_t, 20> pnNumber15;
        int                      stocktype;
        int                      railtype;
        int                      linetype;
        int                      finetype;
        int                      weldtype;
        int                      num22;
        int                      num23;
        std::array<uint16_t, 20> pnNumber24;
        uint32_t                 yearTemp;
        uint32_t                 monthTemp;
        uint32_t                 dateTemp;
        uint32_t                 hourTemp;
        uint32_t                 minuteTemp;
        int                      position;
        int                      num32;
        int                      size;
        int                      type;
        int                      num35;
        int                      num36;
        int                      num37;
        uint32_t                 posiFlage;
        uint32_t                 tailPosiFlage;
    };

    inline constexpr auto SIZE_390_EXTRA = sizeof(_390Extra);

    class DAAType : public Union::__330::DASType, public Union::AScan::Special::RailWeldDigramSpecial {
        _390Extra m_390Extra;

    public:
        static std::unique_ptr<Union::AScan::AScanIntf> FromFile(const std::wstring &fileName);

        size_t __Read(std::ifstream &file, size_t file_size) override;

        virtual std::wstring getProbe(int idx) const override final;

        virtual QVariantMap createTechnologicalParameter(int idx) const override final;

        virtual std::string getInstrumentName(void) const override final;
        virtual QVariantMap createReportValueMap(int idx, double soft_gain) const override final;
        virtual std::string getDate(int idx) const override final;

    private:
        QString getStationName() const;
        QString getStockType() const;
        QString getRailType() const;
        QString getLineType() const;
        QString getFineType() const;
        QString getWeldType() const;
        QString getNum14() const;
        QString getNum22() const;
        QString getNum23() const;
        QString getNum24() const;
        QString getNum25() const;
        QString getNum31() const;
        QString getNum32() const;
        QString getNum33() const;
        QString getNum34() const;
        QString getNum35() const;
        QString getNum36() const;
        QString getNum37() const;
        QString getPosiFlag() const;
        QString getTailPosiFlag() const;

        QString getChannelName() const;
    };
} // namespace Union::__390
