#include "TofdBridge.h"
#include <QBitmap>
#include <QDebug>
#include <QFileInfo>
#include <QLoggingCategory>

namespace Union::TOFD_MULTI::__9x0 {

    static Q_LOGGING_CATEGORY(TAG, "TOFD_MULTI.9x0");

    bool ParseUDFile(QString path, SYSTEM_CONFIG_PARAS* psysconfig, CHANNEL_CONFIG_PARAS* pchconfig, QSize& imgsz, int& version) {
        if (path.isEmpty()) {
            qCWarning(TAG) << "file:" << path << "is empty";
            return false;
        }

        FILE* fp  = nullptr;
        auto  err = fopen_s(&fp, path.toStdString().c_str(), "rb");
        if (err != 0) {
            qCWarning(TAG) << "file:" << path << "open failed, errno:" << err;
            return false;
        }

        int file_size, image_size, para_size;
        int image_offset, para_offset;

        QFileInfo fileInfo(path);
        QImage    image(path);

        // 文件的总大小
        file_size = fileInfo.size();
        // 数据区的偏移量
        image_offset = 1078;
        // 图像数据的大小
        image_size = image.sizeInBytes();
        // 尾部参数信息的偏移量
        para_offset = image_offset + image_size;
        // 尾部参数信息的大小
        para_size = file_size - (image_offset + image_size);

        fseek(fp, para_offset, SEEK_SET);
        std::byte* pParaData = new std::byte[para_size];
        fread(pParaData, para_size, 1, fp);
        fclose(fp);

        version           = 0;
        int version1_size = sizeof(SYSTEM_CONFIG_PARAS) + sizeof(CHANNEL_CONFIG_PARAS) * 8;
        int version2_size = sizeof(SYSTEM_CONFIG_PARAS) + sizeof(CHANNEL_CONFIG_PARAS) * CHANNEL_NUM;

        if (para_size == version1_size) {
            version = 1;
        } else if (para_size == version2_size) {
            version = 2;
        }

        imgsz = image.size();

        memset(psysconfig, 0, sizeof(SYSTEM_CONFIG_PARAS));
        memset(pchconfig, 0, sizeof(CHANNEL_CONFIG_PARAS) * CHANNEL_NUM);

        if (version == 0) {
            // TODO: 移除这段注释
            /*CopyMemory(psysconfig, pParaData, sizeof(SYSTEM_CONFIG_PARAS));
            CopyMemory(pchconfig, pParaData + sizeof(SYSTEM_CONFIG_PARAS), sizeof(CHANNEL_CONFIG_PARAS) * 8);*/
            qCWarning(TAG) << "unrecognized file:" << path;
        } else if (version == 1) {
            memcpy(psysconfig, pParaData, sizeof(SYSTEM_CONFIG_PARAS));
            memcpy(pchconfig, pParaData + sizeof(SYSTEM_CONFIG_PARAS), sizeof(CHANNEL_CONFIG_PARAS) * 8);
        } else if (version == 2) {
            memcpy(psysconfig, pParaData, sizeof(SYSTEM_CONFIG_PARAS));
            memcpy(pchconfig, pParaData + sizeof(SYSTEM_CONFIG_PARAS), sizeof(CHANNEL_CONFIG_PARAS) * CHANNEL_NUM);
        }
        delete pParaData;
        return true;
    }

    TofdBridge::TofdBridge() {
        m_nFileVersion            = 0;
        m_nDisplayLayoutDirection = Horizontal;
        m_nChannelNum             = 0;
        m_nCurChannel             = -1;
        m_nCurChannelIndex        = -1;

        m_isDisplayCross = true;
        m_isDisplayTime  = false;
        m_isAdjustRadius = false;

        for (int i = 0; i < CHANNEL_NUM; i++) {
            m_isChannelActive[i] = false;
            m_isChannelValid[i]  = false;

            m_nCompensateGain[i] = 0;
            m_fXOffset[i]        = 0;
            m_fAdjustDepth[i]    = 0;
            m_fAdjustPosition[i] = 0;
            m_fAdjustPCS[i]      = 0;
            m_fAdjustVelocity[i] = 0;

            m_ViewRectangleInfo[i].SrcData     = nullptr;
            m_ViewRectangleInfo[i].TempData    = nullptr;
            m_ViewRectangleInfo[i].BakData     = nullptr;
            m_ViewRectangleInfo[i].SrcDataLine = nullptr;
        }
    }

