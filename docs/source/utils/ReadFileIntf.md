# 读取接口

## 快速创建类继承接口
```python
python utils/utils.py create <INTF> <sub_name> <class_name>
```
- <INTF>: 接口名称: ASCAN、TOFD_PE
- <sub_name>: 子目录名称: 通常为仪器名称
- <class_name>: 类名称
### 示例
``` python
python utils/utils.py create ASCAN 330 network_port
```

## ASCAN
### 派生类
目前该类的派生类有: `Union::__390N_T8::T8_390N_JSON`、`Union::__390N_T8::LDAT`、
                `Union::__330::DASType`、`Union::__330::DATType`、`Union::__330::Serial_330`

### 虚基类定义

```{doxygenclass} Union::AScan::AScanIntf
:project: UnionLib
:members:
```

## TOFD_PE
### 派生类
目前该类的派生类有: `Union::TOFD_PE::TPE`

### 虚基类定义
```{doxygenclass} Union::TOFD_PE::TofdPeIntf
:project: UnionLib
:members:
```
