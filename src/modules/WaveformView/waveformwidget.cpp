#include "waveformwidget.h"
#include "ui_waveformwidget.h"
#include <QSignalBlocker>
#include <algorithm>
#include <QMouseEvent>
/**
 * @brief 构造函数：初始化波形显示组件
 * @param parent 父窗口指针
 */
WaveformWidget::WaveformWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaveformWidget)
{
    ui->setupUi(this);
    setupCharts();
}

/**
 * @brief 析构函数：清理资源
 */
WaveformWidget::~WaveformWidget()
{
    delete ui;
}

/**
 * @brief 初始化并配置上下两个图表（电压图、电流/功率图）
 * 
 * 主要功能：
 * 1. 配置图例样式（位置、外观、交互）
 * 2. 配置图表交互权限（缩放、选中、框选）
 * 3. 创建曲线（电压、电流、功率）
 * 4. 连接信号与槽（双击改色、选中高亮、点击显示数值、框选统计）
 * 5. 设置 X 轴联动（上下两个图的时间轴同步）
 */
void WaveformWidget::setupCharts()
{
    //  确保 UI 中的图表控件已正确初始化
    if (!ui->plotVoltage || !ui->plotCurrent) return;

    /**
     * Lambda 函数：配置单个图表的交互功能
     * 包括：缩放、选中、框选、事件过滤、信号连接
     */
    auto configPlot = [this](QCustomPlot *plot) {
        // 隐藏图例，不再显示右侧的颜色/曲线选择框
        plot->legend->setVisible(false);

        // 1. 开启交互权限：允许缩放、选中曲线
        plot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables);

        // 2. 设置框选模式：自定义（用于统计）+ 缩放（用于放大）
        plot->setSelectionRectMode(QCP::srmCustom);
        
        // 连接框选完成信号，用于关闭自动跟随
        connect(plot->selectionRect(), &QCPSelectionRect::accepted,
                this, [this]() {
                    // 框选放大时关闭自动跟随
                    m_autoFollow = false;
                });
        
        // 3. 安装事件过滤器，用于处理中键拖拽、Ctrl+滚轮等自定义交互
        plot->installEventFilter(this);

        // 4. 连接信号与槽
        //    (1) 选中状态改变 -> 触发高亮逻辑（线宽变粗）
        connect(plot, &QCustomPlot::selectionChangedByUser,
                this, &WaveformWidget::onSelectionChanged);

        //    (2) 点击曲线 -> 触发数值显示逻辑（Tooltip）
        connect(plot, &QCustomPlot::plottableClick,
                this, &WaveformWidget::onPlottableClick);

        //    (3) 双击曲线 -> 触发颜色修改逻辑（弹出颜色选择对话框）
        connect(plot, &QCustomPlot::plottableDoubleClick,
                this, &WaveformWidget::onPlottableDoubleClicked);

        //    (4) 框选完成 -> 触发统计逻辑（显示区域统计信息）
        connect(plot->selectionRect(), &QCPSelectionRect::accepted,
                this, &WaveformWidget::onSelectionRectAccepted);
    };

    configPlot(ui->plotVoltage);
    configPlot(ui->plotCurrent);
    // ==========================================
    // 1. 配置上方的图表：电压（为每个通道创建graph）
    // ==========================================
    // 清空旧 graph，重新为多通道创建
    ui->plotVoltage->clearGraphs();

    // 为每个通道创建一条电压曲线
    for (int ch = 0; ch < kChannelCount; ++ch) {
        ui->plotVoltage->addGraph();

        // 配一组不同颜色
        static const QColor voltageColors[] = {
            Qt::blue, Qt::darkBlue, Qt::red, Qt::darkRed, Qt::green,
            Qt::darkGreen, Qt::magenta, Qt::darkMagenta, Qt::cyan, Qt::darkCyan
        };
        static const int voltageColorsSize = sizeof(voltageColors) / sizeof(voltageColors[0]);
        QColor c = voltageColors[ch % voltageColorsSize];
        ui->plotVoltage->graph(ch)->setPen(QPen(c));

        ui->plotVoltage->graph(ch)->setName(QStringLiteral("Voltage%1 (V)").arg(ch + 1));
    }

    ui->plotVoltage->yAxis->setLabel("Voltage (V)");


    // ==========================================
    // 2. 配置下方的图表：电流 & 功率（为每个通道创建graph）
    // ==========================================

    ui->plotCurrent->clearGraphs();

    // --- 左侧 Y 轴：电流（每个通道一条曲线） ---
    for (int ch = 0; ch < kChannelCount; ++ch) {
        ui->plotCurrent->addGraph(ui->plotCurrent->xAxis, ui->plotCurrent->yAxis);

        static const QColor currentColors[] = {
            Qt::red, Qt::darkRed, Qt::green, Qt::darkGreen, Qt::blue,
            Qt::darkBlue, Qt::magenta, Qt::darkMagenta, Qt::cyan, Qt::darkCyan
        };
        static const int currentColorsSize = sizeof(currentColors) / sizeof(currentColors[0]);
        QColor c = currentColors[ch % currentColorsSize];
        ui->plotCurrent->graph(ch)->setPen(QPen(c));
        ui->plotCurrent->graph(ch)->setName(QStringLiteral("Current%1 (A)").arg(ch + 1));
    }
    ui->plotCurrent->yAxis->setLabel("Current (A)");

    // --- 右侧 Y 轴 (yAxis2)：功率（每个通道一条曲线） ---
    ui->plotCurrent->yAxis2->setVisible(true);
    for (int ch = 0; ch < kChannelCount; ++ch) {
        ui->plotCurrent->addGraph(ui->plotCurrent->xAxis, ui->plotCurrent->yAxis2);
        int graphIndex = kChannelCount + ch;

        static const QColor powerColors[] = {
            Qt::darkYellow, QColor(160, 120, 0), QColor(200, 80, 0),
            QColor(120, 80, 40), QColor(180, 160, 0),
            QColor(100, 100, 0), QColor(160, 160, 40),
            QColor(200, 200, 0), QColor(150, 120, 60),
            QColor(190, 140, 40)
        };
        static const int powerColorsSize = sizeof(powerColors) / sizeof(powerColors[0]);
        QColor c = powerColors[ch % powerColorsSize];
        ui->plotCurrent->graph(graphIndex)->setPen(QPen(c));
        ui->plotCurrent->graph(graphIndex)->setName(QStringLiteral("Power%1 (W)").arg(ch + 1));
    }
    ui->plotCurrent->yAxis2->setLabel("Power (W)");

    // 让上下两个图左右边距一致（包含下图右侧的 Power 轴），保证绘图区对齐
    {

       //ui->plotVoltage 是作为 Parent（父对象）传入的
       auto *groupLeft=new QCPMarginGroup(ui->plotVoltage);
       auto *groupRight=new QCPMarginGroup(ui->plotVoltage);

        ui->plotVoltage->axisRect()->setMarginGroup(QCP::msLeft, groupLeft);
        ui->plotCurrent->axisRect()->setMarginGroup(QCP::msLeft, groupLeft);
        ui->plotVoltage->axisRect()->setMarginGroup(QCP::msRight, groupRight);
        ui->plotCurrent->axisRect()->setMarginGroup(QCP::msRight, groupRight);
    }


    // ==========================================
    // 3. 设置 X 轴联动
    // ==========================================
    // 使用槽函数 + QSignalBlocker，避免双向 setRange 导致递归/抖动
            connect(ui->plotVoltage->xAxis,
            static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged),
            this,
            [this](const QCPRange &range){
                // 防环锁检测
                if (m_syncingRange) return;

                // 更新视图宽度（用于自动跟随时计算范围）
                m_viewWidth = range.size();
                
                // 获取锁
                m_syncingRange = true;

                // 进行同步
                QSignalBlocker b(ui->plotCurrent->xAxis);
                ui->plotCurrent->xAxis->setRange(range);

                // 释放锁
                m_syncingRange = false;
            });

    connect(ui->plotCurrent->xAxis,
            static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged),
            this,
            [this](const QCPRange &range){
                if (m_syncingRange) return;
                // 更新视图宽度
                m_viewWidth = range.size();    
                m_syncingRange = true;
                QSignalBlocker b(ui->plotVoltage->xAxis);
                ui->plotVoltage->xAxis->setRange(range);
                m_syncingRange = false;
            });
}