    TofdBridge::~TofdBridge() {
        for (int i = 0; i < CHANNEL_NUM; i++) {
            if (m_ViewRectangleInfo[i].SrcData) {
                delete[] m_ViewRectangleInfo[i].SrcData;
                m_ViewRectangleInfo[i].SrcData = nullptr;
            }
            if (m_ViewRectangleInfo[i].TempData) {
                delete[] m_ViewRectangleInfo[i].TempData;
                m_ViewRectangleInfo[i].TempData = nullptr;
            }
            if (m_ViewRectangleInfo[i].BakData) {
                delete[] m_ViewRectangleInfo[i].BakData;
                m_ViewRectangleInfo[i].BakData = nullptr;
            }
            if (m_ViewRectangleInfo[i].SrcDataLine) {
                delete[] m_ViewRectangleInfo[i].SrcDataLine;
                m_ViewRectangleInfo[i].SrcDataLine = nullptr;
            }
        }
    }

    bool TofdBridge::OpenFile(QString filepath) {
        if (filepath.isEmpty()) {
            return false;
        }
        m_FilePath = filepath;

        QSize image_size;

        bool ret = ParseUDFile(filepath, &m_SystemPara, m_ChannelPara, image_size, m_nFileVersion);
        if (ret != true) {
            qCDebug(TAG) << "ParseUDFile error: " << filepath;
        }

        m_iScanLength = image_size.height();
        m_nChannelNum = m_SystemPara.ChannelNumber;

        memset(m_isChannelValid, 0, sizeof(m_isChannelValid));
        for (int i = 0; i < CHANNEL_NUM; i++) {
            int p               = (1 << i);
            m_isChannelValid[i] = (p & m_SystemPara.ChannelFlag);
        }
        memcpy(m_isChannelActive, m_isChannelValid, sizeof(m_isChannelActive));

        m_nCurChannel = -1;
        for (int i = 0; i < CHANNEL_NUM; i++) {
            if (!m_isChannelActive[i]) {
                continue;
            }

            if (m_nCurChannel < 0) {
                m_nCurChannel = i;
            }

            m_fAdjustDepth[i]    = m_ChannelPara[i].AdjustDepth;
            m_fAdjustPosition[i] = m_ChannelPara[i].AdjustPosition;
            m_fAdjustPCS[i]      = m_ChannelPara[i].ProberCenterDistance;
            m_fAdjustVelocity[i] = m_ChannelPara[i].Velocity;
        }

        ParseChannelIndex();

        return true;
    }

