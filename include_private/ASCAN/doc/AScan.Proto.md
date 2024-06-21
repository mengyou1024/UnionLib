# Protocol Documentation
<a name="top"></a>

## Table of Contents

- [AScan.proto](#AScan-proto)
    - [AScanHead](#Union-AScan-Proto-AScanHead)
    - [AScanPayload](#Union-AScan-Proto-AScanPayload)
    - [AVG](#Union-AScan-Proto-AVG)
    - [ChannelParameter](#Union-AScan-Proto-ChannelParameter)
    - [DAC](#Union-AScan-Proto-DAC)
    - [DACCriteriaBias](#Union-AScan-Proto-DACCriteriaBias)
    - [Gate](#Union-AScan-Proto-Gate)
    - [InstrumentParameter](#Union-AScan-Proto-InstrumentParameter)
    - [Performance](#Union-AScan-Proto-Performance)
  
    - [DACCriteria](#Union-AScan-Proto-DACCriteria)
    - [DistanceMode](#Union-AScan-Proto-DistanceMode)
    - [Probe](#Union-AScan-Proto-Probe)
  
- [Scalar Value Types](#scalar-value-types)



<a name="AScan-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## AScan.proto



<a name="Union-AScan-Proto-AScanHead"></a>

### AScanHead
A扫数据头部信息(不区分单幅和连续)


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| instrumentName | [string](#string) |  | 仪器名称 |
| time | [string](#string) |  | 保存文件的时间和日期(格式:`yyyy-M-d H:m:s`) |






<a name="Union-AScan-Proto-AScanPayload"></a>

### AScanPayload
A扫数据有效负载
这块区域保存在整个存储过程中**可能发生变化**的值


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| instrumentParameter | [InstrumentParameter](#Union-AScan-Proto-InstrumentParameter) |  | 仪器参数 |
| channelParameter | [ChannelParameter](#Union-AScan-Proto-ChannelParameter) |  | 通道参数 |
| cameraData | [bytes](#bytes) | optional | 摄像头数据 |






<a name="Union-AScan-Proto-AVG"></a>

### AVG
AVG参数
DAC显示为 &#34;Φ&lt;diameter&gt;  Φ${reflectorDiameter}&#43;${equivalent}dB&#34;


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| baseGain | [float](#float) |  | 基础增益 |
| compensatingGain | [float](#float) |  | 补偿增益 |
| scanGain | [float](#float) |  | 扫查增益 |
| index | [float](#float) | repeated | 采样的index |
| value | [float](#float) | repeated | 采样的value |
| isSubLine | [bool](#bool) |  | 当为真时只显示基线 |
| samplingXAxisBias | [float](#float) |  | AVG采样时X轴的起始偏移 |
| samplingXAxisLen | [float](#float) |  | AVG采样时X轴的长度(即声程) |
| diameter | [float](#float) |  | avg 孔径: |
| reflectorDiameter | [float](#float) |  | 反射体直径 |
| reflectorMaxDepth | [float](#float) |  | 反射体最大深度 |
| equivalent | [float](#float) |  | 当量 |






<a name="Union-AScan-Proto-ChannelParameter"></a>

### ChannelParameter
通道参数


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| gate | [Gate](#Union-AScan-Proto-Gate) | repeated | 波门 |
| ascan | [bytes](#bytes) |  | A扫数据 |
| axisBias | [float](#float) |  | X坐标轴的起始偏移(即延时) |
| axisLen | [float](#float) |  | X坐标轴的长度(即声程) |
| baseGain | [float](#float) |  | 基础增益 |
| scanGain | [float](#float) |  | 扫查增益(扫查增益) |
| compensatingGain | [float](#float) |  | 表面补偿增益 |
| suppression | [int32](#int32) |  | 抑制 |
| distanceMode | [DistanceMode](#Union-AScan-Proto-DistanceMode) |  | 距离模式 |
| dac | [DAC](#Union-AScan-Proto-DAC) | optional | DAC参数 |
| avg | [AVG](#Union-AScan-Proto-AVG) | optional | AVG参数 |
| performance | [Performance](#Union-AScan-Proto-Performance) | optional | 五大性能 |






<a name="Union-AScan-Proto-DAC"></a>

### DAC
DAC 参数
DAC显示为 &#34;${criteria} &#43;${equivalent}dB&#34;


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| baseGain | [float](#float) |  | 基础增益 |
| compensatingGain | [float](#float) |  | 补偿增益 |
| index | [float](#float) | repeated | 采样的index |
| value | [float](#float) | repeated | 采样的value |
| equivalent | [float](#float) |  | DAC 当量 |
| criteria | [DACCriteria](#Union-AScan-Proto-DACCriteria) |  | DAC 判定标准 |
| criteriaBias | [DACCriteriaBias](#Union-AScan-Proto-DACCriteriaBias) | optional | DAC 判定标准偏移 |
| isSubLine | [bool](#bool) |  | 当为真时只显示基线 |
| samplingXAxisBias | [float](#float) |  | DAC采样时X轴的起始偏移 |
| samplingXAxisLen | [float](#float) |  | DAC采样时X轴的长度(即声程) |






<a name="Union-AScan-Proto-DACCriteriaBias"></a>

### DACCriteriaBias
DAC 判定标准偏移


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| rlBias | [sint32](#sint32) |  | RL 偏移 默认 -4.0 |
| slBias | [sint32](#sint32) |  | SL 偏移 默认 -12.0 |
| elBias | [sint32](#sint32) |  | EL 偏移 默认 -18.0 |






<a name="Union-AScan-Proto-Gate"></a>

### Gate
波门


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| idx | [int32](#int32) |  | 波门序号 |
| pos | [float](#float) |  | 波门位置(归一化) |
| width | [float](#float) |  | 波门宽度(归一化) |
| height | [float](#float) |  | 波门高度(归一化) |






<a name="Union-AScan-Proto-InstrumentParameter"></a>

### InstrumentParameter
仪器参数(系统参数)


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| probe | [Probe](#Union-AScan-Proto-Probe) |  | 探头 |
| probeFrequence | [float](#float) |  | 探头频率 单位MHz |
| probeChipShapeL | [float](#float) |  | 探头晶片尺寸低位 |
| probeChipShapeH | [float](#float) |  | 探头晶片尺寸高位 |
| angle | [float](#float) |  | 探头角度 |
| soundVelocity | [float](#float) |  | 声速 |
| probeFrontDistance | [float](#float) |  | 探头前沿 |
| zeroPoint | [float](#float) |  | 零点 单位(μs) |
| channel | [int32](#int32) |  | 通道号 |
| performance | [Performance](#Union-AScan-Proto-Performance) | optional | 五大性能 |






<a name="Union-AScan-Proto-Performance"></a>

### Performance
仪器五大性能


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| horizontalLinearity | [float](#float) |  | 水平线性 |
| verticalLinearity | [float](#float) |  | 垂直线性 |
| resolution | [float](#float) |  | 分辨力 |
| dynamicRange | [float](#float) |  | 动态范围 |
| surplusSensitivity | [float](#float) |  | 灵敏度余量 |





 


<a name="Union-AScan-Proto-DACCriteria"></a>

### DACCriteria
DAC 判定标准 当前前面的值

| Name | Number | Description |
| ---- | ------ | ----------- |
| None | 0 | &#34;&#34; |
| RL | 1 | &#34;RL&#34; |
| SL | 2 | &#34;SL&#34; |
| EL | 3 | &#34;EL&#34; |



<a name="Union-AScan-Proto-DistanceMode"></a>

### DistanceMode
距离模式

| Name | Number | Description |
| ---- | ------ | ----------- |
| Y | 0 |  |
| X | 1 |  |
| S | 2 |  |



<a name="Union-AScan-Proto-Probe"></a>

### Probe
探头

| Name | Number | Description |
| ---- | ------ | ----------- |
| NormalProbe | 0 | 直探头 |
| ObliqueProbe | 1 | 斜探头 |
| TwinCrystalLongitudinalWaveNormalprobe | 2 | 双晶纵波直探头 |
| PenetrationProbe | 3 | 穿透探头 |
| TwinCrystaLongitudinalWaveObliqueProbe | 4 | 双晶纵波斜探头 |


 

 

 



## Scalar Value Types

| .proto Type | Notes | C++ | Java | Python | Go | C# | PHP | Ruby |
| ----------- | ----- | --- | ---- | ------ | -- | -- | --- | ---- |
| <a name="double" /> double |  | double | double | float | float64 | double | float | Float |
| <a name="float" /> float |  | float | float | float | float32 | float | float | Float |
| <a name="int32" /> int32 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint32 instead. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="int64" /> int64 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint64 instead. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="uint32" /> uint32 | Uses variable-length encoding. | uint32 | int | int/long | uint32 | uint | integer | Bignum or Fixnum (as required) |
| <a name="uint64" /> uint64 | Uses variable-length encoding. | uint64 | long | int/long | uint64 | ulong | integer/string | Bignum or Fixnum (as required) |
| <a name="sint32" /> sint32 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int32s. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="sint64" /> sint64 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int64s. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="fixed32" /> fixed32 | Always four bytes. More efficient than uint32 if values are often greater than 2^28. | uint32 | int | int | uint32 | uint | integer | Bignum or Fixnum (as required) |
| <a name="fixed64" /> fixed64 | Always eight bytes. More efficient than uint64 if values are often greater than 2^56. | uint64 | long | int/long | uint64 | ulong | integer/string | Bignum |
| <a name="sfixed32" /> sfixed32 | Always four bytes. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="sfixed64" /> sfixed64 | Always eight bytes. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="bool" /> bool |  | bool | boolean | boolean | bool | bool | boolean | TrueClass/FalseClass |
| <a name="string" /> string | A string must always contain UTF-8 encoded or 7-bit ASCII text. | string | String | str/unicode | string | string | string | String (UTF-8) |
| <a name="bytes" /> bytes | May contain any arbitrary sequence of bytes. | string | ByteString | str | []byte | ByteString | string | String (ASCII-8BIT) |