/**
 * @brief 添加新的数据点并更新显示
 * @param time 时间戳（秒）
 * @param voltage 电压值（V）
 * @param current 电流值（A）
 * @param power 功率值（W）
 * 
 * 处理流程：
 * 1. 保存原始数据到内存（不做删除/降采样，保证数据完整性）
 * 2. 自动滚动视图（显示最新 10 秒的数据）
 * 3. 视觉降采样（减少绘制点数，提高性能，但不修改原始数据）
 * 4. 触发重绘（使用队列重绘，避免阻塞 UI 线程）
 */
void WaveformWidget::addData(double time, double voltage, double current, double power)
{
    // 1) 严格保存原始数据（后台记录），不做删除/降采样
    m_time.push_back(time);
    m_voltage.push_back(voltage);
    m_current.push_back(current);
    m_power.push_back(power);

    // 2) 视图自动滚动（只改显示范围，不影响数据记录）
    const double showRange = (m_viewWidth > 0.0 ? m_viewWidth : 10.0);
    if (m_autoFollow) {
        m_isAutoFollowing = true;  // 标记正在执行自动跟随
        ui->plotVoltage->xAxis->setRange(time,showRange,Qt::AlignRight);
        m_isAutoFollowing = false;  // 清除标记
    }

    // 3) 视觉降采样绘制（只减少绘制点数，严禁修改原始数据）
    applyVisualDownsample(ui->plotVoltage);
    applyVisualDownsample(ui->plotCurrent);

    // 4) 触发重绘（队列重绘，避免阻塞 UI）
    ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
    ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
}

/**
 * @brief 添加多通道数据（线程安全，支持从工作线程调用）
 * @param data 多通道数据包
 * 
 * 处理流程：
 * 1. 遍历数据包中的所有通道
 * 2. 为每个通道保存数据到独立的数据池
 * 3. 自动计算功率（如果未提供）
 * 4. 更新视图和重绘
 */
void WaveformWidget::addChannelData(const MultiChannelData &data)
{
    if (data.channelData.isEmpty()) {
        return;
    }
    
    double maxTime = 0.0;  // 记录最大时间，用于视图跟随
    
    // 遍历所有通道数据
    for (auto it = data.channelData.constBegin(); it != data.channelData.constEnd(); ++it) {
        int channelId = it.key();
        
        // 验证通道ID有效性
        if (channelId < 0 || channelId >= kChannelCount) {
            continue;
        }
        
        const ChannelDataPoint &point = it.value();
        
        // 计算功率（如果未提供或为NaN）
        double power = point.power;
        if (std::isnan(power) || std::isinf(power) || power == 0.0) {
            power = point.voltage * point.current;
        }
        
        // 获取或创建通道数据存储
        ChannelData &channelData = m_channelDataMap[channelId];
        
        // 添加数据点
        channelData.time.push_back(point.time);
        channelData.voltage.push_back(point.voltage);
        channelData.current.push_back(point.current);
        channelData.power.push_back(power);
        
        // 更新最大时间
        if (point.time > maxTime) {
            maxTime = point.time;
        }
        
        // 如果是通道0，同时更新单通道数据（
        if (channelId == 0) {
            m_time.push_back(point.time);
            m_voltage.push_back(point.voltage);
            m_current.push_back(point.current);
            m_power.push_back(power);
        }
    }
    
    // 视图自动滚动
    if (m_autoFollow && maxTime > 0) {
        m_isAutoFollowing = true;  // 标记正在执行自动跟随
        const double showRange = (m_viewWidth > 0.0 ? m_viewWidth : 10.0);
        ui->plotVoltage->xAxis->setRange(maxTime, showRange, Qt::AlignRight);
        m_isAutoFollowing = false;  // 清除标记
    }
    
    // 更新图表显示（多通道数据）
    updateChannelGraphs();
    
    // 触发重绘
    ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
    ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
}

/**
 * @brief 清空所有数据（原始数据 + 图表显示）
 * 
 * 功能：
 * 1. 清空内存中的原始数据（m_time, m_voltage, m_current, m_power）
 * 2. 清空图表中的绘制数据
 * 3. 立即刷新图表显示空白状态
 */
void WaveformWidget::clear()
{
    // 清空原始数据
    m_time.clear();
    m_voltage.clear();
    m_current.clear();
    m_power.clear();

    // 清空图表绘制数据
    ui->plotVoltage->graph(0)->data()->clear();
    ui->plotCurrent->graph(0)->data()->clear();
    ui->plotCurrent->graph(1)->data()->clear();

    // 立即刷新图表以显示空白状态（同步重绘，确保立即生效）
    ui->plotVoltage->replot();
    ui->plotCurrent->replot();
}

/**
 * @brief 清空指定通道的数据
 * @param channelId 通道ID（0 到 kChannelCount-1）
 */
void WaveformWidget::clearChannel(int channelId)
{
    if (channelId < 0 || channelId >= kChannelCount) {
        return;
    }
    
    // 清空通道数据
    if (m_channelDataMap.contains(channelId)) {
        m_channelDataMap[channelId].time.clear();
        m_channelDataMap[channelId].voltage.clear();
        m_channelDataMap[channelId].current.clear();
        m_channelDataMap[channelId].power.clear();
    }
    
    // 如果是通道0，同时清空单通道数据（向后兼容）
    if (channelId == 0) {
        m_time.clear();
        m_voltage.clear();
        m_current.clear();
        m_power.clear();
    }
    
    // 清空对应的graph数据
    if (ui->plotVoltage && channelId < ui->plotVoltage->graphCount()) {
        ui->plotVoltage->graph(channelId)->data()->clear();
    }
    
    if (ui->plotCurrent) {
        // 清空电流graph
        if (channelId < ui->plotCurrent->graphCount()) {
            ui->plotCurrent->graph(channelId)->data()->clear();
        }
        // 清空功率graph
        int powerGraphIndex = kChannelCount + channelId;
        if (powerGraphIndex < ui->plotCurrent->graphCount()) {
            ui->plotCurrent->graph(powerGraphIndex)->data()->clear();
        }
    }
    
    // 触发重绘
    if (ui->plotVoltage) {
        ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
    }
    if (ui->plotCurrent) {
        ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
    }
}

/**
 * @brief 设置测量工具模式
 * @param mode 测量模式：Off（关闭）、Crosshair（十字光标）、Calipers（双线卡尺）
 * 
 * 功能：
 * 1. 如果关闭模式：清除所有测量工具 items
 * 2. 如果开启模式：创建测量工具 items，并初始化默认位置（可视范围的 1/3 和 2/3）
 * 3. 更新显示文本（ΔT、ΔV、ΔA）
 */