    QString TofdBridge::GetParametersFormat() {
        QString ret;
        QString temp;
        if (m_SystemPara.ChannelNumber == 0) {
            return ret;
        }

        temp.asprintf("系统参数:\r\n");
        ret += temp;
        switch (m_SystemPara.Voltage) {
            case 0:
                temp.asprintf("   发射电压:  50V");
                break;
            case 1:
                temp.asprintf("   发射电压: 100V");
                break;
            case 2:
                temp.asprintf("   发射电压: 200V");
                break;
            case 3:
                temp.asprintf("   发射电压: 300V");
                break;
            default:
                temp = "";
        }
        ret += temp;
        temp.asprintf("\t重复频率: %5dHz", m_SystemPara.RepeatFreq);
        ret += temp;
        temp.asprintf("\t平均次数: %2d\r\n", 1 << m_SystemPara.AverageTimes);
        ret += temp;

        switch (m_SystemPara.ScanMode) {
            case 0:
                temp.asprintf("   扫查方式: 非平行");
                break;
            case 1:
                temp.asprintf("   扫查方式: 平行");
                break;
            default:
                temp = "";
        }
        ret += temp;

        switch (m_SystemPara.TriggerMode) {
            case 0: {
                double v = m_SystemPara.Encoder1_Resolution;
                v        = v * pow(2.0, (3 - m_SystemPara.Encoder1_Increment));
                temp.asprintf("\t触发方式: 编码器一\t扫查增量: %4.2fmm\r\n", v);
                break;
            }
            case 1: {
                double v = m_SystemPara.Encoder2_Resolution;
                v        = v * pow(2.0, (m_SystemPara.Encoder2_Increment - 3));
                temp.asprintf("\t触发方式:  时间\t扫查增量: %4.2fms\r\n", v);
                break;
            }
            default:
                temp = "";
        }
        ret += temp;
        ret += "\r\n";

        temp = "工件情况:\r\n";
        ret += temp;
        temp.asprintf("   工件厚度:  %.1fmm", m_SystemPara.WorkpieceThickness);
        ret += temp;
        temp.asprintf("\t焊缝宽度:  %.1fmm", m_SystemPara.WorkpieceWeldWidth);
        ret += temp;
        switch (m_SystemPara.WorkpieceBevelType) {
            case 0: // V
                temp.asprintf("\t坡口类型:  V型\r\n");
                break;
            case 1: // U
                temp.asprintf("\t坡口类型:  U型\r\n");
                break;
            case 2: // X
                temp.asprintf("\t坡口类型:  X型\r\n");
                break;
            default:
                temp.asprintf("\t坡口类型:  未知\r\n");
        }
        ret += temp;
        temp.asprintf("   工件曲率半径:  %.1fmm\r\n", (float)m_SystemPara.WorkpieceRadius);
        ret += temp;
        ret += "\r\n";

        for (int i = 0; i < CHANNEL_NUM; i++) {
            if (!m_isChannelActive[i]) {
                continue;
            }
            int send, recv;
            send = (m_ChannelPara[i].ProbeSelect & (0x0000000f));
            recv = m_ChannelPara[i].ProbeSelect >> 4;
            if (i < TOFD_CHANNEL_NUM) {
                temp.asprintf("第%d通道参数: (类型:TOFD 发射通道:%d 接收通道:%d) [ PCS = %.1fmm ]\r\n", i + 1, send + 1, recv + 1, m_ChannelPara[i].ProberCenterDistance);
            } else {
                temp.asprintf("第%d通道参数: (类型:PE 发射通道:%d 接收通道:%d) [ PCS = %.1fmm ]\r\n", i + 1, send + 1, recv + 1, m_ChannelPara[i].ProberCenterDistance);
            }

            ret += temp;
            temp.asprintf("   增益: %4.2fdB\t声程: %4.2fus\t延时: %4.2fus\t脉冲宽度: %4dns\r\n",
                          m_ChannelPara[i].Gain, m_ChannelPara[i].EchoRange, m_ChannelPara[i].Delay, m_ChannelPara[i].PulseWidth);
            ret += temp;
            if (1) {
                switch (m_ChannelPara[i].Filter) {
                    case 0:
                        temp = "   滤波频带:  1 ~ 5MHz";
                        break;
                    case 1:
                        temp = "   滤波频带:  2 ~ 10MHz";
                        break;
                    case 2:
                        temp = "   滤波频带:  4 ~ 15MHz";
                        break;
                    case 3:
                        temp = "   滤波频带:  0.5 ~ 30MHz";
                        break;
                    default:
                        temp = "";
                }
            } else {
                switch (m_ChannelPara[i].Filter) {
                    case 0:
                        temp = "   滤波频带:  1.5 ~ 4.2MHz";
                        break;
                    case 1:
                        temp = "   滤波频带:  4.3 ~ 7.1MHz";
                        break;
                    case 2:
                        temp = "   滤波频带:  8.7 ~ 11.6MHz";
                        break;
                    case 3:
                        temp = "   滤波频带:  0.3 ~ 17MHz";
                        break;
                    case 4:
                        temp = "   滤波频带:  0.5~5.5MHz";
                        break;
                    case 5:
                        temp = "   滤波频带:  2.2~9.4MHz";
                        break;
                    case 6:
                        temp = "   滤波频带:  3.7~13MHz";
                        break;
                    case 7:
                        temp = "   滤波频带:  0.3~33MHz";
                        break;
                    case 8:
                        temp = "   滤波频带:  0.68~1.5MHz";
                        break;
                    case 9:
                        temp = "   滤波频带:  1.58~3.24MHz";
                        break;
                    case 10:
                        temp = "   滤波频带:  1.71~3.59MHz";
                        break;
                    case 11:
                        temp = "   滤波频带:  1.44~2.86MHz";
                        break;
                    case 12:
                        temp = "   滤波频带:  3.49~7.09MHz";
                        break;
                    case 13:
                        temp = "   滤波频带:  5.25~10.49MHz";
                        break;
                    case 14:
                        temp = "   滤波频带:  6.96~14.10MHz";
                        break;
                    case 15:
                        temp = "   滤波频带:  10.43~21.10MHz";
                        break;
                    case 16:
                        temp = "   滤波频带:  0.51~2.14MHz";
                        break;
                    case 17:
                        temp = "   滤波频带:  1~4MHz";
                        break;
                    case 18:
                        temp = "   滤波频带:  1.16~4.44MHz";
                        break;
                    case 19:
                        temp = "   滤波频带:  1.25~5MHz";
                        break;
                    case 20:
                        temp = "   滤波频带:  2.44~9.87MHz";
                        break;
                    case 21:
                        temp = "   滤波频带:  3.65~14.8MHz";
                        break;
                    case 22:
                        temp = "   滤波频带:  4.8~19.6MHz";
                        break;
                    case 23:
                        temp = "   滤波频带:  7.1~29.3MHz";
                        break;
                    case 24:
                        temp = "   滤波频带:  1.5 - 6MHz";
                        break;
                    case 25:
                        temp = "   滤波频带:  2 - 8MHz";
                        break;
                    case 26:
                        temp = "   滤波频带:  0.3 - 4MHz";
                        break;
                    case 27:
                        temp = "   滤波频带:  0.3 - 5MHz";
                        break;
                    case 28:
                        temp = "   滤波频带:  0.3 - 10MHz";
                        break;
                    case 29:
                        temp = "   滤波频带:  0.3 - 14MHz";
                        break;
                    default:
                        temp = "";
                }
            }
            ret += temp;
            switch (m_ChannelPara[i].EchoMode) {
                case 3:
                    temp = "\t检波方式:  射  频";
                    break;
                case 0:
                    temp = "\t检波方式:  全  波";
                    break;
                case 1:
                    temp = "\t检波方式:  正半波";
                    break;
                case 2:
                    temp = "\t检波方式:  负半波";
                    break;

                default:
                    temp = "";
            }
            ret += temp;
            switch (m_ChannelPara[i].ProberType) {
                case 0:
                    temp = "\t探头类型:  自发自收\r\n";
                    break;
                case 1:
                    temp = "\t探头类型:  一发一收\r\n";
                    break;
                case 2:
                    temp = "\t探头类型:  穿透\r\n";
                    break;
                default:
                    temp = "";
            }
            ret += temp;
            temp.asprintf("   频率: %3.1fMHz\t角度: %3.1f度\t尺寸: %3.1fmm\t前沿: %.1fmm\t零点: %.2fus\r\n\r\n",
                          m_ChannelPara[i].ProberFreq, m_ChannelPara[i].ProberAngle, m_ChannelPara[i].ProberDimension, m_ChannelPara[i].ProberFront, m_ChannelPara[i].ProberZero);
            ret += temp;
        }

        return ret;
    }

