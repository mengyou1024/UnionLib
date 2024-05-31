#include "ldat.hpp"
#include <optional>

#include <Yo/Types>

namespace Yo::File {
    using namespace Union::__390N_T8;
    template <>
    size_t __Read(std::ifstream& file, Union::__390N_T8::LDAT& _ld, size_t file_size) {
        return _ld.__Read(file, file_size);
    }
} // namespace Yo::File

namespace Union::__390N_T8 {
    std::string LDAT::convertTime(std::array<char, 14> arr) const {
        std::ostringstream ss;
        for (int i = 0; std::cmp_less(i, arr.size()); i++) {
            ss << arr[i];
            if (i == 3 || i == 5) {
                ss << "-";
            } else if (i == 7) {
                ss << " ";
            } else if (i == 9 || i == 11) {
                ss << ":";
            }
        }
        return ss.str();
    }

    std::unique_ptr<Union::AScan::AScanIntf> LDAT::FromFile(const std::wstring& fileName) {
        auto ldat = std::make_unique<Union::__390N_T8::LDAT>();
        auto ret  = Yo::File::ReadFile(fileName, *(ldat.get()));
        if (ret) {
            QFileInfo info(QString::fromStdWString(fileName));
            ldat->pushFileNameList(info.completeBaseName().toStdWString());
            return ldat;
        }
        return nullptr;
    }

    size_t LDAT::__Read(std::ifstream& file, size_t file_size) {
        using namespace Yo::File;
        if (file_size < Union::__390N_T8::HEAD_LEN + Union::__390N_T8::BODY_LEN ||
            ((file_size - Union::__390N_T8::HEAD_LEN) % Union::__390N_T8::BODY_LEN != 0)) {
            return 0;
        }
        const auto ldat_size = (file_size - Union::__390N_T8::HEAD_LEN) / Union::__390N_T8::BODY_LEN;
        ldat.resize(ldat_size);
        size_t               read_size = 0;
        std::array<char, 14> dateTime;
        read_size += Yo::File::__Read(file, SkipSize(8), file_size);
        read_size += Yo::File::__Read(file, dateTime, file_size);
        time = convertTime(dateTime);
        for (auto& it : ldat) {
            read_size += Yo::File::__Read(file, SkipSize(8), file_size);
            it.AScan.resize(Union::__390N_T8::ECHO_PACKAGE_SIZE);
            read_size += Yo::File::__Read(file, it.AScan, file_size);
            read_size += Yo::File::__Read(file, SkipSize(42), file_size);
            read_size += Yo::File::__Read(file, it.dac_data, file_size);
            read_size += Yo::File::__Read(file, it.avg_data, file_size);
            read_size += Yo::File::__Read(file, it.chanel_data, file_size);
        }
        return read_size;
    }

    int LDAT::getDataSize(void) const {
        return static_cast<int>(ldat.size());
    }

    std::vector<std::wstring> LDAT::getFileNameList(void) const {
        return m_fileNameList;
    }

    void LDAT::setFileNameIndex(int index) {
        (void)index;
    }

    Base::Performance LDAT::getPerformance(int idx) const {
        Base::Performance ret;
        if (ldat.empty()) {
            return ret;
        }
        ret.horizontalLinearity = ldat.at(idx).chanel_data.ch_yiqi_shuipin / 10.0;
        ret.verticalLinearity   = ldat.at(idx).chanel_data.ch_yiqi_chuzhi / 10.0;
        ret.resolution          = ldat.at(idx).chanel_data.ch_yiqi_fbl / 10.0;
        ret.dynamicRange        = ldat.at(idx).chanel_data.ch_yiqi_dongtai / 10.0;
        ret.sensitivity         = ldat.at(idx).chanel_data.ch_yiqi_lmd / 10.0;
        return ret;
    }

    std::string LDAT::getDate(int idx) const {
        (void)idx;
        return time;
    }

    std::wstring LDAT::getProbe(int idx) const {
        return Union::Base::Probe::Index2Name_QtExtra(ldat.at(idx).chanel_data.ch_probe_type).toStdWString();
    }

    double LDAT::getProbeFrequence(int idx) const {
        return ldat.at(idx).chanel_data.ch_probe_freq / 100.0;
    }

    std::string LDAT::getProbeChipShape(int idx) const {
        auto [_a, _b] = ldat.at(idx).chanel_data.ch_probe_size;
        return Union::Base::Probe::CreateProbeChipShape(ldat.at(idx).chanel_data.ch_probe_type, _a, _b);
    }

    double LDAT::getAngle(int idx) const {
        return ldat.at(idx).chanel_data.ch_k_value[1] / 10.0;
    }

