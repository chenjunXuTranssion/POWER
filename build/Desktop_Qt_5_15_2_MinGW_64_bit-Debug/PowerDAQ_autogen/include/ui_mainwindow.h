/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "waveformwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainVLayout;
    QSplitter *centralSplitter;
    QWidget *topPanel;
    QVBoxLayout *verticalLayout;
    QWidget *menuRowWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnFile;
    QPushButton *btnConfig;
    QPushButton *btnTools;
    QPushButton *btnUser;
    QSpacerItem *spacerTopRight;
    QWidget *menuRowWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnQuickSet;
    QPushButton *btnStart;
    QPushButton *btnPause;
    QPushButton *btnStop;
    QPushButton *btnCaptureMode;
    QPushButton *btnSampleRate;
    QPushButton *btnExport;
    QPushButton *btnExport_2;
    QSpacerItem *spacerBottomRight;
    QSplitter *mainSplitter;
    QWidget *leftPanel;
    QVBoxLayout *leftPanelLayout;
    QTableWidget *tableChannels;
    WaveformWidget *waveformContainer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1111, 655);
        MainWindow->setMinimumSize(QSize(0, 50));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        mainVLayout = new QVBoxLayout(centralwidget);
        mainVLayout->setSpacing(10);
        mainVLayout->setObjectName(QString::fromUtf8("mainVLayout"));
        mainVLayout->setContentsMargins(12, 12, 12, 12);
        centralSplitter = new QSplitter(centralwidget);
        centralSplitter->setObjectName(QString::fromUtf8("centralSplitter"));
        centralSplitter->setOrientation(Qt::Orientation::Vertical);
        centralSplitter->setHandleWidth(8);
        centralSplitter->setStyleSheet(QString::fromUtf8("QSplitter::handle {\n"
"  background: #e9eef3;\n"
"  border: 1px solid #d9dde3;\n"
"  border-radius: 4px;\n"
"}\n"
"QSplitter::handle:hover { background: #dfe7ef; }"));
        topPanel = new QWidget(centralSplitter);
        topPanel->setObjectName(QString::fromUtf8("topPanel"));
        topPanel->setMinimumSize(QSize(0, 0));
        topPanel->setMaximumSize(QSize(16777215, 110));
        topPanel->setStyleSheet(QString::fromUtf8("#topPanel {\n"
"  background: #ffffff;\n"
"  border: 1px solid #e6e8eb;\n"
"  border-radius: 10px;\n"
"}\n"
"QPushButton {\n"
"  padding: 6px 12px;\n"
"  border: 1px solid #d9dde3;\n"
"  border-radius: 8px;\n"
"  background: #ffffff;\n"
"}\n"
"QPushButton:hover { background: #f6f8fa; }\n"
"QPushButton:pressed { background: #eef2f6; }\n"
"QPushButton#btnStart { background: #1f883d; color: white; border-color: #1f883d; }\n"
"QPushButton#btnPause { background: #0969da; color: white; border-color: #0969da; }\n"
"QPushButton#btnStop { background: #cf222e; color: white; border-color: #cf222e; }"));
        verticalLayout = new QVBoxLayout(topPanel);
        verticalLayout->setSpacing(8);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        menuRowWidget = new QWidget(topPanel);
        menuRowWidget->setObjectName(QString::fromUtf8("menuRowWidget"));
        horizontalLayout = new QHBoxLayout(menuRowWidget);
        horizontalLayout->setSpacing(8);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnFile = new QPushButton(menuRowWidget);
        btnFile->setObjectName(QString::fromUtf8("btnFile"));

        horizontalLayout->addWidget(btnFile);

        btnConfig = new QPushButton(menuRowWidget);
        btnConfig->setObjectName(QString::fromUtf8("btnConfig"));

        horizontalLayout->addWidget(btnConfig);

        btnTools = new QPushButton(menuRowWidget);
        btnTools->setObjectName(QString::fromUtf8("btnTools"));

        horizontalLayout->addWidget(btnTools);

        btnUser = new QPushButton(menuRowWidget);
        btnUser->setObjectName(QString::fromUtf8("btnUser"));

        horizontalLayout->addWidget(btnUser);

        spacerTopRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerTopRight);


        verticalLayout->addWidget(menuRowWidget);

        menuRowWidget_2 = new QWidget(topPanel);
        menuRowWidget_2->setObjectName(QString::fromUtf8("menuRowWidget_2"));
        horizontalLayout_2 = new QHBoxLayout(menuRowWidget_2);
        horizontalLayout_2->setSpacing(8);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        btnQuickSet = new QPushButton(menuRowWidget_2);
        btnQuickSet->setObjectName(QString::fromUtf8("btnQuickSet"));

        horizontalLayout_2->addWidget(btnQuickSet);

        btnStart = new QPushButton(menuRowWidget_2);
        btnStart->setObjectName(QString::fromUtf8("btnStart"));

        horizontalLayout_2->addWidget(btnStart);

        btnPause = new QPushButton(menuRowWidget_2);
        btnPause->setObjectName(QString::fromUtf8("btnPause"));

        horizontalLayout_2->addWidget(btnPause);

        btnStop = new QPushButton(menuRowWidget_2);
        btnStop->setObjectName(QString::fromUtf8("btnStop"));

        horizontalLayout_2->addWidget(btnStop);

        btnCaptureMode = new QPushButton(menuRowWidget_2);
        btnCaptureMode->setObjectName(QString::fromUtf8("btnCaptureMode"));

        horizontalLayout_2->addWidget(btnCaptureMode);

        btnSampleRate = new QPushButton(menuRowWidget_2);
        btnSampleRate->setObjectName(QString::fromUtf8("btnSampleRate"));

        horizontalLayout_2->addWidget(btnSampleRate);

        btnExport = new QPushButton(menuRowWidget_2);
        btnExport->setObjectName(QString::fromUtf8("btnExport"));

        horizontalLayout_2->addWidget(btnExport);

        btnExport_2 = new QPushButton(menuRowWidget_2);
        btnExport_2->setObjectName(QString::fromUtf8("btnExport_2"));

        horizontalLayout_2->addWidget(btnExport_2);

        spacerBottomRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(spacerBottomRight);


        verticalLayout->addWidget(menuRowWidget_2);

        centralSplitter->addWidget(topPanel);
        mainSplitter = new QSplitter(centralSplitter);
        mainSplitter->setObjectName(QString::fromUtf8("mainSplitter"));
        mainSplitter->setOrientation(Qt::Orientation::Horizontal);
        mainSplitter->setHandleWidth(8);
        mainSplitter->setStyleSheet(QString::fromUtf8("QSplitter::handle {\n"
"  background: #e9eef3;\n"
"  border: 1px solid #d9dde3;\n"
"  border-radius: 4px;\n"
"}\n"
"QSplitter::handle:hover { background: #dfe7ef; }"));
        leftPanel = new QWidget(mainSplitter);
        leftPanel->setObjectName(QString::fromUtf8("leftPanel"));
        leftPanelLayout = new QVBoxLayout(leftPanel);
        leftPanelLayout->setObjectName(QString::fromUtf8("leftPanelLayout"));
        leftPanelLayout->setContentsMargins(0, 0, 0, 0);
        tableChannels = new QTableWidget(leftPanel);
        if (tableChannels->columnCount() < 4)
            tableChannels->setColumnCount(4);
        tableChannels->setObjectName(QString::fromUtf8("tableChannels"));
        tableChannels->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"  background: #ffffff;\n"
