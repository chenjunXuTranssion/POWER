#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modules/WaveformView/waveformwidget.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QCheckBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initChannelTable();
    wireChannelToggles();

    // 启动测试：3个通道，全部显示
    startWaveformTest(50);
    configureAllChannelsDisplay(3, true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnExport_2_clicked()
{
    if (ui->waveformContainer) {
        ui->waveformContainer->cycleMeasureToolMode();
    }
}

// =========================================================
//  初始化通道表格
// =========================================================
void MainWindow::initChannelTable()
{
    QTableWidget *table = ui->tableChannels;

    // 1. 基础属性设置
    table->setColumnCount(4);
    table->verticalHeader()->setVisible(false);       // 隐藏行号
    table->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中整行
    table->setSelectionMode(QAbstractItemView::SingleSelection); // 单选

    // 设置表头文字
    QStringList headers;
    headers << "通道信息" << "I" << "V" << "P";
    table->setHorizontalHeaderLabels(headers);

    // 2. 关键：设置列宽自适应策略
    QHeaderView *header = table->horizontalHeader();

    // 第0列 (通道名): 自动拉伸 (Stretch)，填满剩余空间
    header->setSectionResizeMode(0, QHeaderView::Stretch);

    // 第1-3列 (复选框): 固定宽度 (Fixed)，不随窗口缩放
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->setSectionResizeMode(3, QHeaderView::Fixed);

    // 设置具体的像素宽度 (35px 刚好放下复选框)
    table->setColumnWidth(1, 35);
    table->setColumnWidth(2, 35);
    table->setColumnWidth(3, 35);

    // 3. 准备数据 (模拟 4 个通道)
    struct ChannelData {
        QString name;
        QString desc;
    };
    QList<ChannelData> dataList = {
        {"通道1", "VBAT_SENSE"},
        {"通道2", "VPH_PWR"},
        {"通道3", "VCORE_LDO"},
        {"通道4", "WIFI_3V3"}
    };

    // 4. 填充数据
    // 先清空所有行，防止重复调用时叠加
    table->setRowCount(0);

    for (int i = 0; i < dataList.size(); ++i) {
        int row = table->rowCount();
        table->insertRow(row);

        // --- 第0列：双行文本 ---
        QString text = QString("%1\n%2").arg(dataList[i].name, dataList[i].desc);
        QTableWidgetItem *item = new QTableWidgetItem(text);
        // 设置文本左对齐，垂直居中
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        // 设置字体颜色或大小(可选)
        // item->setFont(QFont("Microsoft YaHei", 9));
        table->setItem(row, 0, item);

        // --- 第1-3列：插入居中的复选框 ---
        table->setCellWidget(row, 1, createCenteredCheckBox()); // I (电流)
        table->setCellWidget(row, 2, createCenteredCheckBox()); // V (电压)
        table->setCellWidget(row, 3, createCenteredCheckBox()); // P (功率)
    }

    // 5. 设置默认行高
    table->verticalHeader()->setDefaultSectionSize(48);
    // =========================================================
    //  测试代码：模拟 DAQ 硬件产生数据
    // =========================================================
}

void MainWindow::wireChannelToggles()
{

    QTableWidget *table = ui->tableChannels;
    if (!table || !ui->waveformContainer) return;

    auto getCheckBoxAt = [&](int row, int col) -> QCheckBox* {
        QWidget *cell = table->cellWidget(row, col);
        if (!cell) return nullptr;
        return cell->findChild<QCheckBox*>();
    };

    auto recompute = [&]() {
        // 全局显示控制（保留原有逻辑）
        bool anyI = false, anyV = false, anyP = false;
        for (int r = 0; r < table->rowCount(); ++r) {
            // C++11兼容：不使用init-statement in if
            QCheckBox *cb1 = getCheckBoxAt(r, 1);
            if (cb1 && cb1->isChecked()) anyI = true;
            QCheckBox *cb2 = getCheckBoxAt(r, 2);
            if (cb2 && cb2->isChecked()) anyV = true;
            QCheckBox *cb3 = getCheckBoxAt(r, 3);
            if (cb3 && cb3->isChecked()) anyP = true;
        }
        ui->waveformContainer->setCurrentVisible(anyI);
        ui->waveformContainer->setVoltageVisible(anyV);
        ui->waveformContainer->setPowerVisible(anyP);
        
        // 使用新接口：为每个通道单独设置显示状态
        for (int r = 0; r < table->rowCount() && r < WaveformWidget::kChannelCount; ++r) {
            auto cbI = getCheckBoxAt(r, 1);
            auto cbV = getCheckBoxAt(r, 2);
            auto cbP = getCheckBoxAt(r, 3);
            
            bool showI = cbI ? cbI->isChecked() : true;
            bool showV = cbV ? cbV->isChecked() : true;
            bool showP = cbP ? cbP->isChecked() : true;
            
            ui->waveformContainer->setChannelVisible(r, showV, showI, showP);
        }
    };

    // 连接所有复选框：一改动就重新聚合一次
    for (int r = 0; r < table->rowCount(); ++r) {
        for (int c : {1, 2, 3}) {
            if (auto cb = getCheckBoxAt(r, c)) {
                connect(cb, &QCheckBox::toggled, this, [=](bool) { recompute(); });
            }
        }
    }

    // 初始化一次
    recompute();
}


// =========================================================
//  辅助函数：创建居中的复选框
// =========================================================
QWidget* MainWindow::createCenteredCheckBox()
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);

    // 必须把边距清零，否则控件会很大
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    QCheckBox *chk = new QCheckBox();
    chk->setChecked(true); // 默认勾选

    // 不在这里连业务逻辑：统一在 wireChannelToggles() 里根据表格聚合处理。

    layout->addWidget(chk);
    return widget;
}

