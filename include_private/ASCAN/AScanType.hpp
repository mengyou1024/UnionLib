#pragma once

#include "../common/AVG.hpp"
#include "../common/DAC.hpp"
#include "../common/FileReaderSelector.hpp"
#include "../common/Gate.hpp"
#include "../common/Performance.hpp"
#include "../common/common.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#if __has_include("QtCore")
    #include <QFileInfo>
    #include <QJsonArray>
    #include <QJsonObject>
    #include <QLoggingCategory>
    #include <QRegularExpression>
    #include <QRegularExpressionMatch>
    #include <QRegularExpressionMatchIterator>
    #include <QString>
    #include <QtCore>
#endif

#if __has_include("NumCpp/Polynomial/Poly1d.hpp")
    #include <NumCpp/Polynomial/Poly1d.hpp>
#endif

namespace Union::AScan {
    enum DistanceMode {
        DistanceMode_Y,
        DistanceMode_X,
        DistanceMode_S,
    };

    struct DAC_Standard {
        double rlBias = -4.0;
        double slBias = -12.0;
        double elBias = -18.0;
    };

    class AScanIntf {
    public:
        virtual ~AScanIntf() = default;

        virtual size_t                    __Read(std::ifstream &file, size_t file_size) = 0;
        virtual int                       getDataSize(void) const                       = 0;
        virtual std::vector<std::wstring> getFileNameList(void) const                   = 0;
        virtual void                      setFileNameIndex(int index)                   = 0;

        // AScanType
        virtual Base::Performance getPerformance(int idx) const    = 0;
        virtual std::string       getDate(int idx) const           = 0;
        virtual std::wstring      getProbe(int idx) const          = 0;
        virtual double            getProbeFrequence(int idx) const = 0;
        virtual std::string       getProbeChipShape(int idx) const = 0;
        virtual double            getAngle(int idx) const          = 0;
        virtual double            getSoundVelocity(int idx) const  = 0;
        virtual double            getFrontDistance(int idx) const  = 0;
        virtual double            getZeroPointBias(int idx) const  = 0;
        virtual double            getSamplingDelay(int idx) const  = 0;
        virtual int               getChannel(int idx) const        = 0;
        virtual std::string       getInstrumentName(void) const    = 0;
        // AScanData
        virtual std::array<Base::Gate, 2>   getGate(int idx) const                        = 0;
        virtual const std::vector<uint8_t> &getScanData(int idx) const                    = 0;
        virtual double                      getAxisBias(int idx) const                    = 0;
        virtual double                      getAxisLen(int idx) const                     = 0;
        virtual double                      getBaseGain(int idx) const                    = 0;
        virtual double                      getScanGain(int idx) const                    = 0;
        virtual double                      getSurfaceCompentationGain(int idx = 0) const = 0;
        virtual int                         getSupression(int idx) const                  = 0;
        virtual DistanceMode                getDistanceMode(int idx) const                = 0;
        virtual std::optional<Base::AVG>    getAVG(int idx) const                         = 0;
        virtual std::optional<Base::DAC>    getDAC(int idx) const                         = 0;
        virtual DAC_Standard                getDACStandard(int idx) const                 = 0;

        virtual std::function<double(double)> getAVGLineExpr(int idx) const = 0;
        virtual std::function<double(double)> getDACLineExpr(int idx) const = 0;

        virtual std::function<double(double)>              getLineExpr(int idx, const std::vector<int> &_index, const std::vector<uint8_t> &value,
                                                                       const std::array<double, 2>          &r_range      = {0.0, 105.0} /*TODO: 声程计算*/,
                                                                       const std::array<double, 2>          &v_range      = {0, 520.0},
                                                                       std::function<double(double, double)> func_db_diff = Union::EchoDbDiffOfPlan) const final;
        virtual std::optional<std::tuple<double, uint8_t>> getGateResult(int idx, int gate_idx = 0, bool find_center_if_overflow = true) const final;
        virtual std::array<double, 2>                      getAxisRange(int idx) const final;
#if __has_include("QJsonObject")
        QJsonObject    getBoardParamJSON(int idx) const;
        QList<QPointF> getAScanSeriesData(int index = 0, double softGain = 0.0) const;
#endif
    };

#if __has_include("QtCore")
    inline static constexpr std::string_view ASCAN_I_NAME = "AScan";
    using AScanFileSelector                               = Union::Utils::FileReaderSelector<AScanIntf, ASCAN_I_NAME>;
#endif

} // namespace Union::AScan
