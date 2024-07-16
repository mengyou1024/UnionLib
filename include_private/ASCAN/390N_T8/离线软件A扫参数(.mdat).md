

### A扫数据格式

文件内容: **0x556ee655(文件类型标志，小端序) + [仪器基本信息帧](# 仪器基本信息) **

**+ ([A扫数据帧](# A扫数据) + [通道参数帧](# 通道参数]) + option([DAC帧](# DAC)) + option([AVG帧](# AVG)) + option([五大性能帧](# 五大性能)) + option([摄像头数据帧](# 摄像头数据)))[0]**

...

**+([A扫数据帧](# A扫数据) + [通道参数帧](# 通道参数]) + option([DAC帧](# DAC)) + option([AVG帧](# AVG)) + option([五大性能帧](# 五大性能)) + option([摄像头数据帧](# 摄像头数据)))[N]**

> 注：
>
> 1. 带option为**可空帧**， 该类参数**未使用时可以不传**
> 2. N表示连续图像的最大索引序号，单幅图像中索引最大值为0
> 

### 帧结构

`帧头:0x55(uin8_t)`+`class type(uint16_t)`+`payload length(uint32_t)`+`payload(bytes)`+`帧尾:0x6e(uint8_t)`

> 1. class type: 帧的类类型
> 2. payload length: 负载长度, 即payload区块所占的字节数
> 3. payload: 有效负载

### 帧类类型表

| 类型名             | class type |
| ------------------ | ---------- |
| [仪器基本信息](# 仪器基本信息) | 0          |
| [通道参数](# 通道参数)           | 1          |
| [DAC](# DAC)                | 2          |
| [AVG](# AVG)               | 3          |
| [五大性能](# 五大性能)           | 4          |
| [摄像头数据](# 摄像头数据)         | 5          |
| [A扫数据](# A扫数据) | 6 |

#### 兼容性新增

| 类型名             | class type | 说明 |
| ------------------ | ---------- | ------------------ |
| [CMP000](# CMP000) | 0x8000 | 超过3根的DAC曲线、B波门类型(进波门、失波门) |

> 注: 
>
> 1. 所有的多字节类型均保持**小端序**
> 2. 所有的帧负载均**紧凑排布(单字节对齐)**

#### 仪器基本信息

| 字段名           | 类型    | 注释                         |
| ---------------- | ------- | ---------------------------- |
| instrumentName   | uint8_t | 0: PXUT-390N<br />1: PXUT-T8 |
| version| uint32_t| 版本号: v[31:24].[23:16].[15:0] |
| recordTimeLength | uint8_t | recordTime字段的字节数       |
| recordTime       | char[]  | 记录开始时间                 |

#### 通道参数

| 字段名                | 类型      | 注释                                                         |
| --------------------- | --------- | ------------------------------------------------------------ |
| axisBias              | float     | X轴坐标的起始偏移(即延时) 单位: mm                           |
| axisLen              | float     | X轴坐标的长度(即声程) 单位: mm                           |
| baseGain              | float     | 基础增益                                                     |
| scanGain              | float     | 扫查增益                                                     |
| compensatingGain      | float     | 表面补偿增益                                                 |
| suppression           | float     | 抑制                                                         |
| distanceMode          | uint8_t   | 0: Y<br />1: X<br />2: S                                     |
| channel               | uint8_t   | 通道号                                                       |
| zeroPoint             | float     | 零点 单位: μs                                                |
| probeFrontDistance    | float     | 探头前沿 单位: mm                                            |
| soundVelocity         | float     | 声速 单位: mm                                                |
| angle                 | float     | 探头角度 单位: °                                             |
| probeChipShapeWorD    | float     | 探头晶片尺寸的宽度或者直径 单位: mm                          |
| probeChipShapeLorZero | float     | 探头晶片尺寸的长度 单位: mm (仅有直径参数的探头填0)         |
| probe                 | uint8_t   | 0: 直探头<br />1: 斜探头<br />2: 双晶纵波直探头<br />3: 穿透探头<br />4: 双晶纵波斜探头 |
| gateAPos              | float     | A波门位置(归一化)                                            |
| gateAWidth            | float     | A波门宽度(归一化)                                            |
| gateAHeight           | float     | A波门高度(归一化)                                            |
| gateAEnable           | uint8_t     | A波门使能                                           |
| gateBPos              | float     | B波门位置(归一化)                                            |
| gateBWidth            | float     | B波门宽度(归一化)                                            |
| gateBHeight           | float     | B波门高度(归一化)                                            |
| gateBEnable           | uint8_t     | B波门使能                                           |
| probeFrequency | float | 探头频率 单位: MHz |
| samplingDelay | float | 采样延时 单位: μs |

#### DAC

| 字段名            | 类型    | 注释                                                       |
| ----------------- | ------- | ---------------------------------------------------------- |
|   isReady       |  uint8_t  | 是否使用DAC                                                    |
| baseGain          | float   | 基本增益                                                   |
| compensatingGain  | float   | 补偿增益                                                   |
| samplingNumber    | uint8_t | 采样数量 index和value的个数                                |
| index             | float[] | dac采样的index                                             |
| value             | float[] | dac采样的value                                             |
| equivalent        | float   | dac当量                                                    |
| criteria          | uint8_t | dac判定标准<br />0: ""<br />1:"RL"<br />2:"SL"<br />3:"EL" |
| criteriaBiasRL    | float   | RL 偏移                                                    |
| criteriaBiasSL    | float   | SL 偏移                                                    |
| criteriaBiasEL    | float   | EL 偏移                                                    |
| onlyShowBaseLine  | uint8_t | 当为1时只显示基线                                          |
| samplingXAxisBias | float   | 采样时的X轴的起始偏移(即延时) 单位: mm                     |
| samplingXAxisLen | float   | 采样时的X轴的长度(即声程) 单位: mm                         |


#### AVG

| 字段名            | 类型    | 注释                                   |
| ----------------- | ------- | -------------------------------------- |
|   isReady       |  uint8_t  | 是否使用AVG |
| baseGain          | float   | 基础增益                               |
| compensatingGain  | float   | 补偿增益                               |
| scanGain          | float   | 扫查增益                               |
| samplingNumber    | uint8_t | 采样数量 index和value的个数            |
| index             | float[] | 采样的index                            |
| value             | float[] | 采样的value                            |
| onlyShowBaseLine  | uint8_t | 当为1时只显示基线                      |
| samplingXAxisBias | float   | 采样时的X轴的起始偏移(即延时) 单位: mm |
| samplingXAxisLen | float   | 采样时的X轴的长度(即声程) 单位: mm     |
| diameter          | float   | AVG孔径 单位: mm                       |
| reflectorDiameter | float   | 反射体直径 单位: mm                    |
| reflectorMaxDepth | float   | 发射体最大深度                         |
| equivalent        | float   | 当量                                   |

#### 五大性能

| 字段名              | 类型  | 注释       |
| ------------------- | ----- | ---------- |
| horizontalLinearity | float | 水平线性   |
| verticalLinearity   | float | 垂直线性   |
| resolution          | float | 分辨力     |
| dynamicRange        | float | 动态范围   |
| surplusSensitivity  | float | 灵敏度余量 |

#### 摄像头数据

| 字段名     | 类型      | 注释                     |
| ---------- | --------- | ------------------------ |
| width      | uint16_t  | 图像宽度                 |
| height     | uint16_t  | 图像高度                 |
| imgFormat  | uint8_t   | 图像格式<br />0: raw_rgb |
| dataLength | uint32_t  | data字段的字节数         |
| data       | uint8_t[] | 图像数据                 |

#### A扫数据

| 字段名 | 类型      | 注释    |
| ------ | --------- | ------- |
| data   | uint8_t[] | A扫数据 |



### 兼容性新增类型

#### CMP000

| 字段名            | 类型    | 注释                         |
| ----------------- | ------- | ---------------------------- |
| gateBType         | uint8_t | B门类型(0: 失波门 1: 进波门) |
| criteriaBiasLine4 | float   | Line4 偏移                   |
| criteriaBiasLine5 | float   | Line5 偏移                   |
| criteriaBiasLine6 | float   | Line6 偏移                   |
| LineNumber        | uint8_t | DAC曲线根数                  |