// =========================================================
//  测试函数实现：波形显示模块测试
// =========================================================

/**
 * @brief 启动波形测试数据生成
 * @param channelCount 要测试的通道数量（1-10）
 * @param updateInterval 更新间隔（毫秒），默认33ms（约30Hz）
 */
void MainWindow::startWaveformTest(int channelCount, int updateInterval)
{
    if (!ui->waveformContainer) {
        qDebug() << "错误：waveformContainer 未初始化";
        return;
    }
    
    // 限制通道数量范围
    m_testChannelCount = qBound(1, channelCount, 10);
    
    // 停止之前的测试（如果存在）
    stopWaveformTest();
    
    // 清空之前的数据
    ui->waveformContainer->clear();
    m_testTime = 0.0;
    
    // 创建定时器
    m_testTimer = new QTimer(this);
    m_testTimer->setInterval(updateInterval);
    connect(m_testTimer, &QTimer::timeout, this, &MainWindow::generateTestData);
    
    // 启动定时器
    m_testTimer->start();
    
    qDebug() << QString("启动波形测试：%1 个通道，更新间隔 %2 ms").arg(m_testChannelCount).arg(updateInterval);
}

/**
 * @brief 停止波形测试数据生成
 */
void MainWindow::stopWaveformTest()
{
    if (m_testTimer) {
        m_testTimer->stop();
        m_testTimer->deleteLater();
        m_testTimer = nullptr;
    }
    m_testTime = 0.0;
    qDebug() << "停止波形测试";
}

/**
 * @brief 配置通道显示状态
 * @param channelId 通道ID（0开始）
 * @param showVoltage 显示电压
 * @param showCurrent 显示电流
 * @param showPower 显示功率
 */
