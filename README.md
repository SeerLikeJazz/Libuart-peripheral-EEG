# Libuart-peripheral-EEG
8 ch signals send by BLE   
![](/Image/PCB_board.jpg)  

## Attention
- 长按进入BSP_EVENT_SLEEP,配置关机引脚中断，按键释放(LOTOHI)的时候才进入中断
- 指示灯逻辑
- 软件定时器：system_task_timer(500ms,指令是否更新)；charging_timer(500ms充电用定时器)
- DevicePara结构体，变量adv_started用来标记是否广播
- 连接间隔20ms,ios会断开连接

## 待完成
- 电池电量监测，服务or数据包？
- 各种状态下功耗验证：STANDBY-3mA；PWDN-1.26mA；全速工作：15mA；待机：7uA
- 可以失能nRF_Log

## 更新记录
### 23.04.14
- 长按关机时亮红灯提示

### 22.12.19
- fix："Z"发送后"W"运行不对，重新initialize_ads

### 22.11.23
- 整理程序注释，优化函数

### 22.11.22
- 设置发送功率为最高+4dBm
- 添加RSSI测试工程（青风开发板、EEG从机和主机），通过DEVICE_NUM宏定义区分

### 22.11.21-2
- 开机时，进入standby模式，3mA功耗；蓝牙断开连接时，进入standby模式
- 增加STANDBY和WAKEUP指令

### 22.11.21-1
- nus_data_handler服务，接收指令；定时器system_task_timer，500ms处理指令
- DFU工程修改：LED的状态指示、广播名字

### 22.11.21
- fix：增加DFU服务后，无法按键关机。 secure_bootloader工程里，需要删去CONFIG_GPIO_AS_PINRESET

### 22.11.20
- 增加DFU服务，用宏定义控制是否使能。

### 22.11.16
- fix：蓝牙连接后长按无法关机。修改connection_buttons_configure()函数

### 22.11.04
- 调整指示灯显示状态
- 充电时的处理：当蓝牙处于连接状态时，先断开连接，然后停止广播；当蓝牙处于广播状态时，直接停止广播。  
  adv_started 和 m_conn_handle记录连接状态和广播状态

### 22.11.03暂存
- fix：长按关机进入sleep，立马会被唤醒。检测到长按后，配置key_exit_pin高电平（释放后）中断

### 22.11.03
- 定时器charging_timer检测 开机唤醒充电或中途充电 充电器的拔出与充满

### 22.11.02
- 充电器插入检测、充满检测
- 充电红灯闪烁（BSP_INDICATE_USER_STATE_3），充满绿灯常亮（BSP_INDICATE_USER_STATE_1）。bsp_indication_set()使用前记得bsp_board_leds_off()其他led

### 22.10.31
- 充电器插入唤醒
- 充电相关引脚初始化

### 22.10.29
- 添加driver_power.c
- define 定义引脚
- 修改connection_buttons_configure()函数，蓝牙连接后也可以关机

### 22.10.27
- 休眠25uA，还可以降低；nrf_gpio_cfg_default(AVDD_EN)屏蔽以后，7uA
- RESET引脚设置为普通GPIO模式，用于开关机

### 22.10.12
- NFC引脚设置为普通GPIO模式
- 添加并修改matlab plot 代码

### 22.09.30
- Connected和Disconnected事件中使能失能DRDY中断引脚
- 解决0x3401 ERROR ，BLE_ERROR_GATTS_SYS_ATTR_MISSING，断开后可再重连

### 22.09.27
- 3V3、5V电源打开
- LED引脚序号修改

### 22.09.27
- 连接间隔设置20ms，发送定时器20ms中断，50次计数后统计发送速率
- 连接建立后，sd_ble_gap_phy_update，更新为2M的phy