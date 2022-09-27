# Libuart-peripheral-EEG
8 ch signals send by BLE

## 更新记录
### 22.09.27
- 连接间隔设置20ms，发送定时器20ms中断，50次计数后统计发送速率
- 连接建立后，sd_ble_gap_phy_update，更新为2M的phy