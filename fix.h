#ifndef __VS_CODE_H__
#define __VS_CODE_H__
#if defined(__clang__) || defined(VSCODE)
    #define _nop_() (void*)0
    #define interrupt(x)
    #define using(x)
    #define code
    #define __INTRINS_H__     // 封锁 intrins.h
    #define _STDDEF_H_        // 封锁 stddef.h
    #define _STDINT_H_        // 封锁 stdint.h
    #define _STDBOOL_H_       // 封锁 stdbool.h
    #define _WCHAR_T_DEFINED_ // 解决 wchar_t 重复定义冲突
    #include <stdbool.h>
    // #include <stdint.h>
    typedef bool bit;
    class sfr{
    public:
        sfr(int){};
        sfr(){};
        ~sfr(){};
        bool operator ^ (unsigned short data);
        bool operator ^ (int data);
        sfr& operator=(const sfr& other);
        sfr& operator=(const int other);
        int operator ^= (int data);
        int operator &= (int data);
        int operator |= (int data);
        int operator | (int data);
        int operator & (int data);
        operator int(){};
    };
    #define sbit bool
#else
    #define interrupt(x) interrupt x
    #define using(x) using x
#endif
#endif