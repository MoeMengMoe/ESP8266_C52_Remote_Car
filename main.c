#include "PWM.h"
#include "delay.h"
#include "display.h"
#include "fix.h"
#include "hardware.h"
#include "key.h"
#include "usart.h"

typedef unsigned char uchar;
typedef unsigned int uint;

// 声明外部资源
extern code unsigned char Tab[];
extern unsigned char DispBuf[6];
extern bit timer_start_flag;

uchar Recive_table[15];
uint i = 0;

void T0INTSVC() interrupt(1) // 定时器1的中断号为：1
{
  code unsigned char com[] = {0x20, 0x10, 0x08,
                              0x04, 0x02, 0x01}; // 显示位的端口控制字节
  static unsigned char n = 0;                    // n: 扫描显示位计数,0-7

  //================数码管定时扫描驱动显示===============
  TR0 = 0;
  TH0 = 0xFC;
  TL0 = 0x66; // 可以将FC66换成0000，降低扫描速度，观察和理解动态扫描
  TR0 = 1;
  P0 = 0xFF; // 消隐
  CS = 1;
  CS = 0;
  P0 = DispBuf[n]; // 更新扫描显示数据
  SS = 1;
  SS = 0;
  P0 = ~com[n]; // 重新显示
  CS = 1;
  CS = 0;
  n++; // 指向下一位扫描显示
  if (n >= 6)
    n = 0x00;
  //====================================================
  moto();
}

// 命令缓冲区
#define CMD_BUF_SIZE 16
uchar cmd_buffer[CMD_BUF_SIZE];
uint cmd_buf_head = 0;
uint cmd_buf_tail = 0;
bit cmd_received = 0;
uchar last_cmd = 0xFF;

void Uart() interrupt(4) {
  if (RI) {
    RI = 0;
    Recive_table[i] = SBUF;

    // 以 Recive_table[0] 为锚点进行帧同步
    // 检查位置0是否为帧头'-'，而不是检查当前字节，避免数据中含'-'时误触发
    if (Recive_table[0] == '-') {
      i++;
      led1 = 0;
      led2 = 0;
      DispBuf[5] = Tab[16];  // 显示 '-' 表示正在接收
    } else {
      i = 0;
    }

    // 收满完整9字节帧后保存命令到缓冲区
    if (i >= 9) {
      if (Recive_table[0] == '-' &&
          Recive_table[1] == '1' &&
          Recive_table[2] == 'P' &&
          Recive_table[3] == 'd' &&
          Recive_table[8] == '0') {

        // 将命令存入环形缓冲区（第8字节，索引7为动作位）
        if ((cmd_buf_head + 1) % CMD_BUF_SIZE != cmd_buf_tail) {
          cmd_buffer[cmd_buf_head] = Recive_table[7];
          cmd_buf_head = (cmd_buf_head + 1) % CMD_BUF_SIZE;
          cmd_received = 1;
        }
      }
      i = 0;  // 帧处理完毕，重置索引
    }
  } else {
    TI = 0;
  }
}

int main() {
  uint heartbeat_ticks = 0;

  DispInit(); // 内部已包含定时器初始化
  pwmInit();  // 内部已包含电机初始化
  KeyInit();
  delay_ms(100); // 等待系统稳定
  // 开机自检：显示 123456
  // ... (你的自检逻辑) ...
  DispBuf[5] = Tab[1];
  DispBuf[4] = Tab[2];
  DispBuf[3] = Tab[3];
  DispBuf[2] = Tab[4];
  DispBuf[1] = Tab[5];
  DispBuf[0] = Tab[6];
  delay_ms(2000);
  usartInit();
  WIFI_Init();

  timer_start_flag = 1;
  delay_ms(3000); // 等待 TCP 透传链路稳定

  // 初始化完毕，数码管显示 READY
  // 7段码: r=0x50, E=Tab[14], A=Tab[10], d=Tab[13], Y=0x6E
  DispBuf[5] = 0x50;    // r
  DispBuf[4] = Tab[14]; // E
  DispBuf[3] = Tab[10]; // A
  DispBuf[2] = Tab[13]; // d
  DispBuf[1] = 0x6E;    // Y
  DispBuf[0] = 0x00;    // 空

  while (1) {
    KeyScanMotor();

    // 处理串口命令
    if (cmd_received) {
      cmd_received = 0;
      // 从缓冲区取出命令
      while (cmd_buf_tail != cmd_buf_head) {
        uchar cmd = cmd_buffer[cmd_buf_tail];
        cmd_buf_tail = (cmd_buf_tail + 1) % CMD_BUF_SIZE;

        if (cmd >= '0' && cmd <= '4') {
          motomode = cmd - '0';  // 转换为整数 0-4

          // 反馈指示
          led1 = 0;
          led2 = 0;

          // 显示命令值在数码管（全部6位）
          DispBuf[5] = Tab[16]; // '-'
          DispBuf[4] = Tab[1];  // '1'
          DispBuf[3] = Tab[cmd - '0'];  // 显示命令数字
          DispBuf[2] = Tab[13]; // 'd'
          DispBuf[1] = Tab[0];  // '0'
          DispBuf[0] = Tab[0];  // '0'
        }

        // 发送应答给手机（仅新命令时发送，避免重复）
        if (cmd != last_cmd) {
          last_cmd = cmd;
          {
            char ack[3];
            ack[0] = cmd;
            ack[1] = '\n';
            ack[2] = '\0';
            SENT_At(ack);
          }
        }
      }
    }

    if (heartbeat_ticks >= 200) {
      heartbeat_ticks = 0;
      led1 = 1;
      led2 = 1;
      // SENT_At(“12345\n”); // 发5字节”12345”给手机
      // buzzer = 0;
      delay_ms(20);
      // buzzer = 1;
    }

    delay_ms(10);
    heartbeat_ticks++;
  }
}