    double LDAT::getSoundVelocity(int idx) const {
        return ldat.at(idx).chanel_data.ch_sound_speed;
    }

    double LDAT::getFrontDistance(int idx) const {
        return ldat.at(idx).chanel_data.ch_probe_frontier;
    }

    double LDAT::getZeroPointBias(int idx) const {
        return KeepDecimals((ldat.at(idx).chanel_data.ch_zero_point) / 100000.0);
    }

    double LDAT::getSamplingDelay(int idx) const {
        return getAxisBias(idx);
    }

    int LDAT::getChannel(int idx) const {
        return ldat.at(idx).chanel_data.ch_id;
    }

    std::string LDAT::getInstrumentName(void) const {
        return "390N&T8 multiple";
    }

    std::array<Base::Gate, 2> LDAT::getGate(int idx) const {
        std::array<Base::Gate, 2> ret;
        ret.at(0).pos    = (ldat.at(idx).chanel_data.ch_gatea_pos) / getAxisLen(idx);
        ret.at(0).width  = (ldat.at(idx).chanel_data.ch_gatea_width) / getAxisLen(idx);
        ret.at(0).height = ldat.at(idx).chanel_data.ch_gatea_height / 100.0;
        ret.at(1).pos    = (ldat.at(idx).chanel_data.ch_gateb_pos) / getAxisLen(idx);
        ret.at(1).width  = (ldat.at(idx).chanel_data.ch_gateb_width) / getAxisLen(idx);
        ret.at(1).height = ldat.at(idx).chanel_data.ch_gateb_height / 100.0;
        for (auto& it : ret) {
            if (it.height > 0.0) {
                it.enable = true;
            }
        }
        return ret;
    }

    const std::vector<uint8_t>& LDAT::getScanData(int idx) const {
        return ldat.at(idx).AScan;
    }

    double LDAT::getAxisBias(int idx) const {
        return KeepDecimals<1>(ldat.at(idx).chanel_data.ch_yangshi / 100.0);
    }

    double LDAT::getAxisLen(int idx) const {
        const auto distanceMode = ldat.at(idx).chanel_data.ch_sound_distance_type;
        return ldat.at(idx).chanel_data.ch_sound_distance[static_cast<int>(distanceMode)];
    }

    double LDAT::getBaseGain(int idx) const {
        return ldat.at(idx).chanel_data.ch_base_gain / 10.0;
    }

    double LDAT::getScanGain(int idx) const {
        return ldat.at(idx).chanel_data.ch_scan_gain / 10.0;
    }

    double LDAT::getSurfaceCompentationGain(int idx) const {
        return ldat.at(idx).chanel_data.ch_offset_gain / 10.0;
    }

    int LDAT::getSupression(int idx) const {
        return KeepDecimals<0>(ldat.at(idx).chanel_data.ch_suppression / 10.0);
    }

    Union::AScan::DistanceMode LDAT::getDistanceMode(int idx) const {
        return Union::AScan::DistanceMode(ldat.at(idx).chanel_data.ch_sound_distance_type);
    }

    std::optional<Base::AVG> LDAT::getAVG(int idx) const {
        if (ldat.at(idx).avg_data.ch_already_avg != 1) {
            return std::nullopt;
        }
        Base::AVG ret;
        ret.baseGain           = ldat.at(idx).avg_data.ch_avg_base_gain / 10.0;
        ret.biasGain           = ldat.at(idx).avg_data.ch_avg_offset_gain;
        ret.equivalent         = ldat.at(idx).avg_data.ch_avg_dangliang;
        ret.diameter           = ldat.at(idx).avg_data.ch_avg_diameter;
        ret.isSubline          = ldat.at(idx).avg_data.ch_avg_is_subline;
        ret.reflectorDianmeter = ldat.at(idx).avg_data.ch_avg_reflector_diameter;
        ret.reflectorMaxDepth  = ldat.at(idx).avg_data.ch_avg_reflector_max_depth;
        ret.index.clear();
        ret.value.clear();
        for (auto avgIndex = 0; std::cmp_less(avgIndex, ldat.at(idx).avg_data.ch_avg_sample_length); avgIndex++) {
            ret.index.emplace_back(ldat.at(idx).avg_data.ch_avg_sample_index[avgIndex]);
            ret.value.emplace_back(ldat.at(idx).avg_data.ch_avg_sample_value[avgIndex]);
        }
        ret.samplingDepth    = ldat.at(idx).avg_data.ch_avg_sampling_depth;
        ret.decimationFactor = ldat.at(idx).avg_data.ch_avg_sampling_factor;
        ret.scanGain         = ldat.at(idx).avg_data.ch_avg_scan_gain / 10.0;
        return ret;
    }