    void TofdBridge::ResetParameter() {
        m_fAdjustDepth[m_nCurChannel]    = m_ChannelPara[m_nCurChannel].AdjustDepth;
        m_fAdjustPosition[m_nCurChannel] = m_ChannelPara[m_nCurChannel].AdjustPosition;
        m_fAdjustPCS[m_nCurChannel]      = m_ChannelPara[m_nCurChannel].ProberCenterDistance;
        m_fAdjustVelocity[m_nCurChannel] = m_ChannelPara[m_nCurChannel].Velocity;
    }

    float TofdBridge::CalculateAdjustZero(int ChannelID) {
        if (m_fAdjustDepth[ChannelID] == 0) {
            if (m_fAdjustPosition[ChannelID] == 0) {
                return m_fAdjustPCS[ChannelID] / (m_fAdjustVelocity[ChannelID] / 1000.0f);
            } else {
                return m_fAdjustPosition[ChannelID];
            }
        }
        float sd     = m_fAdjustPCS[ChannelID] / 2.0f;
        float dd     = m_fAdjustDepth[ChannelID];
        float deltaL = 2 * sqrt(sd * sd + dd * dd) - 2 * sd;
        float ret    = m_fAdjustPosition[ChannelID] - deltaL / (m_fAdjustVelocity[ChannelID] / 1000.0f);

        return ret;
    }

    float TofdBridge::CalculateCurveLen(int ChannelID) {
        float ret = 0.0;
        // 计算曲率半径 凸面
        if (m_SystemPara.WorkpieceRadius > 0.01 && m_SystemPara.WorkpieceRadius < 150) {
            float sd      = m_fAdjustPCS[ChannelID] / 2.0f;
            float headlen = m_SystemPara.WorkpieceRadius - sqrt(m_SystemPara.WorkpieceRadius * m_SystemPara.WorkpieceRadius - sd * sd);
            ret           = headlen;
        }
        return ret;
    }

    bool TofdBridge::CalculateXOffset(int ChannelID) {
        (void)ChannelID;
        // double minlen = 99999.0, maxlen = -99999.0;
        // int minid = -1, maxid = -1;
        // for (int i = 0; i < MAXIMAGE; i++)
        //{
        //	if (!m_isChannelActive[i])
        //	{
        //		continue;
        //	}
        //	if (m_ChannelPara[i].StartOrigin < minlen)
        //	{
        //		minid = i;
        //		minlen = m_ChannelPara[i].StartOrigin;
        //	}
        //	if (m_ChannelPara[i].StartOrigin > maxlen)
        //	{
        //		maxid = i;
        //		maxlen = m_ChannelPara[i].StartOrigin;
        //	}
        // }
        // m_fOffsetmaxlen = maxlen - minlen;
        // ZeroMemory(m_fOffset, MAXIMAGE * sizeof(double));
        // double baselen = minlen;
        // if (ChannelID != -1)
        //{
        //	baselen = m_ChannelPara[ChannelID].StartOrigin;
        // }
        // for (int i = 0; i < MAXIMAGE; i++)
        //{
        //	if (!m_isChannelActive[i])
        //	{
        //		continue;
        //	}
        //	m_fOffset[i] = m_ChannelPara[i].StartOrigin - baselen;
        // }
        return true;
    }

