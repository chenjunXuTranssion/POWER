/********************************************************************************
** Form generated from reading UI file 'waveformwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAVEFORMWIDGET_H
#define UI_WAVEFORMWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_WaveformWidget
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QCustomPlot *plotVoltage;
    QCustomPlot *plotCurrent;

    void setupUi(QWidget *WaveformWidget)
    {
        if (WaveformWidget->objectName().isEmpty())
            WaveformWidget->setObjectName(QString::fromUtf8("WaveformWidget"));
        WaveformWidget->resize(839, 471);
        verticalLayout = new QVBoxLayout(WaveformWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        splitter = new QSplitter(WaveformWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Orientation::Vertical);
        splitter->setHandleWidth(8);
        splitter->setStyleSheet(QString::fromUtf8("QSplitter::handle {\n"
"  background: #e9eef3;\n"
"  border: 1px solid #d9dde3;\n"
"  border-radius: 4px;\n"
"}\n"
"QSplitter::handle:hover { background: #dfe7ef; }"));
        plotVoltage = new QCustomPlot(splitter);
        plotVoltage->setObjectName(QString::fromUtf8("plotVoltage"));
        splitter->addWidget(plotVoltage);
        plotCurrent = new QCustomPlot(splitter);
        plotCurrent->setObjectName(QString::fromUtf8("plotCurrent"));
        splitter->addWidget(plotCurrent);

        verticalLayout->addWidget(splitter);


        retranslateUi(WaveformWidget);

        QMetaObject::connectSlotsByName(WaveformWidget);
    } // setupUi

    void retranslateUi(QWidget *WaveformWidget)
    {
        WaveformWidget->setWindowTitle(QCoreApplication::translate("WaveformWidget", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WaveformWidget: public Ui_WaveformWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAVEFORMWIDGET_H