void WaveformWidget::setMeasureToolMode(MeasureToolMode mode)
{
    m_measureMode = mode;

    // 关闭模式：清除所有测量工具
    if (m_measureMode == MeasureToolMode::Off) {
        clearMeasureItems();
        if (ui->plotVoltage) ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
        if (ui->plotCurrent) ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    // 确保测量工具 items 已创建
    ensureMeasureItems();

    // 初始化默认位置（放在当前可视范围的 1/3 与 2/3 处）
    if (ui->plotVoltage) {
        const auto xr = ui->plotVoltage->xAxis->range();
        m_caliperX1 = xr.lower + xr.size() * 0.33;  // 第一条竖线位置
        m_caliperX2 = xr.lower + xr.size() * 0.66;  // 第二条竖线位置
        const auto yrV = ui->plotVoltage->yAxis->range();
        m_caliperY1V = yrV.lower + yrV.size() * 0.33;  // 第一条横线位置（电压图）
        m_caliperY2V = yrV.lower + yrV.size() * 0.66;  // 第二条横线位置（电压图）
    }
    if (ui->plotCurrent) {
        const auto yrI = ui->plotCurrent->yAxis->range();
        m_caliperY1I = yrI.lower + yrI.size() * 0.33;  // 第一条横线位置（电流图）
        m_caliperY2I = yrI.lower + yrI.size() * 0.66;  // 第二条横线位置（电流图）
    }

    // 更新显示文本并重绘
    updateCalipersText();
    if (ui->plotVoltage) ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
    if (ui->plotCurrent) ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
}

/**
 * @brief 循环切换测量工具模式
 * 
 * 切换顺序：Off -> Crosshair -> Calipers -> Off
 * 用于主界面"卡尺"按钮的点击事件
 */
void WaveformWidget::cycleMeasureToolMode()
{
    switch (m_measureMode) {
    case MeasureToolMode::Off: setMeasureToolMode(MeasureToolMode::Crosshair); break;
    case MeasureToolMode::Crosshair: setMeasureToolMode(MeasureToolMode::Calipers); break;
    case MeasureToolMode::Calipers: setMeasureToolMode(MeasureToolMode::Off); break;
    }
}

void WaveformWidget::setVoltageVisible(bool visible)
{
    // 1. 更新内部状态变量
    // 记录当前电压曲线是否应该被显示
    m_voltageVisible = visible;

    // 2. 检查对象有效性
    if (ui->plotVoltage && ui->plotVoltage->graphCount() > 0) {

        // 3. 设置图形可见性隐藏
        ui->plotVoltage->graph(0)->setVisible(visible);

        // 4. 触发重绘
        // 由于曲线的显示状态发生了变化，必须重新绘制图表
        // 使用 rpQueuedReplot（异步重绘）以保证界面响应的丝滑度
        ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
    }
}


//显示电流
void WaveformWidget::setCurrentVisible(bool visible)
{
    m_currentVisible = visible;
    if (ui->plotCurrent && ui->plotCurrent->graphCount() > 0) {
        ui->plotCurrent->graph(0)->setVisible(visible);
        ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
    }
}

//显示功率
void WaveformWidget::setPowerVisible(bool visible)
{
    m_powerVisible = visible;
    if (ui->plotCurrent && ui->plotCurrent->graphCount() > kChannelCount) {
        // 索引 [kChannelCount, ...) 为功率曲线
        for (int i = kChannelCount; i < ui->plotCurrent->graphCount(); ++i) {
            ui->plotCurrent->graph(i)->setVisible(visible);
        }
        ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
    }
}

/**
 * @brief 设置指定通道的显示状态
 * @param channelId 通道ID（0 到 kChannelCount-1）
 * @param voltageVisible 电压曲线是否显示
 * @param currentVisible 电流曲线是否显示
 * @param powerVisible 功率曲线是否显示
 */
void WaveformWidget::setChannelVisible(int channelId, bool voltageVisible, 
                                        bool currentVisible, bool powerVisible)
{
    if (channelId < 0 || channelId >= kChannelCount) {
        return;
    }
    
    // 更新通道显示状态
    ChannelVisibility &vis = m_channelVisibility[channelId];
    vis.voltageVisible = voltageVisible;
    vis.currentVisible = currentVisible;
    vis.powerVisible = powerVisible;
    
    // 更新电压graph显示状态（需要同时满足全局和通道设置）
    if (ui->plotVoltage && channelId < ui->plotVoltage->graphCount()) {
        ui->plotVoltage->graph(channelId)->setVisible(voltageVisible && m_voltageVisible);
    }
    
    // 更新电流graph显示状态
    if (ui->plotCurrent && channelId < ui->plotCurrent->graphCount()) {
        ui->plotCurrent->graph(channelId)->setVisible(currentVisible && m_currentVisible);
    }
    
    // 更新功率graph显示状态
    int powerGraphIndex = kChannelCount + channelId;
    if (ui->plotCurrent && powerGraphIndex < ui->plotCurrent->graphCount()) {
        ui->plotCurrent->graph(powerGraphIndex)->setVisible(powerVisible && m_powerVisible);
    }
    
    // 触发重绘
    if (ui->plotVoltage) {
        ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
    }
    if (ui->plotCurrent) {
        ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);
    }
}


// 初始化和配置所有的测量辅助元素（十字光标和卡尺线）
void WaveformWidget::ensureMeasureItems()
{
    // 1. 安全检查：如果绘图控件还没初始化，直接退出
    if (!ui->plotVoltage || !ui->plotCurrent) return;

    // --- Lambda 工具函数定义 ---
    // 创建垂直线 (Vertical Line) 的辅助函数
    auto makeVLine = [](QCustomPlot *plot, QCPItemStraightLine* &line, const QColor &c) {
        if (line) return; // 如果对象已存在，不再重复创建
        line = new QCPItemStraightLine(plot);
        line->setPen(QPen(c, 1, Qt::DashLine)); // 设置画笔：颜色c，宽度1，虚线(DashLine)

        // 设置坐标类型为“绘图坐标”（即根据电压或时间数值定位）
        line->point1->setType(QCPItemPosition::ptPlotCoords);
        line->point2->setType(QCPItemPosition::ptPlotCoords);

        // 设置垂直线
        line->point1->setCoords(0, 0); // 初始点1
        line->point2->setCoords(0, 1); // 垂直线：X相同，Y不同
    };

    // 创建水平线 (Horizontal Line) 的辅助函数
    auto makeHLine = [](QCustomPlot *plot, QCPItemStraightLine* &line, const QColor &c) {
        if (line) return;
        line = new QCPItemStraightLine(plot);
        line->setPen(QPen(c, 1, Qt::DashLine));
        line->point1->setType(QCPItemPosition::ptPlotCoords);
        line->point2->setType(QCPItemPosition::ptPlotCoords);
        line->point1->setCoords(0, 0); // 初始点1
        line->point2->setCoords(1, 0); // 水平线：Y相同，X不同
    };

    // 创建文本框 (Text Label) 的辅助函数（用于显示 Delta T 或 Delta V）
    auto makeText = [](QCustomPlot *plot, QCPItemText* &txt) {
        if (txt) return;
        txt = new QCPItemText(plot);
        // 设置文本对齐方式：左对齐，顶部对齐
        txt->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
        // 设置坐标类型为“比例坐标”（0.0~1.0），这样文本会固定在屏幕角落，不随波形滚动
        txt->position->setType(QCPItemPosition::ptAxisRectRatio);
        txt->position->setCoords(0.01, 0.01); // 放在左上角 1% 的偏移处
        txt->setBrush(QColor(255, 255, 255, 220)); // 半透明白色背景
        txt->setPen(QPen(Qt::gray));               // 灰色边框
        txt->setPadding(QMargins(6, 4, 6, 4));     // 文字内边距
        QFont f = plot->font();
        f.setPointSize(9);
        txt->setFont(f);
    };

    // --- 开始实际创建对象 ---

    // 2. 创建【十字光标 (Crosshair)】对象
    // 电压图的横竖线及坐标文字
    makeVLine(ui->plotVoltage, m_crossV_V, QColor(80, 80, 80)); // 深灰色
    makeHLine(ui->plotVoltage, m_crossH_V, QColor(80, 80, 80));
    makeText(ui->plotVoltage, m_crossText_V);

    // 电流图的横竖线及坐标文字
    makeVLine(ui->plotCurrent, m_crossV_I, QColor(80, 80, 80));
    makeHLine(ui->plotCurrent, m_crossH_I, QColor(80, 80, 80));
    makeText(ui->plotCurrent, m_crossText_I);

    // 3. 创建【测量卡尺 (Calipers)】对象
    // 垂直方向卡尺线（测时间差 Delta T）
    makeVLine(ui->plotVoltage, m_vline1_V, QColor(0, 120, 215)); // 蓝色
    makeVLine(ui->plotVoltage, m_vline2_V, QColor(0, 120, 215));
    makeVLine(ui->plotCurrent, m_vline1_I, QColor(0, 120, 215));
    makeVLine(ui->plotCurrent, m_vline2_I, QColor(0, 120, 215));

    // 水平方向卡尺线（测电压差 Delta V 或电流差 Delta I）
    makeHLine(ui->plotVoltage, m_hline1_V, QColor(0, 120, 215));
    makeHLine(ui->plotVoltage, m_hline2_V, QColor(0, 120, 215));
    makeHLine(ui->plotCurrent, m_hline1_I, QColor(0, 120, 215));
    makeHLine(ui->plotCurrent, m_hline2_I, QColor(0, 120, 215));

    // 测量值结果显示的文本框
    makeText(ui->plotVoltage, m_caliperText_V);
    makeText(ui->plotCurrent, m_caliperText_I);
}

void WaveformWidget::clearMeasureItems()
{
    // 1. 定义 Lambda 工具函数：清空特定图表的所有“项” (Items)
    auto clearPlotItems = [](QCustomPlot *plot) {
        if (!plot) return;

        // clearItems() 会销毁该图表上所有的 QCPItem（如直线、文本、矩形等）
        plot->clearItems();
    };

    // 2. 执行清空动作
    // 分别清空电压图和电流图上的所有图形项
    clearPlotItems(ui->plotVoltage);
    clearPlotItems(ui->plotCurrent);

    // 3. 指针置空 (Nullptr Reset)
    // 防止类成员变量（指针）依然指向已经失效的地址（野指针）。
    // 重置十字光标 (Crosshair) 的指针
    m_crossV_V = m_crossH_V = nullptr;
    m_crossText_V = nullptr;
    m_crossV_I = m_crossH_I = nullptr;
    m_crossText_I = nullptr;

    // 重置卡尺 (Calipers) 的垂直线指针
    m_vline1_V = m_vline2_V = nullptr;
    m_vline1_I = m_vline2_I = nullptr;

    // 重置卡尺的水平线指针
    m_hline1_V = m_hline2_V = nullptr;
    m_hline1_I = m_hline2_I = nullptr;

    // 重置测量结果文本框指针
    m_caliperText_V = nullptr;
    m_caliperText_I = nullptr;
}

