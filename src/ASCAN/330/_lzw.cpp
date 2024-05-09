#include "_lzw.h"

#pragma warning(disable :4101 4554 4244)

//------------------------------------------------------------------------------
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
//------------------------------------------------------------------------------
#define LZW_BASE   0x102 // The code base
#define CODE_LEN   12    // Max code length
#define TABLE_LEN  4099  // It must be prime number and bigger than 2^CODE_LEN=4096.
                         // Such as 5051 is also ok.
#define BUFFERSIZE 1024
//------------------------------------------------------------------------------
typedef struct
{
    HANDLE h_sour; // Source file handle.
    HANDLE h_dest; // Destination file handle.

    HANDLE h_suffix; // Suffix table handle.
    HANDLE h_prefix; // Prefix table handle.
    HANDLE h_code;   // Code table handle.

    LPWORD lp_prefix; // Prefix table head pointer.
    LPBYTE lp_suffix; // Suffix table head pointer.
    LPWORD lp_code;   // Code table head pointer.

    WORD code;
    WORD prefix;
    BYTE suffix;

    BYTE cur_code_len; // Current code length.[ used in Dynamic-Code-Length mode ]

} LZW_DATA_VC, *PLZW_DATA_VC;

typedef struct
{
    WORD top;
    WORD index;

    LPBYTE lp_buffer;
    HANDLE h_buffer;

    BYTE  by_left;
    DWORD dw_buffer;

    BOOL end_flag;

} BUFFER_DATA_VC, *PBUFFER_DATA_VC;

typedef struct // Stack used in decode
{
    WORD   index;
    HANDLE h_stack;
    LPBYTE lp_stack;

} STACK_DATA_VC, *PSTACK_DATA_VC;
//------------------------------------------------------------------------------
VOID stack_create(PSTACK_DATA_VC stack) {
    stack->h_stack  = GlobalAlloc(GHND, TABLE_LEN * sizeof(BYTE));
    stack->lp_stack = (LPBYTE)GlobalLock(stack->h_stack);
    stack->index    = 0;
}
//------------------------------------------------------------------------------
VOID stack_destory(PSTACK_DATA_VC stack) {
    GlobalUnlock(stack->h_stack);
    GlobalFree(stack->h_stack);
}
//------------------------------------------------------------------------------
VOID buffer_create(PBUFFER_DATA_VC buffer) {
    buffer->h_buffer  = GlobalAlloc(GHND, BUFFERSIZE * sizeof(BYTE));
    buffer->lp_buffer = (LPBYTE)GlobalLock(buffer->h_buffer);
    buffer->top       = 0;
    buffer->index     = 0;
    buffer->by_left   = 0;
    buffer->dw_buffer = 0;
    buffer->end_flag  = FALSE;
}
//------------------------------------------------------------------------------
VOID buffer_destory(PBUFFER_DATA_VC buffer) {
    GlobalUnlock(buffer->h_buffer);
    GlobalFree(buffer->h_buffer);
}
//------------------------------------------------------------------------------
VOID re_init_lzw(PLZW_DATA_VC lzw) // When code table reached its top it should
{                                  // be reinitialized.
    memset(lzw->lp_code, 0xFFFF, TABLE_LEN * sizeof(WORD));
    lzw->code         = LZW_BASE;
    lzw->cur_code_len = 9;
}
//------------------------------------------------------------------------------
VOID lzw_create(PLZW_DATA_VC lzw, HANDLE h_sour, HANDLE h_dest) {
    lzw->h_code       = GlobalAlloc(GHND, TABLE_LEN * sizeof(WORD));
    lzw->h_prefix     = GlobalAlloc(GHND, TABLE_LEN * sizeof(WORD));
    lzw->h_suffix     = GlobalAlloc(GHND, TABLE_LEN * sizeof(BYTE));
    lzw->lp_code      = (unsigned short *)GlobalLock(lzw->h_code);
    lzw->lp_prefix    = (unsigned short *)GlobalLock(lzw->h_prefix);
    lzw->lp_suffix    = (LPBYTE)GlobalLock(lzw->h_suffix);
    lzw->code         = LZW_BASE;
    lzw->cur_code_len = 9;
    lzw->h_sour       = h_sour;
    lzw->h_dest       = h_dest;
    memset(lzw->lp_code, 0xFFFF, TABLE_LEN * sizeof(WORD));
}
//------------------------------------------------------------------------------
VOID lzw_destory(PLZW_DATA_VC lzw) {
    GlobalUnlock(lzw->h_code);
    GlobalUnlock(lzw->h_prefix);
    GlobalUnlock(lzw->h_suffix);

    GlobalFree(lzw->h_code);
    GlobalFree(lzw->h_prefix);
    GlobalFree(lzw->h_suffix);
}
//------------------------------------------------------------------------------

