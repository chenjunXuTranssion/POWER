/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *labelTitle;
    QSpacerItem *verticalSpacer_top;
    QLabel *labelUsername;
    QLineEdit *lineEditUsername;
    QSpacerItem *verticalSpacer_username;
    QLabel *labelPassword;
    QLineEdit *lineEditPassword;
    QSpacerItem *verticalSpacer_password;
    QHBoxLayout *horizontalLayout_checkboxes;
    QCheckBox *checkBoxRemember;
    QCheckBox *checkBoxShowPassword;
    QSpacerItem *horizontalSpacer_checkboxes;
    QSpacerItem *verticalSpacer_middle;
    QHBoxLayout *horizontalLayout_buttons;
    QSpacerItem *horizontalSpacer_left;
    QPushButton *btnLogin;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_right;
    QSpacerItem *verticalSpacer_bottom;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QString::fromUtf8("LoginDialog"));
        LoginDialog->resize(520, 480);
        LoginDialog->setMinimumSize(QSize(520, 480));
        LoginDialog->setMaximumSize(QSize(520, 480));
        LoginDialog->setWindowModality(Qt::ApplicationModal);
        LoginDialog->setStyleSheet(QString::fromUtf8("QDialog {\n"
"  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                             stop:0 #f8f9fa, \n"
"                             stop:1 #e9ecef);\n"
"}\n"
"\n"
"QLabel {\n"
"  color: #2c3e50;\n"
"  font-size: 15px;\n"
"  font-weight: 500;\n"
"}\n"
"\n"
"QLineEdit {\n"
"  padding: 14px 18px;\n"
"  border: 2px solid #dee2e6;\n"
"  border-radius: 8px;\n"
"  background: #ffffff;\n"
"  font-size: 15px;\n"
"  color: #212529;\n"
"  min-height: 20px;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"  border-color: #3498db;\n"
"  background: #ffffff;\n"
"  outline: none;\n"
"}\n"
"\n"
"QLineEdit:hover {\n"
"  border-color: #adb5bd;\n"
"}\n"
"\n"
"QPushButton {\n"
"  padding: 12px 32px;\n"
"  border: none;\n"
"  border-radius: 8px;\n"
"  font-size: 15px;\n"
"  font-weight: 600;\n"
"  min-width: 120px;\n"
"  min-height: 42px;\n"
"}\n"
"\n"
"QPushButton#btnLogin {\n"
"  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                              stop:0 #3498db, \n"
"                              stop:"
                        "1 #2980b9);\n"
"  color: #ffffff;\n"
"}\n"
"\n"
"QPushButton#btnLogin:hover {\n"
"  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                              stop:0 #2980b9, \n"
"                              stop:1 #21618c);\n"
"}\n"
"\n"
"QPushButton#btnLogin:pressed {\n"
"  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                              stop:0 #21618c, \n"
"                              stop:1 #1a5490);\n"
"}\n"
"\n"
"QPushButton#btnCancel {\n"
"  background: #6c757d;\n"
"  color: #ffffff;\n"
"}\n"
"\n"
"QPushButton#btnCancel:hover {\n"
"  background: #5a6268;\n"
"}\n"
"\n"
"QPushButton#btnCancel:pressed {\n"
"  background: #545b62;\n"
"}\n"
"\n"
"QCheckBox {\n"
"  color: #495057;\n"
"  font-size: 14px;\n"
"  font-weight: 400;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"  width: 20px;\n"
"  height: 20px;\n"
"  border: 2px solid #adb5bd;\n"
"  border-radius: 4px;\n"
"  background: #ffffff;\n"
"}\n"
"\n"
"QCheckBox::indicator:hover {\n"
"  border-color: #3498db;\n"
"}\n"
"\n"
""
                        "QCheckBox::indicator:checked {\n"
"  background: #3498db;\n"
"  border-color: #3498db;\n"
"  image: none;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked::after {\n"
"  content: \"\";\n"
"  width: 5px;\n"
"  height: 10px;\n"
"  border: solid white;\n"
"  border-width: 0 2px 2px 0;\n"
"  transform: rotate(45deg);\n"
"}"));
        verticalLayout = new QVBoxLayout(LoginDialog);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(50, 50, 50, 50);
        labelTitle = new QLabel(LoginDialog);
        labelTitle->setObjectName(QString::fromUtf8("labelTitle"));
        labelTitle->setStyleSheet(QString::fromUtf8("font-size: 32px;\n"
"font-weight: 700;\n"
"color: #212529;\n"
"margin-bottom: 15px;\n"
"letter-spacing: 1px;"));
        labelTitle->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(labelTitle);

        verticalSpacer_top = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_top);

        labelUsername = new QLabel(LoginDialog);
        labelUsername->setObjectName(QString::fromUtf8("labelUsername"));
        labelUsername->setStyleSheet(QString::fromUtf8("margin-bottom: 8px;\n"
"font-size: 15px;"));

        verticalLayout->addWidget(labelUsername);

        lineEditUsername = new QLineEdit(LoginDialog);
        lineEditUsername->setObjectName(QString::fromUtf8("lineEditUsername"));

        verticalLayout->addWidget(lineEditUsername);

        verticalSpacer_username = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_username);

        labelPassword = new QLabel(LoginDialog);
        labelPassword->setObjectName(QString::fromUtf8("labelPassword"));
        labelPassword->setStyleSheet(QString::fromUtf8("margin-bottom: 8px;\n"
"font-size: 15px;"));

        verticalLayout->addWidget(labelPassword);

        lineEditPassword = new QLineEdit(LoginDialog);
        lineEditPassword->setObjectName(QString::fromUtf8("lineEditPassword"));

        verticalLayout->addWidget(lineEditPassword);

        verticalSpacer_password = new QSpacerItem(20, 18, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_password);

        horizontalLayout_checkboxes = new QHBoxLayout();
        horizontalLayout_checkboxes->setSpacing(30);
        horizontalLayout_checkboxes->setObjectName(QString::fromUtf8("horizontalLayout_checkboxes"));
        checkBoxRemember = new QCheckBox(LoginDialog);
        checkBoxRemember->setObjectName(QString::fromUtf8("checkBoxRemember"));

        horizontalLayout_checkboxes->addWidget(checkBoxRemember);

        checkBoxShowPassword = new QCheckBox(LoginDialog);
        checkBoxShowPassword->setObjectName(QString::fromUtf8("checkBoxShowPassword"));

        horizontalLayout_checkboxes->addWidget(checkBoxShowPassword);

        horizontalSpacer_checkboxes = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_checkboxes->addItem(horizontalSpacer_checkboxes);


        verticalLayout->addLayout(horizontalLayout_checkboxes);

        verticalSpacer_middle = new QSpacerItem(20, 25, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_middle);

        horizontalLayout_buttons = new QHBoxLayout();
        horizontalLayout_buttons->setSpacing(20);
        horizontalLayout_buttons->setObjectName(QString::fromUtf8("horizontalLayout_buttons"));
        horizontalSpacer_left = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_buttons->addItem(horizontalSpacer_left);

        btnLogin = new QPushButton(LoginDialog);
        btnLogin->setObjectName(QString::fromUtf8("btnLogin"));

        horizontalLayout_buttons->addWidget(btnLogin);

        btnCancel = new QPushButton(LoginDialog);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        horizontalLayout_buttons->addWidget(btnCancel);

        horizontalSpacer_right = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_buttons->addItem(horizontalSpacer_right);


        verticalLayout->addLayout(horizontalLayout_buttons);

        verticalSpacer_bottom = new QSpacerItem(20, 50, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer_bottom);


        retranslateUi(LoginDialog);

        btnLogin->setDefault(true);


        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "\347\231\273\345\275\225", nullptr));
        labelTitle->setText(QCoreApplication::translate("LoginDialog", "PowerDAQ \347\231\273\345\275\225", nullptr));
        labelUsername->setText(QCoreApplication::translate("LoginDialog", "\347\224\250\346\210\267\345\220\215", nullptr));
        lineEditUsername->setPlaceholderText(QCoreApplication::translate("LoginDialog", "\350\257\267\350\276\223\345\205\245\347\224\250\346\210\267\345\220\215", nullptr));
        labelPassword->setText(QCoreApplication::translate("LoginDialog", "\345\257\206\347\240\201", nullptr));
        lineEditPassword->setPlaceholderText(QCoreApplication::translate("LoginDialog", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        checkBoxRemember->setText(QCoreApplication::translate("LoginDialog", "\350\256\260\344\275\217\345\257\206\347\240\201", nullptr));
        checkBoxShowPassword->setText(QCoreApplication::translate("LoginDialog", "\346\230\276\347\244\272\345\257\206\347\240\201", nullptr));
        btnLogin->setText(QCoreApplication::translate("LoginDialog", "\347\231\273\345\275\225", nullptr));
        btnCancel->setText(QCoreApplication::translate("LoginDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