void WaveformWidget::updateCrosshair(QCustomPlot *plot, const QPoint &pos)
{
    // 1. 安全检查：确保绘图控件有效
    if (!plot) return;

    // 2. 坐标转换：将鼠标当前的【像素位置】转换为【物理坐标】（时间x 和 数值y）
    const double x = plot->xAxis->pixelToCoord(pos.x());
    const double y = plot->yAxis->pixelToCoord(pos.y());

    // 3. 区分当前操作的是“电压图”还是“电流图”
    if (plot == ui->plotVoltage) {
        // --- 处理电压图的十字光标 ---
        // 检查十字光标的对象（线和文本）是否已创建
        if (!m_crossV_V || !m_crossH_V || !m_crossText_V) return;

        // 更新垂直线位置：锁定在时间 x 处
        m_crossV_V->point1->setCoords(x, 0);
        m_crossV_V->point2->setCoords(x, 1);

        // 更新水平线位置：锁定在电压 y 处
        m_crossH_V->point1->setCoords(0, y);
        m_crossH_V->point2->setCoords(1, y);

        // 更新标签文字：显示 6 位小数的精确时间和电压
        // arg 参数说明: (数值, 字段宽度, 格式'f', 小数位数6)
        m_crossText_V->setText(QString("T: %1 s\nV: %2").arg(x, 0, 'f', 6).arg(y, 0, 'f', 6));

    } else {
        // --- 处理电流图的十字光标 ---
        if (!m_crossV_I || !m_crossH_I || !m_crossText_I) return;

        // 更新垂直线（时间）
        m_crossV_I->point1->setCoords(x, 0);
        m_crossV_I->point2->setCoords(x, 1);

        // 更新水平线（电流）
        m_crossH_I->point1->setCoords(0, y);
        m_crossH_I->point2->setCoords(1, y);

        // 更新标签文字：显示时间和电流
        m_crossText_I->setText(QString("T: %1 s\nI: %2").arg(x, 0, 'f', 6).arg(y, 0, 'f', 6));
    }
}
// 渲染与状态管理器
void WaveformWidget::updateCalipersText()
{
    // 1. 安全检查
    if (!ui->plotVoltage || !ui->plotCurrent) return;

    // --- 垂直卡尺同步 (时间轴 X) ---
    const double x1 = m_caliperX1;
    const double x2 = m_caliperX2;
    const double dt = qAbs(x2 - x1); // 计算两根垂直线之间的时间差 (Delta T)

    // 同步电压图的垂直卡尺线位置
    if (m_vline1_V && m_vline2_V) {
        m_vline1_V->point1->setCoords(x1, 0);
        m_vline1_V->point2->setCoords(x1, 1);

        m_vline2_V->point1->setCoords(x2, 0);
        m_vline2_V->point2->setCoords(x2, 1);
    }
    // 同步电流图的垂直卡尺线位置 (时间通常是跨图表同步的)
    if (m_vline1_I && m_vline2_I) {
        m_vline1_I->point1->setCoords(x1, 0);
        m_vline1_I->point2->setCoords(x1, 1);

        m_vline2_I->point1->setCoords(x2, 0);
        m_vline2_I->point2->setCoords(x2, 1);
    }

    // --- 电压图水平卡尺同步 (Y轴) ---
    const double dv = qAbs(m_caliperY2V - m_caliperY1V); // 计算电压差 (Delta V)
    if (m_hline1_V && m_hline2_V) {
        m_hline1_V->point1->setCoords(0, m_caliperY1V);
        m_hline1_V->point2->setCoords(1, m_caliperY1V);

        m_hline2_V->point1->setCoords(0, m_caliperY2V);
        m_hline2_V->point2->setCoords(1, m_caliperY2V);
    }
    // 更新电压图左上角的测量结果文本
    if (m_caliperText_V) {
        m_caliperText_V->setText(QString("ΔT: %1 s\nΔV: %2")
                                     .arg(dt, 0, 'f', 6)
                                     .arg(dv, 0, 'f', 6));
    }

    // --- 电流图水平卡尺同步 (Y轴) ---
    const double di = qAbs(m_caliperY2I - m_caliperY1I); // 计算电流差 (Delta I)
    if (m_hline1_I && m_hline2_I) {
        m_hline1_I->point1->setCoords(0, m_caliperY1I);
        m_hline1_I->point2->setCoords(1, m_caliperY1I);
        m_hline2_I->point1->setCoords(0, m_caliperY2I);
        m_hline2_I->point2->setCoords(1, m_caliperY2I);
    }
    // 更新电流图左上角的测量结果文本
    if (m_caliperText_I) {
        m_caliperText_I->setText(QString("ΔT: %1 s\nΔA: %2")
                                     .arg(dt, 0, 'f', 6)
                                     .arg(di, 0, 'f', 6));
    }

    // --- 状态控制 (显示/隐藏逻辑) ---
    // 判断当前处于哪种模式
    const bool cross = (m_measureMode == MeasureToolMode::Crosshair); // 十字光标模式
    const bool cal = (m_measureMode == MeasureToolMode::Calipers);   // 卡尺测量模式

    // 定义两个小工具 Lambda，简化显隐设置代码
    auto setVisible = [](QCPItemStraightLine *l, bool v){ if (l) l->setVisible(v); };
    auto setTxtVisible = [](QCPItemText *t, bool v){ if (t) t->setVisible(v); };

    // 根据模式开关控制【十字光标】所有元素的可见性
    setVisible(m_crossV_V, cross);
    setVisible(m_crossH_V, cross);
    setTxtVisible(m_crossText_V, cross);
    setVisible(m_crossV_I, cross);
    setVisible(m_crossH_I, cross);
    setTxtVisible(m_crossText_I, cross);

    // 根据模式开关控制【测量卡尺】所有元素的可见性
    setVisible(m_vline1_V, cal);
    setVisible(m_vline2_V, cal);
    setVisible(m_vline1_I, cal);
    setVisible(m_vline2_I, cal);
    setVisible(m_hline1_V, cal);
    setVisible(m_hline2_V, cal);
    setVisible(m_hline1_I, cal);
    setVisible(m_hline2_I, cal);
    setTxtVisible(m_caliperText_V, cal);
    setTxtVisible(m_caliperText_I, cal);
}

//选择拖拽目标
bool WaveformWidget::handleMeasureMousePress(QCustomPlot *plot, QMouseEvent *e)
{
    // 1. 基础安全检查
    if (!plot) return false;
    // 只响应鼠标左键按下
    if (e->button() != Qt::LeftButton) return false;
    // 只有在“卡尺测量模式”下才执行此逻辑
    if (m_measureMode != MeasureToolMode::Calipers) return false;

    // 2. 定义碰撞检测阈值（单位：像素）
    // 如果鼠标距离线小于 6 像素，就认为“点中了”
    const int thresholdPx = 6;
    const int mx = e->pos().x(); // 获取鼠标当前位置的像素 X 坐标
    const int my = e->pos().y(); // 获取鼠标当前位置的像素 Y 坐标

    // 3. 将逻辑坐标（数值）转为像素坐标，并计算与鼠标的绝对距离
    auto distToX = [&](double xCoord) {
        const int px = plot->xAxis->coordToPixel(xCoord);
        return std::abs(px - mx);
    };
    auto distToY = [&](double yCoord) {
        const int py = plot->yAxis->coordToPixel(yCoord);
        return std::abs(py - my);
    };

    // 4. 重置当前拖拽目标
    m_dragTarget = DragTarget::None;

    // 5. 检测是否点中了垂直卡尺 (X 轴位置，测量时间)
    const int dx1 = distToX(m_caliperX1); // 计算鼠标到第一根垂直线的距离
    const int dx2 = distToX(m_caliperX2); // 计算鼠标到第二根垂直线的距离
    const int bestX = std::min(dx1, dx2); // 取最近的一根

    if (bestX <= thresholdPx) {
        // 如果点中了，判断离哪根更近，并设置为当前的拖拽目标
        m_dragTarget = (dx1 <= dx2) ? DragTarget::X1 : DragTarget::X2;
        return true; // 返回 true 表示事件已处理，拦截后续操作
    }

    // 6. 检测是否点中了水平卡尺 (Y 轴位置，测量电压/电流)
    // 根据当前操作的是“电压图”还是“电流图”分别检测对应的 Y 卡尺
    if (plot == ui->plotVoltage) {
        // 电压图卡尺检测
        const int dy1 = distToY(m_caliperY1V);
        const int dy2 = distToY(m_caliperY2V);
        const int bestY = std::min(dy1, dy2);
        if (bestY <= thresholdPx) {
            m_dragTarget = (dy1 <= dy2) ? DragTarget::Y1V : DragTarget::Y2V;
            return true;
        }
    } else {
        // 电流/功率图卡尺检测
        const int dy1 = distToY(m_caliperY1I);
        const int dy2 = distToY(m_caliperY2I);
        const int bestY = std::min(dy1, dy2);
        if (bestY <= thresholdPx) {
            m_dragTarget = (dy1 <= dy2) ? DragTarget::Y1I : DragTarget::Y2I;
            return true;
        }
    }

    // 如果没有任何线被点中，返回 false，允许 QCustomPlot 执行默认操作（如矩形缩放）
    return false;
}

