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
    if(KEY_K1 == 0) {
        motomode = 1;
    } else if(KEY_K2 == 0) {
        motomode = 2;
    } else if(KEY_K3 == 0) {
        motomode = 3;
    } else if(KEY_K4 == 0) {
        motomode = 4;
    } else {
        motomode = 0;
    }
}
