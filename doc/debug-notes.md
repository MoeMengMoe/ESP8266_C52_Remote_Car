# 调试记录与已知问题

## 当前项目状态 (2026-05-09)

### 构建状态：⚠️ 42 个链接警告，0 个错误

最新构建 (uv4.log) 显示只编译了 `main.c`，链接阶段产生 42 个 L1/L2 警告。虽然 Keil 仍然生成了 hex 文件 (0 Error(s))，但未解析的外部符号意味着运行时会跳转到错误地址，**功能不可能正常**。

### 未解析符号清单

| 符号 | 定义位置 | 引用位置 | 问题 |
|------|---------|---------|------|
| `WIFI_INIT` | usart.c | main.c | usart.c 未编译/链接 |
| `USARTINIT` | usart.c | main.c | usart.c 未编译/链接 |
| `_SENT_AT` | usart.c | main.c | usart.c 未编译/链接 |
| `PWMINIT` | PWM.c | main.c | PWM.c 未编译/链接 |
| `MOTO` | PWM.c | main.c (T0 ISR) | PWM.c 未编译/链接 |
| `MOTOMODE` | PWM.c (全局变量) | main.c (UART ISR + 主循环) | PWM.c 未编译/链接 |
| `_DELAY_MS` | delay.c | main.c + usart.c | delay.c 未编译/链接 |
| `I` | **未声明!** | main.c (UART ISR) | 变量未在任何地方定义 |

### 推测原因

从 git diff 可以看到，`remote_car.uvproj` 和 `remote_car.uvopt` 中添加了 delay.c / PWM.c / usart.c 及其头文件。这些文件在最初的 commit 中已存在，但当时的 uvproj 只包含 `main.c` 和 `display.c`。文件加入项目后**尚未在 Keil 中执行完整重新编译**（Rebuild all），导致这些 .c 文件的 .obj 不存在，链接器找不到符号。

**解决方法**：在 Keil uVision 中执行 `Project → Rebuild all target files`。

---

## 已知 Bugs

### Bug 1: 变量 `i` 未声明 (严重)

**位置**: `main.c` UART ISR (`Uart() interrupt 4`)

**现象**: 链接器报告 `SYMBOL: I` 未解析。整个 UART 接收状态机依赖 `Recive_table[i]` 中的索引 `i`，但 `i` 从未在任何地方声明（非局部变量、非全局变量、非 static）。

**后果**: C51 编译器会将未声明的变量默认当作 `int` 类型处理，但不会为它分配存储空间。链接时变成未解析外部符号。即使侥幸编译通过，初始值也是不确定的，UART 帧接收会错位。

**修复建议**:
```c
// 在 main.c 顶部、Uart() 函数之前添加:
static unsigned char i = 0;
```
或者更安全的重构：把 `i` 和 `Recive_table[]` 封装为结构体 + 状态机。

### Bug 2: 定时器配置冲突 (严重)

**位置**: `display.c:DispInit()` vs `usart.c:usartInit()`

**调用链**:
```
main()
  DispInit()   → TMOD = 0x11  (Timer1: 16位定时, Timer0: 16位定时)
                  TH1=0xFC, TL1=0x66  (1ms 溢出用于数码管扫描)
                  ET1=1, TR1=1
  ...
  usartInit()  → TMOD = 0x20  (Timer1: 8位自动重装/波特率, Timer0: 13位)
                  TH1=0xFD  (9600bps 波特率)
                  TR1=1
```

**后果**: `usartInit()` 完全覆盖了 `DispInit()` 对 TMOD 的设置。Timer1 从 16 位定时器变成波特率发生器。T1INTSVC（数码管扫描 ISR）不再以 1ms 间隔触发，而是以波特率溢出频率触发（约 9600Hz），导致：
- 数码管扫描完全错乱
- 跑表计时 (sec++) 完全不准

**修复建议**: Timer1 不能同时用于波特率发生器和数码管扫描。方案：将数码管扫描移到 Timer0，在 `DispInit()` 中配置 TH0/TL0 并启动 TR0。

### Bug 3: Timer0 从未启动 (严重)

**位置**: `main.c:T0INTSVC()` + 初始化流程

**现象**: `T0INTSVC() interrupt (1)` 函数存在，且负责调用 `moto()` 完成电机 PWM 控制，但 **TR0 从未在 ISR 外部被置 1**。Timer0 的启动代码 (`TR0=1`) 只存在于 ISR 内部 (main.c:35)，这意味着：
- 如果 Timer0 从未被外部启动，电机 PWM 永远不会运行
- 这是"鸡生蛋"问题：ISR 启动了 Timer0，但 Timer0 不运行就无法触发 ISR

