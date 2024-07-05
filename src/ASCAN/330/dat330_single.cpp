#include "dat330_single.h"
#include "../include_private/common/union_common.hpp"
#include <QList>
#include <QPointF>
#include <array>
#include <math.h>

#pragma warning(disable :4554 4018 4310 4701)

constexpr auto C_SIZE_SHORT = 2;

std::array<QVector<QPointF>, 3> dat330_single::DrawDac() {
    std::array<QVector<QPointF>, 3> ret = {};
    int                             i, j;
    double                          fTemp;
    int                             Number;
    short                           dac_line00[500];

    int Width, Hunit;

    Width = 240;
    Hunit = 24;
    Width = 480; // 480
    CalcuDac();
    GetDacLine();
    if ((m_ch_status >> 2 & 0x1) == 1 || (m_ch_status >> 3 & 0x1) == 1) // 2DAC3AVG  判断有DAC或AVG曲线
    {
        Number = ((m_ch_option & 0x07) % 7); // 曲线条数
        for (i = 1; i <= Number; i++) {
            if ((m_ch_sys >> 4) & 0x01) {
            } else {
                QVector<QPointF> points;
                points.clear();

                float   mdelay = 0;
                QString sdelay = "0";
                sdelay         = sdelay.trimmed();
                if (sdelay.length() > 0) {
                    mdelay = sdelay.toFloat();
                }

                for (j = 0; j < Width; j++) {
                    QString s0           = "0";
                    float   fGain        = s0.toFloat();
                    dac_line00[j]        = Union::CalculateGainOutput(dac_line[j], fGain);
                    dac_points[i - 1][j] = dac_line00[j];

                    if (dac_line00[j] < 205) {
                        points.append(QPointF(mdelay + float(j * (m_range_a / 480)), (dac_line00[j] * 2) * 100.0 / 400.0));
                    }
                }
                if (i == 1) {
                    std::swap(points, ret[0]);
                } else if (i == 2) {
                    std::swap(points, ret[1]);
                } else if (i == 3) {
                    std::swap(points, ret[2]);
                }
            }
            if (m_ch_lineGain[i] != m_ch_lineGain[i - 1]) {
                fTemp       = 2.302585 * (m_ch_lineGain[i - 1] - m_ch_lineGain[i]) / 200.0;
                fTemp       = expf((float)fTemp);
                dac_line[0] = (int)(dac_line[0] / fTemp + 0.5);
                for (j = 4; j < Width + 4; j += 4) {
                    dac_line[j]     = (int)(dac_line[j] / fTemp + 0.5);
                    dac_line[j - 2] = (int)((dac_line[j - 4] + dac_line[j]) / 2.0 + 0.5);
                    dac_line[j - 3] = (int)((dac_line[j - 4] + dac_line[j - 2]) / 2.0 + 0.5);
                    dac_line[j - 1] = (int)((dac_line[j - 2] + dac_line[j]) / 2.0 + 0.5);
                }
            }
        }
    }
    return ret;
}

