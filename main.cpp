#include "mainwindow.h"

#include <QApplication>
#include "logindialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 1. 先显示登录对话框
    LoginDialog loginDialog;

    // 2. 如果用户取消登录或关闭对话框，直接退出
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;  // 用户取消登录，退出应用
    }

    // 3. 登录成功，获取用户信息（可选，用于后续使用）
    QString username = loginDialog.getUsername();
    QString password = loginDialog.getPassword();
    // bool rememberPassword = loginDialog.isRememberPassword();

    MainWindow w;
    w.show();
    return a.exec();
}