void MainWindow::configureChannelDisplay(int channelId, bool showVoltage, bool showCurrent, bool showPower)
{
    if (!ui->waveformContainer) {
        return;
    }
    
    if (channelId < 0 || channelId >= 10) {
        qDebug() << QString("错误：通道ID %1 超出范围（0-9）").arg(channelId);
        return;
    }
    
    // 保存配置
    m_channelVoltageVisible[channelId] = showVoltage;
    m_channelCurrentVisible[channelId] = showCurrent;
    m_channelPowerVisible[channelId] = showPower;
    
    // 应用到波形显示模块
    ui->waveformContainer->setChannelVisible(channelId, showVoltage, showCurrent, showPower);
    
    qDebug() << QString("配置通道 %1：V=%2, I=%3, P=%4")
                .arg(channelId)
                .arg(showVoltage ? "显示" : "隐藏", showCurrent ? "显示" : "隐藏", showPower ? "显示" : "隐藏");
}

/**
 * @brief 配置所有通道的显示状态
 * @param channelCount 通道数量
 * @param defaultShowAll 是否默认显示所有（true=全部显示，false=只显示电压）
 */
void MainWindow::configureAllChannelsDisplay(int channelCount, bool defaultShowAll)
{
    if (!ui->waveformContainer) {
        return;
    }
    
    int count = qBound(1, channelCount, 10);
    
    for (int i = 0; i < count; ++i) {
        if (defaultShowAll) {
            // 全部显示
            configureChannelDisplay(i, true, true, true);
        } else {
            // 只显示电压
            configureChannelDisplay(i, true, false, false);
        }
    }
    
    qDebug() << QString("配置 %1 个通道：%2")
                .arg(count)
                .arg(defaultShowAll ? "全部显示" : "只显示电压");
}

/**
 * @brief 生成测试数据（定时器回调函数）
 */
void MainWindow::generateTestData()
{
    if (!ui->waveformContainer || !m_testTimer) {
        return;
    }
    
    // 模拟生成物理量：20 kHz 的正弦/余弦波
    const double freqHz = 20000.0;                 // 信号频率：20 kHz
    const double omega  = 2.0 * M_PI * freqHz;     // 角频率 ω = 2πf
    
    // 创建多通道数据包
    WaveformWidget::MultiChannelData multiData;
    
    // 为每个通道生成数据
    for (int ch = 0; ch < m_testChannelCount; ++ch) {
        // 每个通道使用不同的频率倍数和幅值，便于区分
        double freqMultiplier = 0.8 + ch * 0.3;  // 0.8, 1.1, 1.4, 1.7, ...
        double voltageAmplitude = 2.0 + ch * 0.5; // 2.0, 2.5, 3.0, 3.5, ...
        double currentAmplitude = 0.5 + ch * 0.2; // 0.5, 0.7, 0.9, 1.1, ...
        double voltageOffset = 2.0 + ch * 0.3;   // 2.0, 2.3, 2.6, 2.9, ...
        double currentOffset = 0.5 + ch * 0.15;    // 0.5, 0.65, 0.8, 0.95, ...
        
        // 生成电压：正弦波 + 噪声
        double voltage = voltageAmplitude * qSin(omega * m_testTime * freqMultiplier) + voltageOffset;
        double voltageNoise = QRandomGenerator::global()->generateDouble() * 0.1;
        voltage += voltageNoise;
        
        // 生成电流：余弦波 + 噪声
        double current = currentAmplitude * qCos(omega * m_testTime * freqMultiplier * 1.1) + currentOffset;
        double currentNoise = QRandomGenerator::global()->generateDouble() * 0.15;
        current += currentNoise;
        
        // 添加到数据包（功率自动计算）
        multiData.channelData[ch] = {
            m_testTime,  // time
            voltage,     // voltage
            current,     // current
            NAN          // power: 自动计算
        };
    }
    
    // 使用新的多通道接口添加数据
    ui->waveformContainer->addChannelData(multiData);
    
    // 时间推移（从定时器间隔计算）
    double timeStep = m_testTimer->interval() / 1000.0;
    m_testTime += timeStep;
}