    void TofdBridge::LoadImageData() {
        // 加载图谱
        QImage image;
        image.load(m_FilePath);
        int    linenum = image.height();
        int    datalen = ((image.width() + 1) >> 1) << 1;
        uchar* data    = image.bits();

        int sumlen = 0;
        for (int i = 0; i < CHANNEL_NUM; i++) {
            if (!m_isChannelActive[i]) {
                continue;
            }
            int len;
            if (i < TOFD_CHANNEL_NUM) {
                len = m_ChannelPara[i].EchoRange * SAMPLING_RATE;

            } else {
                len = 512;
            }

            // 添加X轴偏移
            int offline[2];
            offline[0] = 0;
            offline[1] = 0;

            if (m_ViewRectangleInfo[i].SrcData) {
                delete[] m_ViewRectangleInfo[i].SrcData;
                m_ViewRectangleInfo[i].SrcData = NULL;
            }
            if (m_ViewRectangleInfo[i].TempData) {
                delete[] m_ViewRectangleInfo[i].TempData;
                m_ViewRectangleInfo[i].TempData = NULL;
            }
            if (m_ViewRectangleInfo[i].BakData) {
                delete[] m_ViewRectangleInfo[i].BakData;
                m_ViewRectangleInfo[i].BakData = NULL;
            }
            if (m_ViewRectangleInfo[i].SrcDataLine) {
                delete[] m_ViewRectangleInfo[i].SrcDataLine;
                m_ViewRectangleInfo[i].SrcDataLine = NULL;
            }
            int sumline                        = linenum + offline[0] + offline[1];
            m_ViewRectangleInfo[i].SrcData     = new std::byte[sumline * len];
            m_ViewRectangleInfo[i].BakData     = new std::byte[sumline * len];
            m_ViewRectangleInfo[i].TempData    = new std::byte[sumline * len];
            m_ViewRectangleInfo[i].SrcDataLine = new std::byte[len];
            m_ViewRectangleInfo[i].XDataSize   = len;
            m_ViewRectangleInfo[i].YDataSize   = sumline;
            if (i < TOFD_CHANNEL_NUM) {
                memset(m_ViewRectangleInfo[i].SrcData, 255, offline[0] * len);
            } else {
                memset(m_ViewRectangleInfo[i].SrcData, 0, offline[0] * len);
            }
            for (int j = 0; j < linenum; j++) {
                memcpy(m_ViewRectangleInfo[i].SrcData + j * len + offline[0] * len, data + j * datalen + sumlen, len);
            }
            if (i < TOFD_CHANNEL_NUM) {
                memset(m_ViewRectangleInfo[i].SrcData + (offline[0] + linenum) * len, 255, offline[1] * len);
            } else {
                memset(m_ViewRectangleInfo[i].SrcData + (offline[0] + linenum) * len, 0, offline[1] * len);
            }
            memcpy(m_ViewRectangleInfo[i].BakData, m_ViewRectangleInfo[i].SrcData, sumline * len);
            memcpy(m_ViewRectangleInfo[i].TempData, m_ViewRectangleInfo[i].SrcData, sumline * len);

            sumlen += len;
        }
    }