int dat330_single::CalcuDac() {
    short test_num = 0;                     // 选中波次数
    int   dac_db[10 + 2], dac_dist[10 + 2]; // 在画DAC曲线时增加关尾两点，其值与原头尾相同
    int   i, j;
    int   dist1, dist2;
    u_int number;
    float range, delay, speed;
    int   Width, Hunit;

    Width                         = 240;
    Hunit                         = 24;
    constexpr auto C_SCREEN_WIDTH = 500;
    Width                         = 500; // 500

    if (m_unit > 0) {
        range = m_ch_Range * 25.4 / 100;
        delay = m_ch_Delay; //*25.4/100 ;
        speed = m_ch_Speed * 25.4 / 100;
    } else {
        range = m_ch_Range;
        delay = m_ch_Delay;
        speed = m_ch_Speed;
    }

    test_num = m_dac_num; // 选中波次数
    if (test_num < 1 || test_num > 10)
        return -1;

    for (i = 0; i < 10; i++) {
        dac_db[i]   = m_dac_db[i];
        dac_dist[i] = m_dac_dist[i];
    }

    if (m_unit > 0) {                                // 单位inch
        number = m_ch_Crystal_l * 25.4 / 1000 + 0.5; // 晶片尺寸的低8位，如为0则是直探头，用直径计算
        if (number == 0)
            number = m_ch_Crystal_w * 25.4 / 1000 + 0.5;

        if (m_ch_Crystal_w == 0)
            number *= m_ch_Crystal_l * 25.4 / 1000 + 0.5;
        else
            number *= m_ch_Crystal_w * 25.4 / 1000 + 0.5;
        if (number == 0)
            number = 25;
        number = (100 * number * m_ch_Frequence) / (4 * m_ch_Speed * 254 / 1000.0 + 0.5) + 0.5;
    } else {
        number = m_ch_Crystal_l / 1000; // 晶片尺寸的低8位，如为0则是直探头，用直径计算
        if (number == 0)
            number = m_ch_Crystal_w / 1000;

        if (m_ch_Crystal_w == 0)
            number *= m_ch_Crystal_l / 1000;
        else
            number *= m_ch_Crystal_w / 1000;
        if (number == 0)
            number = 25;
        number = (100 * number * m_ch_Frequence) / (4 * m_ch_Speed + 0.5) + 0.5;
    }

    if (m_ch_status >> 4 & 0x1 == 1 || m_ch_status >> 3 & 0x1 == 1) { // AVG

        if (dac_dist[0] <= 3 * number) {   // 第一点在三倍近场区内
            for (i = test_num; i > 0; i--) // 所点向后移一点,增加头
            {
                dac_dist[i] = dac_dist[i - 1];
                dac_db[i]   = dac_db[i - 1];
            }
        } else {                               // 第一点在三倍近场区外
            for (i = test_num + 1; i > 1; i--) // 所有点向后移两点,增加头
            {
                dac_dist[i] = dac_dist[i - 2];
                dac_db[i]   = dac_db[i - 2];
            }
            test_num++;
            if (3 * number < dac_dist[2] / 5)
                dac_dist[1] = dac_dist[2] / 5;
            else
                dac_dist[1] = 3 * number; // 第2点为3倍近场区
            dac_db[1] = 400 * (log10(float(dac_dist[2])) - log10(float(dac_dist[1]))) + 2 * 0 * (dac_dist[2] - dac_dist[1]);
            if (dac_db[1] >= dac_db[2])
                dac_db[1] = 0;
            else
                dac_db[1] = dac_db[2] - dac_db[1];
            dac_dist[0] = 0;
            dac_db[0]   = dac_db[1];
        }

        dac_dist[0] = 0; // 增加的头距离为0，dB数与原第一点相同
        test_num++;
        dac_dist[test_num] = range + delay * speed / (C_SAMPLE_FREQ); // MGetRange(3) + MGetDelay(3);		//最后一点为当前最大声程
        if (dac_dist[test_num] >= 3 * dac_dist[test_num - 1]) {       // 最后一个测试点距屏幕右端非常大，增加一点
            test_num++;
            dac_dist[test_num]     = dac_dist[test_num - 1];
            dac_dist[test_num - 1] = 2 * dac_dist[test_num - 2];
            dac_db[test_num - 1]   = dac_db[test_num - 2] + 120 + 2 * 0 * dac_dist[test_num - 2];
        }
        dac_db[test_num] = dac_db[test_num - 1] + 400 * (log10(float(dac_dist[test_num])) - log10(float(dac_dist[test_num - 1]))) + 2 * 0 * (dac_dist[test_num] - dac_dist[test_num - 1]);
        test_num++; // 增加一头一尾共两点
    } else {        // DAC

        for (i = test_num; i > 0; i--) // 所点向后移一点,增加头
        {
            dac_dist[i] = dac_dist[i - 1];
            dac_db[i]   = dac_db[i - 1];
        }
        dac_dist[0] = 0; // 增加的头距离为0，dB数与原第一点相同
        test_num++;
        dac_dist[test_num] = range + delay * speed / (C_SAMPLE_FREQ); // MGetRange(3) + MGetDelay(3);		//最后一点为当前最大声程
        dac_db[test_num]   = dac_db[test_num - 1];
        test_num++; // 增加一头一尾共两点
    }

    for (i = 1; i < test_num; i++) {                         // 对延时进行处理
        if (delay * speed / (C_SAMPLE_FREQ) < dac_dist[i]) { // 有延时，延掉i-1个点
            dac_db[i - 1]   = GetMiddleValue(dac_dist[i - 1], dac_db[i - 1], dac_dist[i], dac_db[i], delay * speed / (C_SAMPLE_FREQ));
            dac_dist[i - 1] = delay * speed / (C_SAMPLE_FREQ);
            test_num        = test_num - (i - 1); // 测试点减少i-1个点
            for (j = 0; j < test_num; j++) {      // 将延时点前去除
                dac_dist[j] = dac_dist[j + i - 1];
                dac_db[j]   = dac_db[j + i - 1];
            }
            break;
        }
    }
    for (i = 1; i < test_num; i++) {
        if (dac_dist[i] > range + delay * speed / (C_SAMPLE_FREQ)) // MGetDelay(3)+MGetRange(3) )
        {
            dac_db[i]   = GetMiddleValue(dac_dist[i - 1], dac_db[i - 1], dac_dist[i], dac_db[i], range + delay * speed / (C_SAMPLE_FREQ));
            dac_dist[i] = range + delay * speed / (C_SAMPLE_FREQ);
            test_num    = i + 1;
            break;
        }
    }
    // 限位
    for (i = 0; i < test_num; i++) {
        if (dac_db[i] > 65535)
            dac_db[i] = 65535;
        else if (dac_db[i] < 0)
            dac_db[i] = 0;
    }
    dist2           = 0;
    dac_line[dist2] = dac_db[0];
    i               = range;
    for (i = 1, j = 0; i < test_num && dist2 < C_SCREEN_WIDTH - 1; i++) { // 将距离值化为屏幕上的点
        dist1 = dist2;
        dist2 = (dac_dist[i] - delay * speed / (C_SAMPLE_FREQ)) * (Width) / range;

        if (dist2 >= C_SCREEN_WIDTH + 1) {
            dac_line[C_SCREEN_WIDTH - 1] = GetMiddleValue(dist1, dac_line[i - 1], dist2, dac_db[i], C_SCREEN_WIDTH - 1);
            dist2                        = C_SCREEN_WIDTH - 1;
        } else {
            dac_line[dist2] = dac_db[i];
            Last_Test_Dist  = dist2;
        }
        GetLine_TwoDot(dac_line, dist1, dist2);
    }

    return 2;
}

