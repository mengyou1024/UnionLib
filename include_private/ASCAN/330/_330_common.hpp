#pragma once

#include <QPointF>
#include <QVector>
#include <array>

namespace Union::__330 {
    class _330_DAC_C {
    public:
        virtual ~_330_DAC_C() = default;

        virtual const std::array<QVector<QPointF>, 3>& unResolvedGetDacLines(int idx) const = 0;

        virtual void setUnResolvedGetDacLines(const std::array<QVector<QPointF>, 3>& dat, int idx) = 0;
    };
} // namespace Union::__330