// 当鼠标移动时，根据之前在 Press 函数中锁定的目标，实时更新卡尺的位置。
bool WaveformWidget::handleMeasureMouseMove(QCustomPlot *plot, QMouseEvent *e)
{
    // 1. 安全检查
    if (!plot) return false;

    // 2. 情况 A：十字光标模式 (Crosshair)
    // 这种模式下，光标只是跟随鼠标移动显示当前坐标，不涉及“拖拽”
    if (m_measureMode == MeasureToolMode::Crosshair) {
        updateCrosshair(plot, e->pos()); // 更新十字线的位置信息
        // 使用 rpQueuedReplot 异步刷新，避免高频刷新导致界面卡顿
        plot->replot(QCustomPlot::rpQueuedReplot);
        return false; // 返回 false，让图表原有的交互（如坐标值显示）也能工作
    }

    // 3. 情况 B：卡尺模式 (Calipers)
    // 如果不是卡尺模式，或者当前没有选中任何拖拽目标，则直接退出
    if (m_measureMode != MeasureToolMode::Calipers) return false;
    if (m_dragTarget == DragTarget::None) return false;

    // pixelToCoord：将鼠标当前的像素位置翻译成真实的物理数值（时间或电压/电流）
    const double x = plot->xAxis->pixelToCoord(e->pos().x());
    const double y = plot->yAxis->pixelToCoord(e->pos().y());

    // 5. 根据之前“按住”的目标，更新对应的变量
    switch (m_dragTarget) {
    case DragTarget::X1:  m_caliperX1 = x;  break; // 移动第一根垂直线（时间1）
    case DragTarget::X2:  m_caliperX2 = x;  break; // 移动第二根垂直线（时间2）
    case DragTarget::Y1V: m_caliperY1V = y; break; // 移动电压图的第一根水平线
    case DragTarget::Y2V: m_caliperY2V = y; break; // 移动电压图的第二根水平线
    case DragTarget::Y1I: m_caliperY1I = y; break; // 移动电流图的第一根水平线
    case DragTarget::Y2I: m_caliperY2I = y; break; // 移动电流图的第二根水平线
    default: break;
    }

    // 6. 界面反馈
    updateCalipersText(); // 实时计算 delta T 或 delta V，并更新到界面标签上

    // 7. 同步刷新所有图表
    // 即使你只动了电压图的线，时间卡尺（X）通常是两个图表同步的，所以都要重绘
    ui->plotVoltage->replot(QCustomPlot::rpQueuedReplot);
    ui->plotCurrent->replot(QCustomPlot::rpQueuedReplot);

    // 返回 true，表示当前正在拖拽卡尺，拦截图表的默认行为（如框选放大）
    return true;
}

bool WaveformWidget::handleMeasureMouseRelease(QCustomPlot *plot, QMouseEvent *e)
{
    // 1. 抑制编译器警告
    Q_UNUSED(plot);

    // 2. 状态检查
    // 如果当前不是卡尺模式，或者松开的不是左键，则不处理
    if (m_measureMode != MeasureToolMode::Calipers) return false;
    if (e->button() != Qt::LeftButton) return false;

    // 3. 核心动作：重置拖拽目标
    // 如果当前没有任何拖拽目标，返回 false 让系统执行默认逻辑
    if (m_dragTarget == DragTarget::None) return false;

    // 将拖拽目标设为 None
    m_dragTarget = DragTarget::None;

    // 4. 事件拦截
    // 返回 true 表示这个松开动作已经被测量工具“消费”了，防止触发图表的其他默认松开行为
    return true;
}


/**
 * @brief 视觉降采样：根据可视范围和像素宽度，智能减少绘制点数
 * @param plot 目标图表（plotVoltage 或 plotCurrent）
 * 
 * 算法原理：
 * 1. 将可视时间范围按像素宽度分成多个 bin（每个 bin 对应一列像素）
 * 2. 对每个 bin 内的数据点，提取最小值点和最大值点（Min-Max 抽样）
 * 3. 这样既能保证波形形状不丢失，又能大幅减少绘制点数（从数千点降到数百点）
 */
void WaveformWidget::applyVisualDownsample(QCustomPlot *plot)
{
    if (!plot) return;
    // 没数据就不处理
    if (m_time.isEmpty()) return;

    // 1. 计算可视范围和像素宽度
    const QCPRange xr = plot->xAxis->range();  // 当前 X 轴（时间）范围
    const int w = std::max(50, plot->axisRect()->rect().width());  // 绘图区域宽度（像素）
    const double bin = xr.size() / w;  // 每个像素对应的时间跨度
    if (bin <= 0) return;

    // 2. 使用二分查找定位可视范围内的数据点索引范围
    auto findStartIdx = [&](double t) {
        auto it = std::lower_bound(m_time.begin(), m_time.end(), t);
        return int(it - m_time.begin());
    };
    int i0 = std::max(0, findStartIdx(xr.lower));  // 可视范围起始索引
    // 使用 upper_bound 确保包含等于 xr.upper 的边界点
    auto itEnd = std::upper_bound(m_time.begin() + i0, m_time.end(), xr.upper);
    int i1 = std::min(int(m_time.size()), int(itEnd - m_time.begin()));  // 可视范围结束索引
    
    // 3. 如果数据点很少（≤2个），直接全量显示，不做降采样
    if (i1 - i0 <= 2) {
        if (plot == ui->plotVoltage) {
            plot->graph(0)->setData(m_time, m_voltage, true);
        } else {
            plot->graph(0)->setData(m_time, m_current, true);
            plot->graph(1)->setData(m_time, m_power, true);
        }
        return;
    }

    // 4. Min-Max 降采样算法：对每个像素列，提取该列内的最小值和最大值
    // y: 原始电压/电流数组 (y)
    // outX, outY: 存储降采样后用于绘图的数据点 (x, y)
    auto downsampleSeries = [&](const QVector<double> &y, QVector<double> &outX, QVector<double> &outY) {
        // 1. 清空旧数据并预分配内存
        outX.clear(); outY.clear();

        //存储大小由屏幕实际的分辨率显示
        outX.reserve(w * 2); // w 是屏幕宽度(像素)，每列提 2 个点，所以预留 w*2
        outY.reserve(w * 2);

        double tBinStart = xr.lower; // 当前像素“桶”的时间起始位置
        int idx = i0;                // 从可视范围的起始索引 i0 开始处理

        // 2. 外层循环：遍历屏幕上的每一个像素列 (px)
        // 注意：即使 idx >= i1，也要处理完所有像素列，确保边界数据不丢失
        for (int px = 0; px < w; ++px) {
            // 计算当前像素列对应的结束时间
            // 如果是最后一列，强制等于视图最右边缘；否则按 bin (像素时间跨度) 递增
            const double tBinEnd = (px == w - 1) ? xr.upper : (tBinStart + bin);

            bool has = false;             // 标记当前像素范围内是否有数据点
            double minV = 0, maxV = 0;    // 当前像素内的最小值和最大值 (电压)
            double minT = 0, maxT = 0;    // 最小值和最大值对应的时间戳 (时间)

            // 3. 内层循环：找出落在当前像素列时间段 [tBinStart, tBinEnd] 内的所有原始数据点
            // 注意：最后一列使用 <= 包含边界点，其他列使用 < 避免重复
            bool isLastBin = (px == w - 1);
            while (idx < i1) {
                const double t = m_time[idx];
                // 对于最后一列，包含等于边界的点；对于其他列，不包含右边界（避免重复）
                if (isLastBin ? (t > tBinEnd) : (t >= tBinEnd)) {
                    break;
                }
                
                const double v = y[idx];      // 当前点的数值

                if (!has) {
                    // 如果是该像素段内的第一个点，直接初始化为最大/最小值
                    has = true;
                    minV = maxV = v;
                    minT = maxT = t;
                } else {
                    // 更新最大值和最小值
                    if (v < minV) { minV = v; minT = t; }
                    if (v > maxV) { maxV = v; maxT = t; }
                }
                ++idx; // 继续看原始数组里的下一个点
            }

            // 4. 将提取出的两个点存入输出数组
            if (has) {
                // 排序逻辑：为了让连线顺滑，比较 minT 和 maxT 的先后顺序
                // 确保在同一个像素列内，先画时间早的点，再画时间晚的点
                if (minT <= maxT) {
                    outX.push_back(minT); outY.push_back(minV);
                    outX.push_back(maxT); outY.push_back(maxV);
                } else {
                    outX.push_back(maxT); outY.push_back(maxV);
                    outX.push_back(minT); outY.push_back(minV);
                }
            }

            // 更新下一个像素桶的开始时间
            tBinStart = tBinEnd;
        }
    };


    if (plot == ui->plotVoltage) {
        QVector<double> x, y;
        downsampleSeries(m_voltage, x, y);
        plot->graph(0)->setData(x, y, true);
    } else {
        QVector<double> xI, yI, xP, yP;
        downsampleSeries(m_current, xI, yI);
        downsampleSeries(m_power, xP, yP);
        plot->graph(0)->setData(xI, yI, true);
        plot->graph(1)->setData(xP, yP, true);
    }
}

