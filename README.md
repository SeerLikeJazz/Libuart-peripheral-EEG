# Libuart-peripheral-EEG
8 ch signals send by BLE   
![](/Image/PCB_board.jpg)  


## 待完成
- DFU模式下，无法按键关机？？？
- 电池电量监测
- 各种状态下功耗验证

## 更新记录
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