#define DIV      TABLE_LEN
#define HASHSTEP 13 // It should bigger than 0.
//------------------------------------------------------------------------------
WORD get_hash_index(PLZW_DATA_VC lzw) {
    DWORD tmp;
    WORD  result;
    DWORD prefix;
    DWORD suffix;
    prefix = lzw->prefix;
    suffix = lzw->suffix;
    tmp    = prefix << 8 | suffix;
    result = tmp % DIV;
    return result;
}
//------------------------------------------------------------------------------
WORD re_hash_index(WORD hash) // If hash conflict occured we must recalculate
{                             // hash index .
    WORD result;
    result = hash + HASHSTEP;
    result = result % DIV;
    return result;
}
//------------------------------------------------------------------------------
BOOL in_table(PLZW_DATA_VC lzw) // To find whether current code is already in table.
{
    BOOL result;
    WORD hash;

    hash = get_hash_index(lzw);
    if (lzw->lp_code[hash] == 0xFFFF) {
        result = FALSE;
    } else {
        if (lzw->lp_prefix[hash] == lzw->prefix &&
            lzw->lp_suffix[hash] == lzw->suffix) {
            result = TRUE;
        } else {
            result = FALSE;
            while (lzw->lp_code[hash] != 0xFFFF) {
                if (lzw->lp_prefix[hash] == lzw->prefix &&
                    lzw->lp_suffix[hash] == lzw->suffix) {
                    result = TRUE;
                    break;
                }
                hash = re_hash_index(hash);
            }
        }
    }
    return result;
}
//------------------------------------------------------------------------------
WORD get_code(PLZW_DATA_VC lzw) {
    WORD hash;
    WORD code;
    hash = get_hash_index(lzw);
    if (lzw->lp_prefix[hash] == lzw->prefix &&
        lzw->lp_suffix[hash] == lzw->suffix) {
        code = lzw->lp_code[hash];
    } else {
        while (lzw->lp_prefix[hash] != lzw->prefix ||
               lzw->lp_suffix[hash] != lzw->suffix) {
            hash = re_hash_index(hash);
        }
        code = lzw->lp_code[hash];
    }
    return code;
}
//------------------------------------------------------------------------------
VOID insert_table(PLZW_DATA_VC lzw) {
    WORD hash;
    hash = get_hash_index(lzw);
    if (lzw->lp_code[hash] == 0xFFFF) {
        lzw->lp_prefix[hash] = lzw->prefix;
        lzw->lp_suffix[hash] = lzw->suffix;
        lzw->lp_code[hash]   = lzw->code;
    } else {
        while (lzw->lp_code[hash] != 0xFFFF) {
            hash = re_hash_index(hash);
        }
        lzw->lp_prefix[hash] = lzw->prefix;
        lzw->lp_suffix[hash] = lzw->suffix;
        lzw->lp_code[hash]   = lzw->code;
    }
}

