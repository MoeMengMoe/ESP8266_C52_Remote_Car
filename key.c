#include "key.h"

extern unsigned int motomode;

sbit KEY_K1 = P3^4;
sbit KEY_K2 = P3^5;
sbit KEY_K3 = P3^6;
sbit KEY_K4 = P3^7;

void KeyInit(void)
{
    KEY_K1 = 1;
    KEY_K2 = 1;
    KEY_K3 = 1;
    KEY_K4 = 1;
}

void KeyScanMotor(void)
{
    static unsigned char last_key = 0;
    unsigned char curr_key = 0;

    if(KEY_K1 == 0) {
        curr_key = 1;
    } else if(KEY_K2 == 0) {
        curr_key = 2;
    } else if(KEY_K3 == 0) {
        curr_key = 3;
    } else if(KEY_K4 == 0) {
        curr_key = 4;
    }

    // 仅在新按键按下时触发（边沿检测，防抖）
    if(curr_key != 0 && curr_key != last_key) {
        if(motomode == curr_key) {
            motomode = 0;        // 同键再按 → 停止
        } else {
            motomode = curr_key; // 切换模式
        }
    }

    last_key = curr_key;
}
