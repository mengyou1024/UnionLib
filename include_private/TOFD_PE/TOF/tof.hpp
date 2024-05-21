#pragma once
#include "../TofdPeType.hpp"
#include "../TPE/tpe.hpp"
#include <memory>

namespace Union::TOFD_PE::TOF {
    using Union::TOFD_PE::TPE::Data;
    using Union::TOFD_PE::TPE::TpeType;
    class TofType : public TpeType {
    public:
        static std::unique_ptr<TofdPeIntf> FromFile(const std::wstring& fileName);

        virtual size_t __Read(std::ifstream& file, size_t file_size) override final;
    };
} // namespace Union::TOFD_PE::TOF
