#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>           // 定时器
#include <QtMath>           // 用于生成正弦波 qSin
#include <QRandomGenerator> // 用于生成一点随机噪声，模拟真实信号
#include <QMap>             // 用于存储通道配置

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnExport_2_clicked();

private:
    Ui::MainWindow *ui;
    void initChannelTable();
    // 辅助函数：创建一个居中的复选框
    QWidget* createCenteredCheckBox();
    void wireChannelToggles();
    
    // =========================================================
    // 测试函数：波形显示模块测试
    // =========================================================
    /**
     * @brief 启动波形测试数据生成
     * @param channelCount 要测试的通道数量（1-10）
     * @param updateInterval 更新间隔（毫秒），默认33ms（约30Hz）
     */
    void startWaveformTest(int channelCount = 3, int updateInterval = 33);
    
    /**
     * @brief 停止波形测试数据生成
     */
    void stopWaveformTest();
    
    /**
     * @brief 配置通道显示状态
     * @param channelId 通道ID（0开始）
     * @param showVoltage 显示电压
     * @param showCurrent 显示电流
     * @param showPower 显示功率
     */
    void configureChannelDisplay(int channelId, bool showVoltage, bool showCurrent, bool showPower);
    
    /**
     * @brief 配置所有通道的显示状态
     * @param channelCount 通道数量
     * @param defaultShowAll 是否默认显示所有（true=全部显示，false=只显示电压）
     */
    void configureAllChannelsDisplay(int channelCount, bool defaultShowAll = true);
    
    /**
     * @brief 生成测试数据（定时器回调函数）
     */
    void generateTestData();
    
private:
    QTimer *m_testTimer = nullptr;      // 测试定时器
    int m_testChannelCount = 3;         // 当前测试的通道数量
    double m_testTime = 0.0;            // 测试时间计数器
    QMap<int, bool> m_channelVoltageVisible;  // 通道电压显示状态
    QMap<int, bool> m_channelCurrentVisible;  // 通道电流显示状态
    QMap<int, bool> m_channelPowerVisible;    // 通道功率显示状态
};
#endif // MAINWINDOW_H