    void TofdBridge::LoadLayout() {
        int channelActiveNum = 0;
        for (int i = 0; i < CHANNEL_NUM; i++) {
            if (m_isChannelActive[i])
                channelActiveNum++;
        }

        QRect displayRect = m_WindowRect;

        // 显示区域宽度
        int displayWidth = displayRect.width();
        // 显示区域高度度
        int displayHeight = displayRect.height();

        // A扫像素宽度占比
        const float ASCAN_PIXEL_RATIO = 0.15f;

        // 分隔线像素宽度
        const int SEPARATOR_PIXEL = 4;
        // 卡尺像素宽度
        const int CALIPER_PIXEL = 20;
        // 滚动条像素宽度
        const int SCROLL_PIXEL = 16;

        int caliperPixel, scrollPixel;
        if (m_isDisplayCaliper) {
            caliperPixel = CALIPER_PIXEL;
        } else {
            caliperPixel = 0;
        }

        if (m_isDisplayScroll) {
            scrollPixel = SCROLL_PIXEL;
        } else {
            scrollPixel = 0;
        }

        if (m_nDisplayLayoutDirection == Vertical) {
            // 竖项排列

            // A扫像素宽度
            int ascan_pixel = ASCAN_PIXEL_RATIO * displayHeight;
            // B扫像素宽度
            int image_pixel = (displayWidth - SEPARATOR_PIXEL * (channelActiveNum - 1)) / channelActiveNum;
            // A扫像素宽度+分隔符宽度
            int ascan_with_separator_pixel = ascan_pixel + SEPARATOR_PIXEL;
            // B扫像素宽度+分隔符宽度
            int image_with_separator_pixel = image_pixel + SEPARATOR_PIXEL;

            for (int ch = 0, i = 0; ch < CHANNEL_NUM; ch++) {
                if (!m_isChannelActive[ch]) {
                    continue;
                }

                QRect windowRect;
                QRect windowRect0, imageRect0, yCaliperRect0, yColorMapRect0;
                QRect windowRect1, imageRect1, xCaliperRect1, yCaliperRect1, xScrollRect1, yScrollRect1;

                // 整体区域
                windowRect = displayRect;
                windowRect.setLeft(displayRect.left() + i * image_with_separator_pixel);
                windowRect.setRight(displayRect.left() + (i + 1) * image_with_separator_pixel - SEPARATOR_PIXEL);

                // A扫
                windowRect0 = windowRect;
                windowRect0.setBottom(windowRect.top() + ascan_pixel);

                imageRect0 = windowRect0;
                imageRect0.setLeft(windowRect0.left() + caliperPixel + scrollPixel);

                yCaliperRect0 = windowRect0;
                yCaliperRect0.setLeft(windowRect0.left() + scrollPixel);
                yCaliperRect0.setRight(windowRect0.left() + scrollPixel + caliperPixel);

                yColorMapRect0 = windowRect0;
                yColorMapRect0.setRight(windowRect0.left() + scrollPixel);

                // B扫
                windowRect1 = windowRect;
                windowRect1.setTop(windowRect.top() + ascan_with_separator_pixel);

                imageRect1 = windowRect1;
                imageRect1.setLeft(windowRect1.left() + caliperPixel + scrollPixel);
                imageRect1.setBottom(windowRect1.bottom() - caliperPixel - scrollPixel);

                xCaliperRect1 = windowRect1;
                xCaliperRect1.setLeft(windowRect1.left() + caliperPixel + scrollPixel);
                xCaliperRect1.setTop(windowRect1.bottom() - caliperPixel - scrollPixel);
                xCaliperRect1.setBottom(windowRect1.bottom() - scrollPixel);

                yCaliperRect1 = windowRect1;
                yCaliperRect1.setLeft(windowRect1.left() + scrollPixel);
                yCaliperRect1.setRight(windowRect1.left() + caliperPixel + scrollPixel);
                yCaliperRect1.setBottom(windowRect1.bottom() - caliperPixel - scrollPixel);

                xScrollRect1 = windowRect1;
                xScrollRect1.setLeft(windowRect1.left() + caliperPixel + scrollPixel);
                xScrollRect1.setTop(windowRect1.bottom() - scrollPixel);

                yScrollRect1 = windowRect1;
                yScrollRect1.setRight(windowRect1.left() + scrollPixel);
                yScrollRect1.setBottom(windowRect1.bottom() - caliperPixel - scrollPixel);

                m_ViewRectangleInfo[ch].WindowRect         = windowRect;
                m_ViewRectangleInfo[ch].AScanRect          = windowRect0;
                m_ViewRectangleInfo[ch].BScanRect          = windowRect1;
                m_ViewRectangleInfo[ch].AScanImageRect     = imageRect0;
                m_ViewRectangleInfo[ch].BScanImageRect     = imageRect1;
                m_ViewRectangleInfo[ch].AScanYCaliperRect  = yCaliperRect0;
                m_ViewRectangleInfo[ch].AScanYColorMapRect = yColorMapRect0;
                m_ViewRectangleInfo[ch].BScanXCaliperRect  = xCaliperRect1;
                m_ViewRectangleInfo[ch].BScanYCaliperRect  = yCaliperRect1;

                m_ViewRectangleInfo[ch].XScrollRect = xScrollRect1;
                m_ViewRectangleInfo[ch].YScrollRect = yScrollRect1;

                i++;
            }
        } else if (m_nDisplayLayoutDirection == Horizontal) {
            // 横项排列

            // A扫像素宽度
            int ascan_pixel = ASCAN_PIXEL_RATIO * displayWidth;
            // B扫像素宽度
            int image_pixel = (displayHeight - SEPARATOR_PIXEL * (channelActiveNum - 1)) / channelActiveNum;
            // A扫像素宽度+分隔符宽度
            int ascan_with_separator_pixel = ascan_pixel + SEPARATOR_PIXEL;
            // B扫像素宽度+分隔符宽度
            int image_with_separator_pixel = image_pixel + SEPARATOR_PIXEL;

            for (int ch = 0, i = 0; ch < CHANNEL_NUM; ch++) {
                if (!m_isChannelActive[ch]) {
                    continue;
                }

                QRect windowRect;
                QRect windowRect0, imageRect0, yCaliperRect0, yColorMapRect0;
                QRect windowRect1, imageRect1, xCaliperRect1, yCaliperRect1, xScrollRect1, yScrollRect1;

                // 整体区域
                windowRect = displayRect;
                windowRect.setTop(displayRect.top() + i * image_with_separator_pixel);
                windowRect.setBottom(displayRect.top() + (i + 1) * image_with_separator_pixel - SEPARATOR_PIXEL);

                // A扫
                windowRect0 = windowRect;
                windowRect0.setRight(windowRect.left() + ascan_pixel);

                imageRect0 = windowRect0;
                imageRect0.setBottom(windowRect0.bottom() - caliperPixel - scrollPixel);

                yCaliperRect0 = windowRect0;
                yCaliperRect0.setTop(windowRect0.bottom() - caliperPixel - scrollPixel);
                yCaliperRect0.setBottom(windowRect0.bottom() - scrollPixel);

                yColorMapRect0 = windowRect0;
                yColorMapRect0.setTop(windowRect0.bottom() - scrollPixel);

                // B扫
                windowRect1 = windowRect;
                windowRect1.setLeft(windowRect.left() + ascan_with_separator_pixel);

                imageRect1 = windowRect1;
                imageRect1.setLeft(windowRect1.left() + caliperPixel + scrollPixel);
                imageRect1.setBottom(windowRect1.bottom() - caliperPixel - scrollPixel);

                yCaliperRect1 = windowRect1;
                yCaliperRect1.setLeft(windowRect1.left() + caliperPixel + scrollPixel);
                yCaliperRect1.setTop(windowRect1.bottom() - caliperPixel - scrollPixel);
                yCaliperRect1.setBottom(windowRect1.bottom() - scrollPixel);

                xCaliperRect1 = windowRect1;
                xCaliperRect1.setLeft(windowRect1.left() + scrollPixel);
                xCaliperRect1.setRight(windowRect1.left() + caliperPixel + scrollPixel);
                xCaliperRect1.setBottom(windowRect1.bottom() - caliperPixel - scrollPixel);

                yScrollRect1 = windowRect1;
                yScrollRect1.setLeft(windowRect1.left() + caliperPixel + scrollPixel);
                yScrollRect1.setTop(windowRect1.bottom() - scrollPixel);

                xScrollRect1 = windowRect1;
                xScrollRect1.setRight(windowRect1.left() + scrollPixel);
                xScrollRect1.setBottom(windowRect1.bottom() - caliperPixel - scrollPixel);

                m_ViewRectangleInfo[ch].WindowRect         = windowRect;
                m_ViewRectangleInfo[ch].AScanRect          = windowRect0;
                m_ViewRectangleInfo[ch].BScanRect          = windowRect1;
                m_ViewRectangleInfo[ch].AScanImageRect     = imageRect0;
                m_ViewRectangleInfo[ch].BScanImageRect     = imageRect1;
                m_ViewRectangleInfo[ch].AScanYCaliperRect  = yCaliperRect0;
                m_ViewRectangleInfo[ch].AScanYColorMapRect = yColorMapRect0;
                m_ViewRectangleInfo[ch].BScanXCaliperRect  = xCaliperRect1;
                m_ViewRectangleInfo[ch].BScanYCaliperRect  = yCaliperRect1;

                m_ViewRectangleInfo[ch].XScrollRect = xScrollRect1;
                m_ViewRectangleInfo[ch].YScrollRect = yScrollRect1;

                i++;
            }
        }

        for (int ch = 0; ch < CHANNEL_NUM; ch++) {
            if (!m_isChannelActive[ch]) {
                continue;
            }

            m_ViewRectangleInfo[ch].DisplayDirection = m_nDisplayLayoutDirection;

            m_ViewRectangleInfo[ch].IsShowCaliper = m_isDisplayCaliper;
            m_ViewRectangleInfo[ch].IsShowScroll  = m_isDisplayScroll;
            m_ViewRectangleInfo[ch].IsShowCross   = m_isDisplayCross;
            m_ViewRectangleInfo[ch].IsShowGate    = m_isDisplayGate;

            if (m_nDisplayLayoutDirection == Vertical) {
                m_ViewRectangleInfo[ch].BScanXCaliperPadding  = Bottom;
                m_ViewRectangleInfo[ch].BScanYCaliperPadding  = Left;
                m_ViewRectangleInfo[ch].AScanYCaliperPadding  = Left;
                m_ViewRectangleInfo[ch].AScanYColorMapPadding = Left;
            } else if (m_nDisplayLayoutDirection == Horizontal) {
                m_ViewRectangleInfo[ch].BScanXCaliperPadding  = Left;
                m_ViewRectangleInfo[ch].BScanYCaliperPadding  = Bottom;
                m_ViewRectangleInfo[ch].AScanYCaliperPadding  = Bottom;
                m_ViewRectangleInfo[ch].AScanYColorMapPadding = Bottom;
            }
        }
    }

