#include "TOFD_PE/tofd_pe.hpp"
#include <Yo/File>

namespace Yo::File {
    template <>
    size_t __Read(std::ifstream &file, Union::__TOFD_PE::Data &data, [[maybe_unused]] size_t file_size) {
        size_t ret = 0;
        ret += __Read(file, MakeStructSub(&(data.pulseMode), &(data.bandMode), sizeof(int32_t)), file_size);
        data.data.resize((data.line + 1) * 500);
        ret += __Read(file, data.data, file_size);
        ret += __Read(file, data.flag, file_size);

        if (data.flag == 2) {
            data.subData = Union::__TOFD_PE::SubData();
            ret += __Read(file, MakeStructSub(&(data.subData->baseGain), &(data.subData->derect), sizeof(int32_t)), file_size);
            data.subData->data.resize((data.subData->line + 1) * 500);
            ret += __Read(file, data.subData->data, file_size);
        }
        return ret;
    }

} // namespace Yo::File

namespace Union::__TOFD_PE {
    std::optional<Data> Data::FromFile(const std::wstring &fileName) {
        Data ret = {};
        auto res = Yo::File::ReadFile(fileName, ret);
        if (res) {
            return ret;
        }
        return std::nullopt;
    }
#if __has_include("QString")
    std::optional<Data> Data::FromFile(const QString &fileName) {
        return FromFile(fileName.toStdWString());
    }
#endif
} // namespace Union::__TOFD_PE
