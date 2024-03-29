#include "ldat.hpp"
#include <optional>

#if __has_include("Yo/File")
namespace Yo::File {
    using namespace Union::__390N_T8;
    template <>
    size_t __Read(std::ifstream& file, Union::__390N_T8::LDAT& _ld, size_t file_size) {
        if (file_size < Union::__390N_T8::HEAD_LEN + Union::__390N_T8::BODY_LEN ||
            ((file_size - Union::__390N_T8::HEAD_LEN) % Union::__390N_T8::BODY_LEN != 0)) {
            return 0;
        }
        const auto ldat_size = (file_size - Union::__390N_T8::HEAD_LEN) / Union::__390N_T8::BODY_LEN;
        _ld.ldat.resize(ldat_size);
        size_t               read_size = 0;
        std::array<char, 14> dateTime;
        read_size += __Read(file, SkipSize(8), file_size);
        read_size += __Read(file, dateTime, file_size);
        std::ostringstream ss;
        for (int i = 0; std::cmp_less(i, dateTime.size()); i++) {
            ss << dateTime[i];
            if (i == 3 || i == 5) {
                ss << "-";
            } else if (i == 7) {
                ss << " ";
            } else if (i == 9 || i == 11) {
                ss << ":";
            }
        }
        std::string time_str = ss.str();
        _ld.time             = time_str;
        for (auto& it : _ld.ldat) {
            read_size += __Read(file, SkipSize(8), file_size);
            it.AScan.resize(Union::__390N_T8::ECHO_PACKAGE_SIZE);
            read_size += __Read(file, it.AScan, file_size);
            read_size += __Read(file, SkipSize(42), file_size);
            read_size += __Read(file, it.dac_data, file_size);
            read_size += __Read(file, it.avg_data, file_size);
            read_size += __Read(file, it.chanel_data, file_size);
        }
        return read_size;
    }
} // namespace Yo::File

#endif

namespace Union::AScan {
    template <>
    std::optional<AScanType> CONVERT_TO_STANDARD_ASCAN_TYPE(const Union::__390N_T8::LDAT& ldat) {
        AScanType ret;
        ret.instrumentName = "390N&T8 multiple";
        ret.time           = ldat.time;
        ret.data.resize(ldat.ldat.size());
        for (auto i = 0; std::cmp_less(i, ret.data.size()); i++) {
            auto& _t                   = ret.data[i];
            auto& _s                   = ldat.ldat[i];
            auto  axisBias             = _s.chanel_data.ch_yangshi;
            auto  axisLen              = _s.chanel_data.ch_sound_distance[_s.chanel_data.ch_sound_distance_type];
            _t.gate[0].enable          = true;
            _t.gate[0].pos             = (_s.chanel_data.ch_gatea_pos - axisBias) / static_cast<double>(axisLen);
            _t.gate[0].width           = (_s.chanel_data.ch_gatea_width - axisBias) / static_cast<double>(axisLen);
            _t.gate[0].height          = _s.chanel_data.ch_gatea_height / 100.0;
            _t.gate[1].pos             = (_s.chanel_data.ch_gatea_pos - axisBias) / static_cast<double>(axisLen);
            _t.gate[1].width           = (_s.chanel_data.ch_gatea_width - axisBias) / static_cast<double>(axisLen);
            _t.gate[1].height          = _s.chanel_data.ch_gateb_height / 100.0;
            _t.ascan                   = _s.AScan;
            _t.axisBias                = _s.chanel_data.ch_yangshi;
            _t.axisLen                 = _s.chanel_data.ch_sound_distance[_s.chanel_data.ch_sound_distance_type];
            _t.baseGain                = _s.chanel_data.ch_base_gain / 10.0;
            _t.scanGain                = _s.chanel_data.ch_scan_gain / 10.0;
            _t.surfaceCompentationGain = _s.chanel_data.ch_offset_gain;
            _t.suppression             = _s.chanel_data.ch_suppression;
            _t.distanceMode            = Union::AScan::DistanceMode(_s.chanel_data.ch_sound_distance_type);
            if (_s.dac_data.ch_already_dac == 1) {
                _t.dac            = Base::DAC();
                _t.dac->baseGain  = _s.dac_data.ch_dac_base_gain / 10.0;
                _t.dac->gate      = _s.dac_data.ch_dac_gate;
                _t.dac->isSubline = _s.dac_data.ch_dac_is_subline;
                _t.dac->index.clear();
                _t.dac->value.clear();
                for (auto dacIndex = 0; dacIndex < _s.dac_data.ch_dac_sample_length; dacIndex++) {
                    _t.dac->index.emplace_back(_s.dac_data.ch_dac_sample_index[dacIndex]);
                    _t.dac->value.emplace_back(_s.dac_data.ch_dac_sample_value[dacIndex]);
                }
                _t.dac->samplingDepth    = _s.dac_data.ch_dac_sampling_depth;
                _t.dac->decimationFactor = _s.dac_data.ch_dac_sampling_factor;
            }
            if (_s.avg_data.ch_already_avg == 1) {
                _t.avg                     = Base::AVG();
                _t.avg->baseGain           = _s.avg_data.ch_avg_base_gain / 10.0;
                _t.avg->biasGain           = _s.avg_data.ch_avg_offset_gain;
                _t.avg->equivalent         = _s.avg_data.ch_avg_dangliang;
                _t.avg->diameter           = _s.avg_data.ch_avg_diameter;
                _t.avg->isSubline          = _s.avg_data.ch_avg_is_subline;
                _t.avg->reflectorDianmeter = _s.avg_data.ch_avg_reflector_diameter;
                _t.avg->reflectorMaxDepth  = _s.avg_data.ch_avg_reflector_max_depth;
                _t.avg->index.clear();
                _t.avg->value.clear();
                for (auto avgIndex = 0; avgIndex < _s.avg_data.ch_avg_sample_length; avgIndex++) {
                    _t.avg->index.emplace_back(_s.avg_data.ch_avg_sample_index[avgIndex]);
                    _t.avg->value.emplace_back(_s.avg_data.ch_avg_sample_value[avgIndex]);
                }
                _t.avg->samplingDepth    = _s.avg_data.ch_avg_sampling_depth;
                _t.avg->decimationFactor = _s.avg_data.ch_avg_sampling_factor;
                _t.avg->scanGain         = _s.avg_data.ch_avg_scan_gain / 10.0;
            }
            _t.std.rlBias = _s.chanel_data.ch_xuan_rl;
            _t.std.slBias = _s.chanel_data.ch_xuan_sl;
            _t.std.elBias = _s.chanel_data.ch_xuan_el;
        }
        return ret;
    }
} // namespace Union::AScan
namespace Union::__390N_T8 {
    std::optional<Union::AScan::AScanType> __390N_T8_LDAT_READ(const std::wstring& fileName) {
#if __has_include("Yo/File")
        Union::__390N_T8::LDAT ldat = {};
        auto                   ret  = Yo::File::ReadFile(fileName, ldat);
    #if __has_include("QtCore")
        qDebug(QLoggingCategory("AScanType")) << "read :" << fileName << "res:" << ret;
    #endif
        if (!ret) {
    #if __has_include("QtCore")
            qWarning(QLoggingCategory("AScanType")) << "read:" << fileName << "error!";
    #endif
            return std::nullopt;
        }
        return Union::AScan::CONVERT_TO_STANDARD_ASCAN_TYPE(ldat);
#else
        return std::nullopt;
#endif
    }
} // namespace Union::__390N_T8
