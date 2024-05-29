#include <vdata.hpp>

#include <QLoggingCategory>
#include <Yo/Types>

static Q_LOGGING_CATEGORY(TAG, "VDATA");

namespace Yo::File {
    using namespace Union::__390N_T8;
    template <>
    size_t __Read(std::ifstream& file, Union::__390N_T8::VDATA& _ld, size_t file_size) {
        return _ld.__Read(file, file_size);
    }
} // namespace Yo::File

namespace Union::__390N_T8 {
    std::unique_ptr<Union::AScan::AScanIntf> VDATA::FromFile(const std::wstring& fileName) {
        auto vdat = std::make_unique<Union::__390N_T8::VDATA>();
        auto ret  = Yo::File::ReadFile(fileName, *(vdat.get()));
        if (ret) {
            QFileInfo info(QString::fromStdWString(fileName));
            vdat->pushFileNameList(info.completeBaseName().toStdWString());
            return vdat;
        }
        return nullptr;
    }

    size_t VDATA::__Read(std::ifstream& file, size_t file_size) {
        using namespace Yo::File;
        using namespace Yo::Types;
        if (file_size == 0) {
            return 0;
        }
        size_t               ret = 0;
        std::array<char, 14> dateTime;
        ret += Yo::File::__Read(file, SkipSize(8), file_size);
        ret += Yo::File::__Read(file, dateTime, file_size);
        time = convertTime(dateTime);
        do {
            _ldat temp;
            ret += Yo::File::__Read(file, SkipSize(8), file_size);
            temp.AScan.resize(Union::__390N_T8::ECHO_PACKAGE_SIZE);
            ret += Yo::File::__Read(file, temp.AScan, file_size);
            ret += Yo::File::__Read(file, SkipSize(42), file_size);
            ret += Yo::File::__Read(file, temp.dac_data, file_size);
            ret += Yo::File::__Read(file, temp.avg_data, file_size);
            ret += Yo::File::__Read(file, temp.chanel_data, file_size);
            ldat.emplace_back(temp);
            int img_size = 0;
            ret += Yo::File::__Read(file, img_size, file_size);
            uint8_t* img_buf = new uint8_t[img_size];
            file.read(reinterpret_cast<char*>(img_buf), img_size);
            ret += img_size;
            QImage img(img_buf, 160, 120, QImage::Format::Format_RGB888, [](void* buf) { delete[] static_cast<uint8_t*>(buf); });
            m_image.emplace_back(img);
        } while (file_size - ret > 4);
        if (file_size - ret == 4) {
            int all_buf_size = 0;
            ret += Yo::File::__Read(file, all_buf_size, file_size);
            if (std::cmp_not_equal(ldat.size(), all_buf_size) || std::cmp_not_equal(m_image.size(), all_buf_size)) {
                qWarning(TAG) << "ldat.size:" << ldat.size() << " m_image.size:" << m_image.size() << " all_buf_size:" << all_buf_size;
                return 0;
            }
            return ret;
        } else if (file_size - ret > 4) {
            return ret;
        }

        if (ret != file_size) {
            qWarning(TAG) << "ret: " << ret << " file_size:" << file_size;
        }
        return ret;
    }

    QImage VDATA::getCameraImage(int idx) const {
        return m_image.at(idx);
    }
} // namespace Union::__390N_T8