    void TofdBridge::Update() {
        float encoder_vel = 0.0;
        if (m_nFileVersion == 1) {
            switch (m_SystemPara.TriggerMode) {
                case 0: {
                    encoder_vel = (float)pow(2.0, (3 - m_SystemPara.Encoder1_Increment)) * m_SystemPara.Encoder1_Resolution;
                    break;
                }
                case 1: {
                    encoder_vel = (float)pow(2.0, (3 - m_SystemPara.Encoder2_Increment)) * m_SystemPara.Encoder2_Resolution;
                    break;
                }
                case 2: {
                    encoder_vel = 1;
                    break;
                }
                default:
                    break;
            }
        } else if (m_nFileVersion == 2) {
            switch (m_SystemPara.TriggerMode) {
                case 0: {
                    encoder_vel = (float)pow(2.0, (3 - m_SystemPara.Encoder1_Increment)) * SCAN_INCREMENT_MIN;
                    break;
                }
                case 1: {
                    encoder_vel = (float)pow(2.0, (3 - m_SystemPara.Encoder2_Increment)) * SCAN_INCREMENT_MIN;
                    break;
                }
                case 2: {
                    encoder_vel = 1;
                    break;
                }
                default:
                    break;
            }
        }

        for (int ch = 0; ch < CHANNEL_NUM; ch++) {
            if (!m_isChannelActive[ch]) {
                continue;
            }

            if (ch < TOFD_CHANNEL_NUM) {
                m_ViewRectangleInfo[ch].VeiwType = TOFD;
            } else {
                m_ViewRectangleInfo[ch].VeiwType = PE;
            }

            m_ViewRectangleInfo[ch].CompensateGain = m_nCompensateGain[ch];

            m_ViewRectangleInfo[ch].EncoderVel = encoder_vel;

            m_ViewRectangleInfo[ch].CenterOffset = QPoint(0, 0);

            m_ViewRectangleInfo[ch].AScanCrossType = Cross;

            if (m_ViewRectangleInfo[ch].VeiwType == TOFD) {
                // tofd
                m_ViewRectangleInfo[ch].BScanCrossType = CrossWithParabola; // 抛物线光标
                m_ViewRectangleInfo[ch].DeathHeader    = CalculateCurveLen(ch);

                m_ViewRectangleInfo[ch].ZRange.Start = -100.0;
                m_ViewRectangleInfo[ch].ZRange.End   = 100.0;
            } else if (m_ViewRectangleInfo[ch].VeiwType == PE) {
                // pe
                m_ViewRectangleInfo[ch].BScanCrossType = Cross; // 十字光标
                m_ViewRectangleInfo[ch].DeathHeader    = 0;

                m_ViewRectangleInfo[ch].ZRange.Start = 0;
                m_ViewRectangleInfo[ch].ZRange.End   = 100.0;
            }

            m_ViewRectangleInfo[ch].XRange.Start = m_ChannelPara[ch].Delay;
            m_ViewRectangleInfo[ch].XRange.End   = m_ChannelPara[ch].Delay + m_ChannelPara[ch].EchoRange;
            m_ViewRectangleInfo[ch].YRange.Start = 0.0f;
            m_ViewRectangleInfo[ch].YRange.End   = encoder_vel * m_iScanLength;

            m_ViewRectangleInfo[ch].XScale = -1.0f;
            m_ViewRectangleInfo[ch].YScale = -1.0f;

            m_ViewRectangleInfo[ch].SoundSpeed = m_ChannelPara[ch].Velocity / 1000.0f;
            m_ViewRectangleInfo[ch].PCS        = m_fAdjustPCS[ch];
            m_ViewRectangleInfo[ch].ZeroTime   = CalculateAdjustZero(ch);
        }
    }

    void TofdBridge::HitCurChannel(QPoint point) {
        for (int i = 0; i < CHANNEL_NUM; i++) {
            if (!m_isChannelActive[i]) {
                continue;
            }

            if (m_ViewRectangleInfo[i].WindowRect.contains(point)) {
                m_nCurChannel = i;
                break;
            }
        }
    }

    void TofdBridge::ParseChannelIndex() {
        int index          = -1;
        m_nCurChannelIndex = index;
        for (int i = 0; i < CHANNEL_NUM; i++) {
            if (m_isChannelActive[i]) {
                index++;
                if (m_nCurChannel == i) {
                    m_nCurChannelIndex = index;
                }
            }
        }
    }

    void TofdBridge::UpdateCrossInfo() {
        m_RedCrossValue  = m_ViewRectangleInfo[m_nCurChannel].RedCrossValue;
        m_BlueCrossValue = m_ViewRectangleInfo[m_nCurChannel].BlueCrossValue;
    }
} // namespace Union::TOFD_MULTI::__9x0
