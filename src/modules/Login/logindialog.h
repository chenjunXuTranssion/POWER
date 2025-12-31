#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginDialog;
}
QT_END_NAMESPACE

/**
 * @brief 登录对话框
 * 提供用户登录界面，包含用户名、密码输入和登录功能
 */
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    /**
     * @brief 获取输入的用户名
     * @return 用户名
     */
    QString getUsername() const;

    /**
     * @brief 获取输入的密码
     * @return 密码
     */
    QString getPassword() const;

    /**
     * @brief 是否记住密码
     * @return true表示记住密码
     */
    bool isRememberPassword() const;

private slots:
    /**
     * @brief 登录按钮点击事件
     */
    void onLoginClicked();

    /**
     * @brief 取消按钮点击事件
     */
    void onCancelClicked();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H


