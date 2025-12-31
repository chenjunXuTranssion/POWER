#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include "qcustomplot.h"
#include <QWidget>
#include <QColorDialog>
#include <QPen>
#include <QVector>
#include <QMap>
#include <QPointer>

namespace Ui {
class WaveformWidget;
}

/**
 * @brief 波形显示核心类
 * 继承自 QWidget，封装了 QCustomPlot 用于双图表（电压/电流）同步展示。
 */
class WaveformWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget *parent = nullptr);
    ~WaveformWidget() override;

    // 测试：虚拟通道数量（每个通道包含 V/I/P 三条曲线）
    static constexpr int kChannelCount = 20;

    // --- 数据结构定义 ---
    /**
     * @brief 单个通道的数据点
     */
    struct ChannelDataPoint {
        double time;        // 时间戳（秒）
        double voltage;     // 电压（V）
        double current;     // 电流（A）
        double power;       // 功率（W），如果为NaN则自动计算 power = voltage * current
    };

    /**
     * @brief 多通道数据包（一次更新的多通道数据）
     */
    struct MultiChannelData {
        QMap<int, ChannelDataPoint> channelData;  // channelId -> 数据点
        // channelId 范围：0 到 kChannelCount-1
    };

    // --- 数据操作接口 ---
    /**
     * @brief 添加单个数据点（单通道，通道0）- 保留用于向后兼容
     */
    void addData(double time, double voltage, double current, double power);
    
    /**
     * @brief 添加多通道数据（线程安全，支持从工作线程调用）
     * @param data 多通道数据包
     */
    void addChannelData(const MultiChannelData &data);
    
    /**
     * @brief 清空所有通道的数据
     */
    void clear();
    
    /**
     * @brief 清空指定通道的数据
     * @param channelId 通道ID（0 到 kChannelCount-1）
     */
    void clearChannel(int channelId);

    // --- 图层控制（控制曲线 Show/Hide） ---
    /**
     * @brief 设置所有通道的电压/电流/功率显示状态（全局控制）
     */
    void setVoltageVisible(bool visible);
    void setCurrentVisible(bool visible);
    void setPowerVisible(bool visible);
    
    /**
     * @brief 设置指定通道的显示状态
     * @param channelId 通道ID（0 到 kChannelCount-1）
     * @param voltageVisible 电压曲线是否显示
     * @param currentVisible 电流曲线是否显示
     * @param powerVisible 功率曲线是否显示
     */
    void setChannelVisible(int channelId, bool voltageVisible, 
                           bool currentVisible, bool powerVisible);

    // --- 视图跟随控制 ---
    void setAutoFollow(bool enable) { m_autoFollow = enable; }
    bool autoFollow() const { return m_autoFollow; }

    // --- 测量工具定义 ---
    enum class MeasureToolMode {
        Off = 0,        // 禁用工具
        Crosshair = 1,  // 十字准星：随鼠标移动，显示单点精确值
        Calipers = 2,   // 测量卡尺：两根竖线测时间差(ΔT)，两根横线测幅值差(ΔV/ΔA)
    };

    void setMeasureToolMode(MeasureToolMode mode);
    MeasureToolMode measureToolMode() const { return m_measureMode; }
    void cycleMeasureToolMode(); // 在 Off -> Crosshair -> Calipers 之间轮转

private slots:
    // --- QCustomPlot 波形控件相关的信号关联槽 ---
    void onPlottableDoubleClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event); // 双击曲线改颜色
    void onSelectionRectAccepted(const QRect &rect, QMouseEvent *event);           // 框选放大
    void onSelectionChanged();                                                     // 选中曲线高亮
    void onPlottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);

private:
    // --- 内部初始化与辅助 ---
    void setupCharts();
    void applyVisualDownsample(QCustomPlot *plot); // 高性能渲染：大数据量时只抽取可见点绘制
    void updateChannelGraphs(); // 更新所有通道的图表显示
    void applyVisualDownsampleForChannel(QCustomPlot *plot, int graphIndex, 
                                         const QVector<double> &time, 
                                         const QVector<double> &values); // 为单个通道应用降采样

    // --- 测量工具核心私有方法 ---
    void ensureMeasureItems(); // 延迟加载：第一次开启工具时创建所有线条和文本对象
    void clearMeasureItems();  // 彻底移除所有测量 Item 指针
    void updateCrosshair(QCustomPlot *plot, const QPoint &pos); // 计算鼠标位置对应的坐标
    void updateCalipersText(); // 计算并更新 ΔT = T2 - T1 等统计文字内容

    // --- 鼠标交互分发（用于处理拖拽卡尺线条） ---
    bool handleMeasureMousePress(QCustomPlot *plot, QMouseEvent *e);
    bool handleMeasureMouseMove(QCustomPlot *plot, QMouseEvent *e);
    bool handleMeasureMouseRelease(QCustomPlot *plot, QMouseEvent *e);
    
    // --- Y轴缩放处理（以0刻度为中心） ---
    void handleYAxisZoom(QCustomPlot *plot, QWheelEvent *wheelEvent, QCPAxis *targetAxis);