/**
 * @brief 曲线双击时的处理（槽函数）
 * @param plottable 被双击的曲线对象
 * @param dataIndex 数据点索引（未使用）
 * @param event 鼠标事件（未使用）
 * 
 * 功能：双击曲线时弹出颜色选择对话框，修改对应曲线的颜色
 * 
 * 处理流程：
 * 1. 类型转换：QCPAbstractPlottable -> QCPGraph
 * 2. 弹出颜色选择对话框
 * 3. 更新曲线颜色并刷新图表
 */
void WaveformWidget::onPlottableDoubleClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    Q_UNUSED(dataIndex);
    Q_UNUSED(event);
    
    // ---------------------------------------------------------
    // 第一步：类型转换
    // ---------------------------------------------------------
    // 尝试将其转换为 QCPGraph（波形图）
    QCPGraph *graph = qobject_cast<QCPGraph*>(plottable);

    // 如果转换失败（比如它是柱状图或其他类型），直接返回
    if (!graph) return;

    // ---------------------------------------------------------
    // 第二步：颜色选择逻辑
    // ---------------------------------------------------------
    QPen currentPen = graph->pen();
    QColor initialColor = currentPen.color();

    // 弹出颜色选择对话框
    QColor newColor = QColorDialog::getColor(initialColor, this, "修改波形颜色");

    // 如果用户选择了有效颜色（点击了"确定"），则更新曲线颜色
    if (newColor.isValid()) {
        QPen newPen = currentPen;
        newPen.setColor(newColor);
        graph->setPen(newPen);

        // ---------------------------------------------------------
        // 第三步：刷新图表
        // ---------------------------------------------------------
        // 使用 graph->parentPlot() 获取 QCustomPlot 指针并重绘
        if (graph->parentPlot()) {
            graph->parentPlot()->replot();
        }
    }
}
/**
 * @brief 事件过滤器：拦截并处理图表控件的鼠标/滚轮事件
 * @param watched 被监听的对象（QCustomPlot）
 * @param event 事件对象
 * @return true 表示事件已处理，不再传递；false 表示继续传递
 * 
 * 处理的事件类型：
 * 1. 测量工具交互（十字光标移动、卡尺拖拽）
 * 2. Ctrl + 滚轮智能缩放（在 Y 轴区域垂直缩放，在绘图区水平缩放）
 * 3. 中键/右键拖拽平移视图
 */
bool WaveformWidget::eventFilter(QObject *watched, QEvent *event)
{
    // 将 watched 对象转换为 QCustomPlot 指针
    QCustomPlot *plot = qobject_cast<QCustomPlot*>(watched);
    if (!plot) return QWidget::eventFilter(watched, event);

    // =========================
    // 测量工具（卡尺/十字光标）事件处理
    // =========================
    if (m_measureMode != MeasureToolMode::Off) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent*>(event);

            //卡尺测量的优先级高于图表的普通缩放和拖拽
            if (handleMeasureMousePress(plot, me)) return true;

        } else if (event->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (handleMeasureMouseMove(plot, me)) return true;

        } else if (event->type() == QEvent::MouseButtonRelease) {
            auto *me = static_cast<QMouseEvent*>(event);
            if (handleMeasureMouseRelease(plot, me)) return true;
        }
    }

    // =========================================================
    // 功能 1：Ctrl + 滚轮智能缩放
    // =========================================================
    // 根据鼠标位置自动选择缩放方向：
    // - 在 Y 轴刻度区域（左侧）-> 垂直缩放（Y 轴，以0为中心）
    // - 在绘图区域 + Ctrl -> 水平缩放（X 轴/时间轴）
    // - 在绘图区域 + 不按 Ctrl -> 双向缩放（X轴默认，Y轴以0为中心）
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);

        // 计算左右刻度区域宽度：左侧用于 yAxis，右侧用于 yAxis2（功率）
        const QRect ar = plot->axisRect()->rect();
        const int leftAxisWidth = plot->axisRect()->left();
        // rightAxisWidth 保留用于未来可能的右轴判断逻辑
        // const int rightAxisWidth = plot->viewport().right() - plot->axisRect()->right();
        // Qt5兼容：使用pos()而不是position()
        // 注意：在Qt5.15中pos()虽然被标记为deprecated，但仍然可用
        QPoint pos = wheelEvent->pos();
        
        // 检查是否按住了 Ctrl 键
        bool ctrlPressed = wheelEvent->modifiers() & Qt::ControlModifier;
        
        // 鼠标在左侧刻度区域 -> 缩放左轴 (yAxis)
        if (pos.x() < leftAxisWidth) {
            handleYAxisZoom(plot, wheelEvent, plot->yAxis);
            return true;
        }
        // 鼠标在右侧刻度区域（仅电流/功率图有 yAxis2）-> 缩放右轴 (yAxis2)
        if (pos.x() > ar.right() && plot->yAxis2 && plot->yAxis2->visible()) {
            handleYAxisZoom(plot, wheelEvent, plot->yAxis2);
            return true;
        }

        // 在绘图区
        if (ctrlPressed) {
            // 按Ctrl -> 只水平缩放（X 轴/时间轴），交给 QCustomPlot 默认处理
            plot->axisRect()->setRangeZoom(Qt::Horizontal);
        } else {
            // 不按 Ctrl -> 先按左轴规则缩放 Y（以0为中心），再手动缩放 X
            handleYAxisZoom(plot, wheelEvent, plot->yAxis);

            // 计算缩放因子
            double zoomFactor = 1.15;
            if (wheelEvent->angleDelta().y() < 0) {
                zoomFactor = 1.0 / zoomFactor;
            }
            // 获取当前X轴范围
            QCPRange xRange = plot->xAxis->range();
            double newRangeSize = xRange.size() / zoomFactor;
            
            // 判断0是否在当前范围内
            bool zeroInRange = (xRange.lower <= 0 && xRange.upper >= 0);
            double newLower = 0.0, newUpper = 0.0;

            if (zeroInRange) {
                // 保持0的相对位置不变
                double distToLower = 0 - xRange.lower;
                double distToUpper = xRange.upper - 0;
                double totalDist = xRange.size();
                double lowerRatio = distToLower / totalDist;
                double upperRatio = distToUpper / totalDist;

                newRangeSize = qMax(newRangeSize, 1e-9);
                newLower = 0 - newRangeSize * lowerRatio;
                newUpper = 0 + newRangeSize * upperRatio;
            } else {
                // 0 不在范围内，仍然以鼠标位置为中心
                double mouseX = plot->xAxis->pixelToCoord(pos.x());
                newLower = mouseX - (mouseX - xRange.lower) / zoomFactor;
                newUpper = mouseX + (xRange.upper - mouseX) / zoomFactor;
            }
            
            plot->xAxis->setRange(newLower, newUpper);
            plot->replot(QCustomPlot::rpQueuedReplot);
            
            // 拦截事件，因为我们已经手动处理了X和Y轴缩放
            return true;
        }
    }

    // =========================================================
    // 功能 2：中键/右键拖拽平移视图
    // =========================================================

    // --- 按下中键/右键：开始拖拽 ---
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::MiddleButton || mouseEvent->button() == Qt::RightButton) {
            m_isDragging = true;
            m_lastDragPos = mouseEvent->pos();
            plot->setCursor(Qt::ClosedHandCursor); // 改变鼠标形状为"抓手"
            return true; // 吃掉事件，不传给父类
        }
    }

    // --- 移动鼠标：执行拖拽计算 ---
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        if (m_isDragging) {
            // 1. 获取当前的坐标轴
            QCPAxis *xAxis = plot->xAxis;
            QCPAxis *yAxis = plot->yAxis;

            // 2. 将像素差值转换为坐标差值（Pixel -> Coordinate）
            //    公式：坐标差值 = pixelToCoord(新位置) - pixelToCoord(旧位置)
            double diffX = xAxis->pixelToCoord(mouseEvent->pos().x()) - xAxis->pixelToCoord(m_lastDragPos.x());
            double diffY = yAxis->pixelToCoord(mouseEvent->pos().y()) - yAxis->pixelToCoord(m_lastDragPos.y());

            // 3. 移动坐标轴范围（注意方向：鼠标往右拉，视野往左移，所以是减）
            xAxis->moveRange(-diffX);
            yAxis->moveRange(-diffY);
            
            // 用户手动拖拽时间轴时，关闭自动跟随（只拖拽Y轴时不关闭）
            if (qAbs(diffX) > 0.001) {  // 如果X轴有移动
                m_autoFollow = false;
            }

            // 4. 如果有右侧 Y 轴（功率轴），也同步移动
            if (plot->yAxis2->visible()) {
                double diffY2 = plot->yAxis2->pixelToCoord(mouseEvent->pos().y()) - plot->yAxis2->pixelToCoord(m_lastDragPos.y());
                plot->yAxis2->moveRange(-diffY2);
            }

            // 5. 刷新重绘
            plot->replot(QCustomPlot::rpQueuedReplot);

            // 6. 更新位置
            m_lastDragPos = mouseEvent->pos();
            return true;
        }
    }

    // --- 松开中键/右键：结束拖拽 ---
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::MiddleButton || mouseEvent->button() == Qt::RightButton) {
            m_isDragging = false;
            plot->setCursor(Qt::ArrowCursor); // 恢复鼠标形状
            return true;
        }
    }

    // 其他事件交给父类默认处理
    return QWidget::eventFilter(watched, event);
}

