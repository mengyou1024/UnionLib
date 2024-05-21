#include "tof.hpp"
#include <QDebug>
#include <QLoggingCategory>
#include <Yo/File>
#include <stdexcept>

static Q_LOGGING_CATEGORY(TAG, "TOF");

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::TOFD_PE::TOF::TofType &data, [[maybe_unused]] size_t file_size) {
        return data.__Read(file, file_size);
    }

} // namespace Yo::File

namespace Union::TOFD_PE::TOF {

    std::unique_ptr<TofdPeIntf> TofType::FromFile(const std::wstring &fileName) {
        auto ret = std::make_unique<TofType>();
        if (!Yo::File::ReadFile(fileName, (*(ret.get())))) {
            return nullptr;
        }
        return ret;
    }

    size_t TofType::__Read(std::ifstream &file, size_t file_size) {
        size_t ret = 0;
        try {
            ret += Yo::File::__Read(file, Yo::File::MakeStructSub(&(m_data.pulseMode), &(m_data.bandMode), sizeof(int32_t)), file_size);
            m_data.line--;
            m_data.data.resize((m_data.line + 1) * getAScanSize());
            ret += Yo::File::__Read(file, m_data.data, file_size);
            if (ret > file_size) {
                throw std::runtime_error("read files out of range");
            }
            return file_size;
        } catch (std::exception &e) {
            qCritical(TAG) << e.what();
            return 0;
        }
    }
} // namespace Union::TOFD_PE::TOF
