#include "tpe.hpp"
#include <Yo/File>
#include <stdexcept>

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::TOFD_PE::TPE::TpeType &data, [[maybe_unused]] size_t file_size) {
        return data.__Read(file, file_size);
    }

} // namespace Yo::File

namespace Union::TOFD_PE::TPE {
    std::unique_ptr<TofdPeIntf> TpeType::FromFile(const std::wstring &fileName) {
        auto ret = std::make_unique<TpeType>();
        if (!Yo::File::ReadFile(fileName, (*(ret.get())))) {
            return nullptr;
        }
        return ret;
    }

    size_t TpeType::__Read(std::ifstream &file, size_t file_size) {
        size_t ret = 0;
        ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(m_data.pulseMode), &(m_data.bandMode), sizeof(int32_t)), file_size);
        m_data.data.resize((m_data.line + 1) * 500);
        ret += Yo::File::__Read(file, m_data.data, file_size);
        ret += Yo::File::__Read(file, m_data.flag, file_size);

        if (m_data.flag == 2) {
            m_data.subData = SubData();
            ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(m_data.subData->baseGain), &(m_data.subData->derect), sizeof(int32_t)), file_size);
            m_data.subData->data.resize((m_data.subData->line + 1) * 500);
            ret += Yo::File::__Read(file, m_data.subData->data, file_size);
        }
        return ret;
    }

    int TpeType::getTofdLines(void) const {
        return m_data.line + 1;
    }

    const std::vector<uint8_t> &TpeType::getTofdData(void) const {
        return m_data.data;
    }

    double TpeType::getTofdDelay(void) const {
        return m_data.delay / 100.0;
    }

    double TpeType::getTofdRange(void) const {
        return m_data.range / 10.0;
    }

    bool TpeType::hasPe(void) const {
        return m_data.subData.has_value();
    }

    int TpeType::getPeLines(void) const {
        if (!hasPe()) {
            return -1;
        }
        return m_data.subData->line + 1;
    }

    const std::vector<uint8_t> &TpeType::getPeData(void) const {
        if (!hasPe()) {
            throw std::runtime_error("No PE data");
        }
        return m_data.subData->data;
    }

    double TpeType::getPeRange(void) const {
        if (!hasPe()) {
            return 0.0;
        }
        return m_data.subData->range / 300.0;
    }

    double TpeType::getPeDelay(void) const {
        return 0.0;
    }

    double TpeType::getPeAngle(void) const {
        if (!hasPe()) {
            return 0.0;
        }
        return m_data.subData->angle / 10.0;
    }

    void TpeType::removeThroughWaveEvent(double x, double y, double w, double h) {
        for (auto i = static_cast<int>(x * getTofdLines() + 1.5); std::cmp_less_equal(i, static_cast<int>((x + w) * getTofdLines() + 1.5)); i++) {
            for (auto j = static_cast<int>(y * 500.0 + 1.5); std::cmp_less_equal(j, static_cast<int>((y + h) * 500.0 + 1.5)); j++) {
                m_data.data[i * 500 + j] += -m_data.data[static_cast<int>(x * getTofdLines() - 0.5) * 500 + j] + 128;
            }
        }
    }

    void TpeType::pullThroughWaveEvent(double x, double y, double w, double h) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        // TODO: 实现拉直通波功能
    }

    void TpeType::backup(void) {
        if (!m_data_bak.has_value()) {
            m_data_bak = m_data;
        }
    }

    void TpeType::rollback(void) {
        if (m_data_bak.has_value()) {
            m_data = m_data_bak.value();
        }
    }

} // namespace Union::TOFD_PE::TPE