/**
 * @brief Y轴缩放处理（以0刻度为中心，如果0在范围内）
 * @param plot 图表对象
 * @param wheelEvent 滚轮事件
 * 
 * 功能：
 * 1. 检查当前Y轴范围是否包含0
 * 2. 如果包含0，以0为中心进行缩放
 * 3. 如果不包含0，以当前中心点进行缩放
 */
void WaveformWidget::handleYAxisZoom(QCustomPlot *plot, QWheelEvent *wheelEvent, QCPAxis *targetAxis)
{
    if (!plot || !targetAxis) return;
    
    // 获取当前Y轴范围
    QCPRange currentRange = targetAxis->range();
    
    // 计算缩放因子（滚轮向上放大，向下缩小）
    double zoomFactor = 1.15;  // 每次缩放15%
    if (wheelEvent->angleDelta().y() < 0) {
        zoomFactor = 1.0 / zoomFactor;  // 向下滚动，缩小
    }
    
    // 计算新的范围大小
    double newRangeSize = currentRange.size() / zoomFactor;
    
    // 判断0是否在当前范围内
    bool zeroInRange = (currentRange.lower <= 0 && currentRange.upper >= 0);
    
    double newLower, newUpper;
    
    if (zeroInRange) {
        // 如果0在范围内，保持0的位置不变（不强制居中）
        // 计算0到上下边界的距离比例
        double distToLower = 0 - currentRange.lower;  // 0到下限的距离
        double distToUpper = currentRange.upper - 0;  // 0到上限的距离
        double totalDist = currentRange.size();
        double lowerRatio = distToLower / totalDist;  // 0在下方的比例
        double upperRatio = distToUpper / totalDist;  // 0在上方的比例
        
        // 缩放后，保持这个比例不变
        newRangeSize = qMax(newRangeSize, 0.01);  // 最小范围限制
        newLower = 0 - newRangeSize * lowerRatio;
        newUpper = 0 + newRangeSize * upperRatio;
    } else {
        // 如果0不在范围内，以当前中心点进行缩放
        double center = currentRange.center();
        newRangeSize = qMax(newRangeSize, 0.01);  // 最小范围限制
        newLower = center - newRangeSize / 2.0;
        newUpper = center + newRangeSize / 2.0;
    }
    
    // 应用新范围
    targetAxis->setRange(newLower, newUpper);
    
    // 触发重绘
    plot->replot(QCustomPlot::rpQueuedReplot);
}

/**
 * @brief 框选完成后的统计处理（槽函数）
 * @param rect 框选区域的像素矩形
 * @param event 鼠标事件（用于显示 Tooltip 位置）
 * 
 * 功能：
 * 1. 将框选的像素矩形转换为时间范围
 * 2. 基于原始数据（不是降采样后的数据）计算统计信息
 * 3. 显示统计结果（时间范围、时长、平均值、最大值、最小值）
 * 
 * 注意：使用原始数据计算，保证统计结果准确
 */
void WaveformWidget::onSelectionRectAccepted(const QRect &rect, QMouseEvent *event)
{
    // 1. 获取发送信号的图表对象（因为可能是 plotVoltage 也可能是 plotCurrent）
    QCPSelectionRect *selRect = qobject_cast<QCPSelectionRect*>(sender());
    if (!selRect) return;
    QCustomPlot *plot = selRect->parentPlot();

    // 2. 将像素矩形转换为坐标范围（主要关心 X 轴/时间）
    //    rect.left() 和 right() 是屏幕像素坐标
    double t1=plot->xAxis->pixelToCoord(rect.left());
    double t2=plot->xAxis->pixelToCoord(rect.right());

    // 确保 tStart 小于 tEnd（处理从右往左框选的情况）
    double tStart = qMin(t1, t2);
    double tEnd = qMax(t1, t2);
    double duration = tEnd - tStart; // 时长

    // 3. 准备结果字符串（HTML 格式）
    QString resultInfo = QString("<b>=== 区域统计 ===</b><br>"
                                 "<b>时间范围:</b> %1 s - %2 s<br>"
                                 "<b>时长 (Duration):</b> %3 s<br>")
                             .arg(tStart, 0, 'f', 2)
                             .arg(tEnd, 0, 'f', 2)
                             .arg(duration, 0, 'f', 3);

    // 4. 遍历该图表下所有"可见曲线"的统计（基于原始数据，而不是降采样后的绘制数据）
    auto calcStats = [&](const QVector<double> &y, const QString &name) {
        double minVal = 1e300;
        double maxVal = -1e300;
        long double sumVal = 0;
        int count = 0;

        // 二分定位到时间范围内
        auto itBegin = std::lower_bound(m_time.begin(), m_time.end(), tStart);
        auto itEnd = std::upper_bound(m_time.begin(), m_time.end(), tEnd);
        // 迭代器相减转换为索引
        int b = int(itBegin - m_time.begin());
        int e = int(itEnd - m_time.begin());

        // 在索引范围内找最大值和最小值
        for (int i = b; i < e; ++i) {
            const double v = y[i];
            minVal = std::min(minVal, v);
            maxVal = std::max(maxVal, v);
            sumVal += v;
            ++count;
        }

        resultInfo += QString("<hr><b>%1:</b><br>").arg(name);
        if (count > 0) {
            // 计算平均值
            const double avgVal = double(sumVal / count);
            resultInfo += QString("Avg: %1<br>Max: %2<br>Min: %3")
                              .arg(avgVal, 0, 'f', 3)
                              .arg(maxVal, 0, 'f', 3)
                              .arg(minVal, 0, 'f', 3);
        } else {
            resultInfo += "No Data";
        }
    };

    // 调用之前写的Lambda函数
    if (plot == ui->plotVoltage) {
        if (m_voltageVisible) calcStats(m_voltage, "Voltage (V)");
    } else {
        if (m_currentVisible) calcStats(m_current, "Current (A)");
        if (m_powerVisible) calcStats(m_power, "Power (W)");
    }

    // 5. 临时的、浮动的文本提示窗口。
    // event->globalPos() 让弹窗出现在鼠标旁边
    // Qt5兼容：使用globalPos()而不是globalPosition()
    QToolTip::showText(event->globalPos(), resultInfo, plot);
    // 它是瞬时的
}

