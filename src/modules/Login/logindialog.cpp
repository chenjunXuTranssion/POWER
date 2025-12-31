#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    setWindowTitle("登录");
    setModal(true);
    
    // 设置密码输入框为密码模式
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    
    // 连接信号槽
    connect(ui->btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    
    // 显示密码复选框
    connect(ui->checkBoxShowPassword, &QCheckBox::toggled, this, [this](bool checked) {
        ui->lineEditPassword->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });
    
    // 回车键登录
    connect(ui->lineEditPassword, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(ui->lineEditUsername, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::getUsername() const
{
    return ui->lineEditUsername->text();
}

QString LoginDialog::getPassword() const
{
    return ui->lineEditPassword->text();
}

bool LoginDialog::isRememberPassword() const
{
    return ui->checkBoxRemember->isChecked();
}

void LoginDialog::onLoginClicked()
{
    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text();
    
    // 基本验证
    if (username.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名");
        ui->lineEditUsername->setFocus();
        return;
    }
    
    if (password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入密码");
        ui->lineEditPassword->setFocus();
        return;
    }
    
    // 验证通过，接受对话框
    accept();
}

void LoginDialog::onCancelClicked()
{
    reject();
}

