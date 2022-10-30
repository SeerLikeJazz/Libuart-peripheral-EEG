# Libuart-peripheral-EEG
8 ch signals send by BLE   
![](/Image/PCB_board.jpg)  

## 更新记录
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