void dat330_single::GetDacLine() {
    int            basegain;
    int            i;
    int            dist1, dist2;
    int            step           = 4;
    int            retvalue       = -1;
    constexpr auto C_SCREEN_WIDTH = 500;
#define C_EXPF 2.302585
    u_char pzText[2] = " ";

    char dactable[C_SCREEN_WIDTH * 2];
    int  Width, Hunit;
    Width = 240;
    Hunit = 24;
    Width = C_SCREEN_WIDTH; // 500

    pzText[0] = 'z' + 14;

    retvalue = 0;
    if ((m_sys >> 25 & 1) && ((m_ch_status >> C_TEST_DAC) & 0x1 == 0) && ((m_ch_status >> C_TEST_AVG) & 0x1 == 0)) { // MGetSystemMode() == 0 && MGetTestStatus(C_TEST_DAC)== 0 && MGetTestStatus(C_TEST_AVG)
        retvalue = -1;                                                                                               // 不在自动调节状态，也未制作DAC\AVG,无射频
    }

    {
        for (i = 0; i < C_COORWIDTH; i++) {
            dactable[i] = (char)160;
        }
    }

    if (((m_sys >> 25) & 1) && (((m_ch_status >> C_TEST_DAC) & 0x1) == 0) && (((m_ch_status >> C_TEST_AVG) & 0x01) == 0)) // MGetSystemMode() == 1 && (MGetTestStatus(C_TEST_DAC) == 0 && MGetTestStatus(C_TEST_AVG) == 0)
    {
        basegain = (int)(m_ch_BaseGain + 481.31); // 基准增益
    } else {
        basegain = (int)(m_ch_BaseGain + m_ch_lineGain[0] + 481.31); // 基准增益，应加上判废线
    }
    dist2           = 0;
    dac_line[dist2] = (u_int)(Mexpf((float)(C_EXPF * (basegain - dac_line[dist2]) / 200.0), C_SIZE_SHORT));
    if (((m_ch_sys >> 4) & 0x01) == 1) {
        dist1           = dist2;
        dist2           = Width - 1;
        dac_line[dist2] = dac_line[dist1];
        GetLine_TwoDot(dac_line, dist1, dist2);
    } else {
        while (dist2 < C_SCREEN_WIDTH) {
            dist1 = dist2;
            dist2 += step;
            dac_line[dist2] = (u_int)(expf((float)(C_EXPF * (basegain - dac_line[dist2]) / 200.0)));
            GetLine_TwoDot(dac_line, dist1, dist2);
        }
    }

    for (int j = Last_Test_Dist + 1; j < C_SCREEN_WIDTH - 1; j++) {
        dac_line[j + 1] = dac_line[Last_Test_Dist];
    }

    if (m_ch_gatedB != m_ch_lineGain[0]) {
        double fTemp = 2.302585 * (m_ch_lineGain[0] - m_ch_gatedB) / 200.0;
        fTemp        = exp((float)fTemp);
        if (fTemp > 0) {
            dac_gate[0] = (int)(dac_line[0] / fTemp + 0.5);

            for (int j = 4; j < ECHO_PACKAGE_SIZE + 4; j += 4) {
                dac_gate[j]     = (int)(dac_line[j] / fTemp + 0.5);
                dac_gate[j - 2] = (int)((dac_gate[j - 4] + dac_gate[j]) / 2.0 + 0.5);
                dac_gate[j - 3] = (int)((dac_gate[j - 4] + dac_gate[j - 2]) / 2.0 + 0.5);
                dac_gate[j - 1] = (int)((dac_gate[j - 2] + dac_gate[j]) / 2.0 + 0.5);
            }
        }
    }
}