**修复建议**:
```c
void DispInit(void) {
    // ... 现有代码 ...
    // 同时初始化 Timer0:
    TH0 = 0xFC;
    TL0 = 0x66;
    ET0 = 1;   // 使能 Timer0 中断
    TR0 = 1;   // 启动 Timer0
}
```

### Bug 4: PWM_EN2 声明错误 (已修复)

**位置**: `PWM.h`

**原始代码**: `bit PWM_EN2 = P1 ^ 5;`

**问题**: `bit` 是 C51 中用于声明位寻址 RAM 变量（地址 0x20–0x2F）的关键字。声明一个端口引脚应该用 `sbit`。`bit PWM_EN2 = P1 ^ 5` 实际上是声明了一个叫 PWM_EN2 的 RAM 位变量并初始化为 `P1 ^ 5` 的值（即 P1.5 的当前电平），而不是指向 P1.5 引脚。

**修复**: 已改为 `sbit PWM_EN2 = P1 ^ 5;`（符合预期行为：控制 P1.5 输出）。

### Bug 5: LED 指示逻辑不一致 (轻微)

**位置**: `main.c` UART ISR + `usart.c` WIFI_Init()

**现象**: LED 亮灭逻辑混乱：
- `WIFI_Init()` 中 `led1=0` 表示"发送成功亮灯"
- `WIFI_Init()` 中 `led1=1` 表示"发送成功灭灯"
- UART ISR 中 `led1=0, led2=0` 表示"收到帧头，亮灯提示"
- UART ISR 中 `led2=1` 在 case '0' 表示停止模式
- 主循环中 `led1=1, led2=1` 注释为"发送成功后灭灯"

缺乏统一的约定（0=亮还是0=灭），给调试增加困惑。建议统一为 `#define LED_ON 0` / `#define LED_OFF 1`。

### Bug 6: 按键消抖缺失 (轻微)

**位置**: `main.c` 主循环

```c
while(key1!=0);       // 等待按下
motomode++;           // 切换模式
if(motomode>2) motomode=0;
```

没有消抖处理，一次按键可能触发多次模式切换。也没有等待释放的逻辑，松开按键也会导致误触发。

---

## 项目版本演进

从 git log 和 diff 分析：

```
20aa74d Initial Commit (最早的提交)
  ├─ main.c       — 原始版本（可能只包含基本的 main + UART ISR）
  ├─ display.c/h  — 原始版本
  ├─ PWM.c/h      — 存在但未加入项目
  ├─ usart.c/h    — 存在但未加入项目
  ├─ delay.c/h    — 存在但未加入项目
  ├─ fix.h        — 存在
  ├─ hardware.h   — 存在
  └─ remote_car.uvproj — 只包含 main.c + display.c

当前工作区 (未提交的修改):
  ├─ .clangd              — 添加了 -include fix.h, 移除 ClangTidy.Terminate
  ├─ PWM.h                — 修复 PWM_EN2 从 bit 改为 sbit
  ├─ main.c               — 空白字符变化
  ├─ remote_car.uvproj    — 添加所有 .c/.h 文件到项目
  ├─ remote_car.uvopt     — 添加所有文件到项目视图
  ├─ remote_car.uvgui.*   — Keil IDE 窗口状态变化
  └─ .vscode/uv4.log      — 构建日志（42w 版本）
```

## 开发环境

| 工具 | 版本/路径 | 用途 |
|------|----------|------|
| Keil uVision 5 | `D:/Keil_v5/` | 编译、链接、烧录 |
| C51 编译器 | Keil 内置 | MCS-51 工具链 |
| VS Code | — | 代码编辑 (clangd 语法高亮) |
| Google Drive | `goodrive://.../code` | 文件同步 (_gsdata_ 日志) |
| STC ISP | (未记录) | 烧录 hex 到 STC89C52RC |

## 调试检查清单

在测试硬件之前，按顺序确认：

- [ ] Keil 中 Rebuild all — 0 Error(s), 0 Warning(s)
- [ ] 示波器/逻辑分析仪确认 P2.6 (SS)、P2.7 (CS) 有时钟输出 → 数码管扫描正常
- [ ] P1.2–P1.7 有 PWM 波形 → 电机控制正常
- [ ] 串口助手 (9600bps) 接 P3.0/P3.1 → 上电后收到 AT 指令序列
- [ ] ESP8266 蓝灯常亮 → WiFi 已连接
- [ ] 手机/PC 连接同一服务器 → 发送 "-1PdXXXX0" 帧，观察电机响应