    std::optional<Base::DAC> LDAT::getDAC(int idx) const {
        if (ldat.at(idx).dac_data.ch_already_dac != 1) {
            return std::nullopt;
        }
        Base::DAC ret = {};
        ret.baseGain  = ldat.at(idx).dac_data.ch_dac_base_gain / 10.0;
        ret.gate      = ldat.at(idx).dac_data.ch_dac_gate;
        ret.isSubline = ldat.at(idx).dac_data.ch_dac_is_subline;
        ret.index.clear();
        ret.value.clear();
        for (auto dacIndex = 0; std::cmp_less(dacIndex, ldat.at(idx).dac_data.ch_dac_sample_length); dacIndex++) {
            ret.index.emplace_back(ldat.at(idx).dac_data.ch_dac_sample_index[dacIndex]);
            ret.value.emplace_back(ldat.at(idx).dac_data.ch_dac_sample_value[dacIndex]);
        }
        ret.samplingDepth    = ldat.at(idx).dac_data.ch_dac_sampling_depth;
        ret.decimationFactor = ldat.at(idx).dac_data.ch_dac_sampling_factor;
        return ret;
    }

    Union::AScan::DAC_Standard LDAT::getDACStandard(int idx) const {
        Union::AScan::DAC_Standard ret;
        ret.rlBias = ldat.at(idx).chanel_data.ch_xuan_rl / 10.0;
        ret.slBias = ldat.at(idx).chanel_data.ch_xuan_sl / 10.0;
        ret.elBias = ldat.at(idx).chanel_data.ch_xuan_el / 10.0;
        return ret;
    }

    void LDAT::pushFileNameList(const std::wstring& fileName) {
        m_fileNameList.push_back(fileName);
    }
    QJsonArray LDAT::createGateValue(int idx, double soft_gain) const {
        QJsonArray ret = Union::AScan::AScanIntf::createGateValue(idx, soft_gain);

        std::array<QString, 2> m_equi = {"-", "-"};
        std::array<QString, 2> m_a    = {"-", "-"};
        std::array<QString, 2> m_b    = {"-", "-"};
        std::array<QString, 2> m_c    = {"-", "-"};

        if (ldat.at(idx).dac_data.ch_already_dac) {
            auto                 index      = ldat.at(idx).chanel_data.ch_equivalent_standard;
            double               equivalent = ldat.at(idx).chanel_data.ch_flaw_equivalent / 10.0;
            constexpr std::array lstrequi   = {" ", "RL", "SL", "EL"};
            m_equi[0]                       = QString::asprintf("%s %+.1fdB", lstrequi[index], equivalent);
        } else if (ldat.at(idx).avg_data.ch_already_avg) {
            auto reflector_diameter = ldat.at(idx).avg_data.ch_avg_reflector_diameter;
            auto equivlant          = ldat.at(idx).chanel_data.ch_flaw_equivalent / 10.0;
            auto avg_diameter       = ldat.at(idx).avg_data.ch_avg_diameter;
            m_equi[0]               = QString::asprintf("Φ%.1f Φ%.1f %+.1fdB", avg_diameter, reflector_diameter, equivlant);
        }

        m_c[0] = QString::asprintf("%.1f", ldat.at(idx).chanel_data.ch_flaw_actual_dist / 10.0);
        m_a[0] = QString::asprintf("%.1f", ldat.at(idx).chanel_data.ch_flaw_horizontal_dist / 10.0);
        m_b[0] = QString::asprintf("%.1f", ldat.at(idx).chanel_data.ch_flaw_depth / 10.0);

        auto obj1      = ret[0].toObject();
        auto obj2      = ret[1].toObject();
        obj1["equi"]   = m_equi[0];
        obj1["dist_c"] = m_c[0];
        obj1["dist_a"] = m_a[0];
        obj1["dist_b"] = m_b[0];
        obj2["equi"]   = m_equi[1];
        obj2["dist_c"] = m_c[1];
        obj2["dist_a"] = m_a[1];
        obj2["dist_b"] = m_b[1];
        ret.replace(0, obj1);
        ret.replace(1, obj2);
        return ret;
    }

    std::pair<double, double> LDAT::getProbeSize(int idx) const {
        auto [_a, _b] = ldat.at(idx).chanel_data.ch_probe_size;
        return std::make_pair(static_cast<double>(_a), static_cast<double>(_b));
    }
} // namespace Union::__390N_T8
