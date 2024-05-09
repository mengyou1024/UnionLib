#pragma once
#include <cstdint>
namespace Union::Temp::Unresovled {
    // 这部分功能是从原来的程序里面移植过来的, 需要优化
    void __decode(uint8_t *inbuf, int inlen, uint8_t *outbuf, int *outlen);
}