"  border: 1px solid #e6e8eb;\n"
"  border-radius: 10px;\n"
"  gridline-color: #eef2f6;\n"
"  font-size: 12px;\n"
"}\n"
"QHeaderView::section {\n"
"  background: #f6f8fa;\n"
"  border: none;\n"
"  border-bottom: 1px solid #e6e8eb;\n"
"  padding: 6px;\n"
"  font-weight: 600;\n"
"}\n"
"QTableWidget::item { padding: 6px; }\n"
"QTableWidget::item:selected { background: #dbeafe; color: #111827; }\n"
"QTableCornerButton::section { background: #f6f8fa; border: none; }"));
        tableChannels->setAlternatingRowColors(true);
        tableChannels->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        tableChannels->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        tableChannels->setShowGrid(false);
        tableChannels->setColumnCount(4);

        leftPanelLayout->addWidget(tableChannels);

        mainSplitter->addWidget(leftPanel);
        waveformContainer = new WaveformWidget(mainSplitter);
        waveformContainer->setObjectName(QString::fromUtf8("waveformContainer"));
        waveformContainer->setStyleSheet(QString::fromUtf8("QWidget {\n"
"  background: #ffffff;\n"
"  border: 1px solid #e6e8eb;\n"
"  border-radius: 10px;\n"
"}"));
        mainSplitter->addWidget(waveformContainer);
        centralSplitter->addWidget(mainSplitter);

        mainVLayout->addWidget(centralSplitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1111, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnFile->setText(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266", nullptr));
        btnConfig->setText(QCoreApplication::translate("MainWindow", "\351\205\215\347\275\256", nullptr));
        btnTools->setText(QCoreApplication::translate("MainWindow", "\345\267\245\345\205\267", nullptr));
        btnUser->setText(QCoreApplication::translate("MainWindow", "\347\224\250\346\210\267", nullptr));
        btnQuickSet->setText(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256", nullptr));
        btnStart->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        btnPause->setText(QCoreApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
        btnStop->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
        btnCaptureMode->setText(QCoreApplication::translate("MainWindow", "capture only", nullptr));
        btnSampleRate->setText(QCoreApplication::translate("MainWindow", "\351\207\207\346\240\267\347\216\207", nullptr));
        btnExport->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272", nullptr));
        btnExport_2->setText(QCoreApplication::translate("MainWindow", "\345\215\241\345\260\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