WORD load_buffer(BYTE *inbuf, int inlen, int *plen, PBUFFER_DATA_VC buffer) // Load file to buffer
{
    int i = *plen;
    int j = 0;
    for (; i < inlen && j < BUFFERSIZE; i++) {
        buffer->lp_buffer[j] = inbuf[i];
        j++;
    }
    buffer->index = 0;
    buffer->top   = (WORD)j;
    *plen         = i;
    // ReadFile(h_sour,buffer->lp_buffer,BUFFERSIZE,&ret,NULL);
    // buffer->index = 0;
    // buffer->top = (WORD)ret;
    return (WORD)j;
}
//------------------------------------------------------------------------------
WORD empty_buffer(BYTE *outbuf, int *pos, PBUFFER_DATA_VC buffer) // Output buffer to file
{
    DWORD ret = {};
    int   i   = 0;
    int   n   = *pos;
    if (buffer->end_flag) // The flag mark the end of decode
    {
        if (buffer->by_left) {
            buffer->lp_buffer[buffer->index++] = (BYTE)(buffer->dw_buffer >> 32 - buffer->by_left) << (8 - buffer->by_left);
        }
    }
    for (i = 0; i < buffer->index; i++) {
        outbuf[n] = buffer->lp_buffer[i];
        n++;
    }
    *pos = n;

    // WriteFile(lzw->h_dest, buffer->lp_buffer,buffer->index,&ret,NULL);
    buffer->index = 0;
    buffer->top   = i;
    return (WORD)ret;
}

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
//------------------------------------------------------------------------------
VOID out_code(WORD code, PBUFFER_DATA_VC buffer, PLZW_DATA_VC lzw, PSTACK_DATA_VC stack, BYTE *outbuf, int *pos) {
    WORD tmp;
    if (code < 0x100) {
        stack->lp_stack[stack->index++] = code;
    } else {
        stack->lp_stack[stack->index++] = lzw->lp_suffix[code];
        tmp                             = lzw->lp_prefix[code];
        while (tmp > 0x100) {
            stack->lp_stack[stack->index++] = lzw->lp_suffix[tmp];
            tmp                             = lzw->lp_prefix[tmp];
        }
        stack->lp_stack[stack->index++] = (BYTE)tmp;
    }

    while (stack->index) {
        if (buffer->index == BUFFERSIZE) {
            empty_buffer(outbuf, pos, buffer);
        }
        buffer->lp_buffer[buffer->index++] = stack->lp_stack[--stack->index];
    }
}
//------------------------------------------------------------------------------
VOID insert_2_table(PLZW_DATA_VC lzw) {
    lzw->lp_code[lzw->code]   = lzw->code;
    lzw->lp_prefix[lzw->code] = lzw->prefix;
    lzw->lp_suffix[lzw->code] = lzw->suffix;
    lzw->code++;

    if (lzw->code == ((WORD)1 << lzw->cur_code_len) - 1) {
        lzw->cur_code_len++;
        if (lzw->cur_code_len == CODE_LEN + 1)
            lzw->cur_code_len = 9;
    }
    if (lzw->code >= 1 << CODE_LEN) {
        re_init_lzw(lzw);
    }
}
//------------------------------------------------------------------------------
WORD get_next_code(PBUFFER_DATA_VC buffer, PLZW_DATA_VC lzw, BYTE *inbuf, int inlen, int *plen) {
    BYTE next;
    WORD code;
    while (buffer->by_left < lzw->cur_code_len) {
        if (buffer->index == BUFFERSIZE) {
            load_buffer(inbuf, inlen, plen, buffer);
        }
        next = buffer->lp_buffer[buffer->index++];
        buffer->dw_buffer |= (DWORD)next << (24 - buffer->by_left);
        buffer->by_left += 8;
    }
    code = buffer->dw_buffer >> (32 - lzw->cur_code_len);
    buffer->dw_buffer <<= lzw->cur_code_len;
    buffer->by_left -= lzw->cur_code_len;

    return code;
}
//------------------------------------------------------------------------------
VOID do_decode(PBUFFER_DATA_VC in, PBUFFER_DATA_VC out, PLZW_DATA_VC lzw, PSTACK_DATA_VC stack, BYTE *inbuf, int inlen, int *plen, BYTE *outbuf, int *pos) {
    WORD code;
    WORD tmp;
    while (in->index != in->top) {
        code = get_next_code(in, lzw, inbuf, inlen, plen);

        if (code < 0x100) {
            // code already in table
            // then simply output the code
            lzw->suffix = (BYTE)code;
        } else {
            if (code < lzw->code) {
                // code also in table
                // then output code chain

                tmp = lzw->lp_prefix[code];
                while (tmp > 0x100) {
                    tmp = lzw->lp_prefix[tmp];
                }
                lzw->suffix = (BYTE)tmp;
            } else {
                // code == lzw->code
                // code not in table
                // add code into table
                // and out put code
                tmp = lzw->prefix;
                while (tmp > 0x100) {
                    tmp = lzw->lp_prefix[tmp];
                }
                lzw->suffix = (BYTE)tmp;
            }
        }
        insert_2_table(lzw);
        out_code(code, out, lzw, stack, outbuf, pos);

        lzw->prefix = code;
    }
}
//------------------------------------------------------------------------------
VOID decode_vc(BYTE *inbuf, int inlen, BYTE *outbuf, int *outlen) {
    LZW_DATA_VC    lzw;
    BUFFER_DATA_VC in;
    BUFFER_DATA_VC out;
    STACK_DATA_VC  stack;
    BOOL           first_run;
    int            len    = 0;
    int            outpos = 0;

    first_run = TRUE;

    lzw_create(&lzw, NULL, NULL);
    buffer_create(&in);
    buffer_create(&out);
    stack_create(&stack);

    while (load_buffer(inbuf, inlen, &len, &in)) {
        if (first_run) {
            lzw.prefix = get_next_code(&in, &lzw, inbuf, inlen, &len);
            lzw.suffix = lzw.prefix;
            out_code(lzw.prefix, &out, &lzw, &stack, outbuf, &outpos);
            first_run = FALSE;
        }
        do_decode(&in, &out, &lzw, &stack, inbuf, inlen, &len, outbuf, &outpos);
    }

    empty_buffer(outbuf, &outpos, &out);
    *outlen = outpos;

    lzw_destory(&lzw);
    buffer_destory(&in);
    buffer_destory(&out);
    stack_destory(&stack);
}
namespace Union::Temp::Unresovled {
    void __decode(uint8_t *inbuf, int inlen, uint8_t *outbuf, int *outlen) {
        decode_vc(inbuf, inlen, outbuf, outlen);
    }
} // namespace Union::Temp::Unresovled
