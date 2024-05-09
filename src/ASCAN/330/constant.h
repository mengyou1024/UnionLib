#ifndef CONSTANT_H
#define CONSTANT_H

/*
#define u_char unsigned char
#define u_short unsigned short
#define u_int unsigned int
#define u_long unsigned long
*/

typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;
// typedef unsigned char BOOL;
// #ifndef WIN32
// typedef VOID void;
// #endif
#define BOOL u_char

typedef u_short WORD;
typedef u_char  BYTE;
typedef BYTE   *LPBYTE;
typedef u_long  DWORD;
typedef WORD   *LPWORD;

#define TRUE  1
#define FALSE 0

#endif // CONSTANT_H