void WaveformWidget::onSelectionChanged()
{
    // 获取发送信号的 plot（电压图或电流图）
    QCustomPlot *plot = qobject_cast<QCustomPlot*>(sender());
    if (!plot) return;

    // 1. 先恢复上一次高亮的曲线线宽（避免遍历所有 graph）
    for (QPointer<QCPGraph> &g : m_lastHighlightedGraphs) {
        if (!g) continue;
        QPen pen = g->pen();
        pen.setWidthF(1.0);          // 恢复为细线
        g->setPen(pen);
    }
    m_lastHighlightedGraphs.clear();

    // 2. 只处理当前选中的曲线
    const auto selected = plot->selectedPlottables();
    for (QCPAbstractPlottable *p : selected) {
        if (auto *graph = qobject_cast<QCPGraph*>(p)) {
            QPen pen = graph->pen();
            pen.setWidthF(1.5);      // 轻微加粗，减小重绘成本
            graph->setPen(pen);
            m_lastHighlightedGraphs.push_back(graph);
        }
    }

    // 3. 刷新重绘
    plot->replot(QCustomPlot::rpQueuedReplot);
}

/**
 * @brief 点击曲线时的处理（槽函数）
 * @param plottable 被点击的曲线对象
 * @param dataIndex 数据点索引（暂时未使用，使用原始数据重新查找）
 * @param event 鼠标事件（用于获取点击位置）
 * 
 * 功能：
 * 1. 将鼠标点击位置转换为时间坐标
 * 2. 在原始数据中查找最近的数据点（避免降采样后点不准）
 * 3. 显示 Tooltip，显示该点的时间和数值
 */
void WaveformWidget::onPlottableClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event)
{
    Q_UNUSED(dataIndex);

    // 1. 确认点到的是一条曲线（Graph）
    QCPGraph *graph = qobject_cast<QCPGraph*>(plottable);
    if (!graph) return;

    // 2. 获取鼠标位置对应的 X 轴坐标（Time）
    QCustomPlot *plot = graph->parentPlot();
    double xCoord = plot->xAxis->pixelToCoord(event->pos().x());

    // 3. 基于"原始数据"做拾取，避免降采样后点不准
    if (m_time.isEmpty()) return;
    
    // 使用二分查找定位最近的数据点
    auto it = std::lower_bound(m_time.begin(), m_time.end(), xCoord);
    //  获取查到的索引
    int idx = int(it - m_time.begin());
    //  检查是否越界
    if (idx >= m_time.size()) idx = m_time.size() - 1;
    
    // 比较当前点和前一个点，选择距离更近的
    if (idx > 0) {
        const double d0 = qAbs(m_time[idx - 1] - xCoord);
        const double d1 = qAbs(m_time[idx] - xCoord);
        if (d0 < d1) idx = idx - 1;
    }

    // 4. 获取精确的时间和数值
    const double time = m_time[idx];
    double value = 0;
    if (plot == ui->plotVoltage) {
        value = m_voltage[idx];
    } else {
        // 通过 name 区分 Current/Power
        const QString n = graph->name().toLower();
        value = (n.contains("power")) ? m_power[idx] : m_current[idx];
    }

    // 5. 格式化显示的文本
    //    示例： "Time: 12.50 s\nVoltage: 5.12 V"
    QString tipText = QString("<b>%1</b><br>Time: %2 s<br>Value: %3")
                          .arg(graph->name())
                          .arg(time, 0, 'f', 4)  // 保留4位小数
                          .arg(value, 0, 'f', 4);

    // 6. 在鼠标位置显示 Tooltip（浮窗）
    // Qt5兼容：使用globalPos()而不是globalPosition()
    QToolTip::showText(event->globalPos(), tipText, plot);
}

/**
 * @brief 更新所有通道的图表显示（内部辅助函数）
 * 根据m_channelDataMap中的数据更新各个graph
 */
void WaveformWidget::updateChannelGraphs()
{
    if (!ui->plotVoltage || !ui->plotCurrent) {
        return;
    }
    
    // 遍历所有通道数据
    for (auto it = m_channelDataMap.constBegin(); it != m_channelDataMap.constEnd(); ++it) {
        int channelId = it.key();
        if (channelId < 0 || channelId >= kChannelCount) {
            continue;
        }
        
        const ChannelData &channelData = it.value();
        if (channelData.time.isEmpty()) {
            continue;
        }
        
        // 获取通道显示状态
        ChannelVisibility vis = m_channelVisibility.value(channelId, ChannelVisibility());
        bool showVoltage = vis.voltageVisible && m_voltageVisible;
        bool showCurrent = vis.currentVisible && m_currentVisible;
        bool showPower = vis.powerVisible && m_powerVisible;
        
        // 应用视觉降采样并更新电压graph
        if (channelId < ui->plotVoltage->graphCount() && showVoltage) {
            applyVisualDownsampleForChannel(ui->plotVoltage, channelId, channelData.time, channelData.voltage);
        }
        
        // 更新电流graph
        if (channelId < ui->plotCurrent->graphCount() && showCurrent) {
            applyVisualDownsampleForChannel(ui->plotCurrent, channelId, channelData.time, channelData.current);
        }
        
        // 更新功率graph
        int powerGraphIndex = kChannelCount + channelId;
        if (powerGraphIndex < ui->plotCurrent->graphCount() && showPower) {
            applyVisualDownsampleForChannel(ui->plotCurrent, powerGraphIndex, channelData.time, channelData.power);
        }
    }
}

/**
 * @brief 为单个通道应用视觉降采样（内部辅助函数）
 * @param plot 图表对象
 * @param graphIndex graph索引
 * @param time 时间序列
 * @param values 数值序列
 */
void WaveformWidget::applyVisualDownsampleForChannel(QCustomPlot *plot, int graphIndex,
                                                      const QVector<double> &time,
                                                      const QVector<double> &values)
{
    if (!plot || graphIndex < 0 || graphIndex >= plot->graphCount() || 
        time.isEmpty() || values.isEmpty() || time.size() != values.size()) {
        return;
    }
    
    // 获取图表尺寸和X轴范围
    const int w = plot->width();
    const QCPRange xr = plot->xAxis->range();
    const double bin = xr.size() / w;  // 每个像素对应的时间跨度
    
    if (bin <= 0 || w <= 0) {
        return;
    }
    
    // 找到可视范围内的数据索引
    int i0 = 0;
    for (int i = 0; i < time.size(); ++i) {
        if (time[i] >= xr.lower) {
            i0 = i;
            break;
        }
    }
    
    // 使用 upper_bound 确保包含等于 xr.upper 的边界点
    auto itEnd = std::upper_bound(time.begin() + i0, time.end(), xr.upper);
    int i1 = std::min(int(time.size()), int(itEnd - time.begin()));
    
    // 如果数据点很少，直接全量显示
    if (i1 - i0 <= 2) {
        plot->graph(graphIndex)->setData(time, values, true);
        return;
    }
    
    // Min-Max 降采样算法
    QVector<double> outX, outY;
    outX.reserve(w * 2);
    outY.reserve(w * 2);
    
    double tBinStart = xr.lower;
    int idx = i0;
    
    // 注意：即使 idx >= i1，也要处理完所有像素列，确保边界数据不丢失
    for (int px = 0; px < w; ++px) {
        const double tBinEnd = (px == w - 1) ? xr.upper : (tBinStart + bin);
        
        bool has = false;
        double minV = 0, maxV = 0;
        double minT = 0, maxT = 0;
        
        // 注意：最后一列使用 <= 包含边界点，其他列使用 < 避免重复
        bool isLastBin = (px == w - 1);
        while (idx < i1) {
            const double t = time[idx];
            // 对于最后一列，包含等于边界的点；对于其他列，不包含右边界（避免重复）
            if (isLastBin ? (t > tBinEnd) : (t >= tBinEnd)) {
                break;
            }
            
            const double v = values[idx];
            
            if (!has) {
                has = true;
                minV = maxV = v;
                minT = maxT = t;
            } else {
                if (v < minV) { minV = v; minT = t; }
                if (v > maxV) { maxV = v; maxT = t; }
            }
            ++idx;
        }
        
        if (has) {
            if (minT <= maxT) {
                outX.push_back(minT); outY.push_back(minV);
                outX.push_back(maxT); outY.push_back(maxV);
            } else {
                outX.push_back(maxT); outY.push_back(maxV);
                outX.push_back(minT); outY.push_back(minV);
            }
        }
        
        tBinStart = tBinEnd;
    }
    
    // 更新graph数据
    plot->graph(graphIndex)->setData(outX, outY, true);
}