float dat330_single::Mexpf(float ex, u_int len) // 指数函数，得到e^ex,位数不大于len×8
{
    float  retvalue;
    int    nMax = 1;
    double fMax;
    if (len <= 1) {
        len  = 1;
        fMax = 4.844187086; // ln(127)
    } else if (len == 2) {
        len  = 2;
        fMax = 10.39717719; // ln(32767
    } else if (len > 2 && len <= 4) {
        len  = 4;
        fMax = 10.39717719 * 2;
    } else if (len > 4) {
        len  = 8;
        fMax = 10.39717719 * 4;
    }
    if (ex > fMax)
        ex = (float)fMax;
    nMax = (nMax << (len * 8)) - 1;

    retvalue = expf(ex);

    return retvalue;
}

int dat330_single::GetMiddleValue(int dist1, int db1, int dist2, int db2, int dist) { // 由两点得中间点的当量
    return Union::ValueMap(dist, {(double)db1, (double)db2}, {(double)dist1, (double)dist2});
}

int dat330_single::GetLine_TwoDot(short line[], int dist1, int dist2) {
    // FUNC: 实现line[dist1]~line[dist2]之见数据的线性插值
    int    dist;
    double k;
    if (dist1 > dist2 + 1) {
        dist  = dist1;
        dist1 = dist2;
        dist2 = dist;
    } else if (dist1 >= dist2 - 1)
        return -1;

    k = (double)(line[dist2] - line[dist1]) / (double)(dist2 - dist1);
    for (dist = 1; dist1 + dist < dist2; dist++) {
        line[dist1 + dist] = (int)(dist * k + line[dist1]);
    }
    return 1;
}
