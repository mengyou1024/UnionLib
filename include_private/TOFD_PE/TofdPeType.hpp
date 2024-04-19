#pragma once

#include "../common/FileReaderSelector.hpp"
#include <cstdint>
#include <fstream>
#include <vector>

namespace Union::TOFD_PE {
    class TofdPeIntf {
    public:
        virtual ~TofdPeIntf()                                        = default;
        virtual size_t __Read(std::ifstream& file, size_t file_size) = 0;

        virtual int                         getTofdLines(void) const = 0;
        virtual const std::vector<uint8_t>& getTofdData(void) const  = 0;
        virtual double                      getTofdDelay(void) const = 0;
        virtual double                      getTofdRange(void) const = 0;

        virtual bool                        hasPe(void) const      = 0;
        virtual int                         getPeLines(void) const = 0;
        virtual const std::vector<uint8_t>& getPeData(void) const  = 0;
        virtual double                      getPeDelay(void) const = 0;
        virtual double                      getPeRange(void) const = 0;
        virtual double                      getPeAngle(void) const = 0;

        virtual void removeThroughWaveEvent(double x, double y, double w, double h) = 0;
        virtual void pullThroughWaveEvent(double x, double y, double w, double h)   = 0;
        virtual void backup(void)                                                   = 0;
        virtual void rollback(void)                                                 = 0;
    };
#if __has_include("QtCore")
    inline static constexpr std::string_view TOFD_PE_I_NAME = "TOFD_PE";
    using TofdPeFileSelector                                = Union::Utils::FileReaderSelector<TofdPeIntf, TOFD_PE_I_NAME>;
#endif
} // namespace Union::TOFD_PE
