# 波形显示模块使用示例

## 新增接口使用说明

### 1. addChannelData() - 多通道数据输入

```cpp
#include "modules/WaveformView/waveformwidget.h"

// 创建多通道数据包
WaveformWidget::MultiChannelData data;

// 添加通道0的数据（slot1）
data.channelData[0] = {
    1.0,    // time: 1.0秒
    3.3,    // voltage: 3.3V
    0.5,    // current: 0.5A
    1.65    // power: 1.65W (可选，如果为NaN会自动计算)
};

// 添加通道1的数据（slot2）
data.channelData[1] = {
    1.0,    // time: 1.0秒
    5.0,    // voltage: 5.0V
    0.2,    // current: 0.2A
    NAN     // power: 自动计算为 5.0 * 0.2 = 1.0W
};

// 从工作线程调用（线程安全）
emit dataReady(data);  // 通过信号槽传递

// 或在主线程直接调用
waveformWidget->addChannelData(data);
```

### 2. setChannelVisible() - 通道显示控制

```cpp
// 控制通道0的显示状态
waveformWidget->setChannelVisible(
    0,              // channelId: 通道0
    true,           // voltageVisible: 显示电压曲线
    true,           // currentVisible: 显示电流曲线
    false           // powerVisible: 隐藏功率曲线
);

// 控制通道1只显示电压
waveformWidget->setChannelVisible(1, true, false, false);

// 控制通道2显示所有曲线
waveformWidget->setChannelVisible(2, true, true, true);
```

### 3. clearChannel() - 清空单通道数据

```cpp
// 清空通道0的数据
waveformWidget->clearChannel(0);

// 清空通道1的数据
waveformWidget->clearChannel(1);

// 清空所有通道数据
waveformWidget->clear();
```

## 完整使用示例

```cpp
// 在MainWindow中
void MainWindow::onDataReceived(const ProcessedDataPacket &packet)
{
    // 创建多通道数据包
    WaveformWidget::MultiChannelData data;
    
    // 遍历处理后的数据包，填充到波形显示模块
    for (int i = 0; i < packet.channelIds.size(); ++i) {
        int chId = packet.channelIds[i];
        
        data.channelData[chId] = {
            packet.timestamps[i],
            packet.voltages[i],
            packet.currents[i],
            packet.powers[i]  // 如果为NaN会自动计算
        };
    }
    
    // 添加到波形显示（线程安全）
    ui->waveformWidget->addChannelData(data);
}

// 响应通道选择复选框
void MainWindow::onChannelCheckboxToggled(int channelId, bool checked)
{
    // 根据复选框状态控制通道显示
    ui->waveformWidget->setChannelVisible(
        channelId,
        checked,  // 电压
        checked,  // 电流
        checked   // 功率
    );
}

// 清空特定通道
void MainWindow::onClearChannel(int channelId)
{
    ui->waveformWidget->clearChannel(channelId);
}
```

## 线程安全使用

### 方式1：通过信号槽（推荐）

```cpp
// 在工作线程中
class DataProcessor : public QObject
{
    Q_OBJECT
    
signals:
    void dataReady(const WaveformWidget::MultiChannelData &data);
    
private:
    void processData() {
        WaveformWidget::MultiChannelData data;
        // ... 填充数据 ...
        emit dataReady(data);  // 发送信号
    }
};

// 在主窗口连接（使用QueuedConnection确保线程安全）
connect(dataProcessor, &DataProcessor::dataReady,
        waveformWidget, &WaveformWidget::addChannelData,
        Qt::QueuedConnection);
```

### 方式2：使用QMetaObject::invokeMethod

```cpp
// 在工作线程中
QMetaObject::invokeMethod(waveformWidget, "addChannelData",
                          Qt::QueuedConnection,
                          Q_ARG(WaveformWidget::MultiChannelData, data));
```

## 注意事项

1. **通道ID范围**：通道ID必须在 0 到 kChannelCount-1 之间（当前为0-9）
2. **线程安全**：所有接口都支持从工作线程调用，但必须使用信号槽或QMetaObject::invokeMethod
3. **功率计算**：如果power为NaN、Inf或0.0，会自动计算为 voltage * current
4. **向后兼容**：原有的`addData()`接口仍然可用，会自动更新通道0的数据
5. **显示控制**：通道显示状态受两个因素控制：
   - 全局显示状态（`setVoltageVisible()`等）
   - 通道显示状态（`setChannelVisible()`）
   - 最终显示 = 全局状态 && 通道状态