private:
    Ui::WaveformWidget *ui;
    double m_viewWidth = 10.0;      // 当前视图显示的时间宽度（秒），随缩放自动更新

    // --- 自定义交互状态 ---
    bool m_isDragging = false;      // 标记是否正处于中键或特殊拖拽状态
    QPoint m_lastDragPos;           // 记录鼠标上一帧位置，用于计算位移偏移量
    bool m_autoFollow = true;       // 是否自动跟随最新数据（缩放/拖拽后置 false）
    bool m_isAutoFollowing = false; // 标记当前是否正在执行自动跟随操作（防止误关闭）

    // --- 核心原始数据池 ---
    // 单通道数据（向后兼容，用于通道0）
    QVector<double> m_time;         // 时间轴数据 (X轴)
    QVector<double> m_voltage;      // 电压数据 (Y轴1)
    QVector<double> m_current;      // 电流数据 (Y轴2)
    QVector<double> m_power;        // 功率数据 (派生计算值)
    
    // 多通道数据存储（每个通道独立存储）
    struct ChannelData {
        QVector<double> time;
        QVector<double> voltage;
        QVector<double> current;
        QVector<double> power;
    };
    QMap<int, ChannelData> m_channelDataMap;  // channelId -> 通道数据

    bool m_voltageVisible = true;
    bool m_currentVisible = true;
    bool m_powerVisible = true;
    
    // 每个通道的显示状态（通道ID -> 显示状态）
    struct ChannelVisibility {
        bool voltageVisible = true;
        bool currentVisible = true;
        bool powerVisible = true;
    };
    QMap<int, ChannelVisibility> m_channelVisibility;  // channelId -> 显示状态

    bool m_syncingRange = false;    // 状态锁：防止双轴联动死循环（A改B, B又触发A）

    // --- 测量系统状态变量 ---
    MeasureToolMode m_measureMode = MeasureToolMode::Off;

    // 卡尺在坐标系中的位置值
    double m_caliperX1 = 0;         // 卡尺竖线1的时间位置
    double m_caliperX2 = 1;         // 卡尺竖线2的时间位置
    double m_caliperY1V = 0, m_caliperY2V = 1; // 电压区域的两根横线
    double m_caliperY1I = 0, m_caliperY2I = 1; // 电流区域的两根横线

    // 拖拽目标标记：区分当前鼠标点中的是哪一根线
    enum class DragTarget { None, X1, X2, Y1V, Y2V, Y1I, Y2I };
    DragTarget m_dragTarget = DragTarget::None;

    // --- UI 对象指针（由 QCustomPlot 内部负责内存管理） ---

    // 1. 十字光标组件 (Crosshair)
    QCPItemStraightLine *m_crossV_V = nullptr; // 电压图垂直线
    QCPItemStraightLine *m_crossH_V = nullptr; // 电压图水平线
    QCPItemText *m_crossText_V = nullptr;      // 电压图坐标跟随文字

    QCPItemStraightLine *m_crossV_I = nullptr; // 电流图垂直线
    QCPItemStraightLine *m_crossH_I = nullptr; // 电流图水平线
    QCPItemText *m_crossText_I = nullptr;      // 电流图坐标跟随文字

    // 2. 卡尺组件 (Calipers)
    QCPItemStraightLine *m_vline1_V = nullptr; // 竖线1 (跨图联动)
    QCPItemStraightLine *m_vline2_V = nullptr; // 竖线2 (跨图联动)
    QCPItemStraightLine *m_vline1_I = nullptr;
    QCPItemStraightLine *m_vline2_I = nullptr;

    QCPItemStraightLine *m_hline1_V = nullptr; // 电压图横线1
    QCPItemStraightLine *m_hline2_V = nullptr; // 电压图横线2
    QCPItemStraightLine *m_hline1_I = nullptr; // 电流图横线1
    QCPItemStraightLine *m_hline2_I = nullptr; // 电流图横线2

    // 3. 统计结果文本
    QCPItemText *m_caliperText_V = nullptr;    // 在电压图显示 ΔT 和 ΔV
    QCPItemText *m_caliperText_I = nullptr;    // 在电流图显示 ΔT 和 ΔI

    // 4. 记录上一次被高亮的曲线，用于只调整选中曲线的线宽
    QVector<QPointer<QCPGraph>> m_lastHighlightedGraphs;

protected:
    /**
     * @brief 事件过滤器
     * 作用：在 QCustomPlot 接收到鼠标事件前进行拦截。
     * 用于实现：1. 滚轮自定义缩放 2. 测量工具线条的点击与拖拽 3. 屏蔽默认右键菜单
     */
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // WAVEFORMWIDGET_H
