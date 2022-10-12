#include "administrator.h"
#include "ui_administrator.h"
#include <QDebug>
#include <QMessageBox>
Administrator::Administrator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Administrator)
{
    ui->setupUi(this);
    setFixedSize(600, 500);
    setWindowIcon(QIcon(":/source/icon.png"));
    setWindowTitle("管理系统");
    isLogin = false;
    QPushButton *login = ui->login;
    //验证是否成功登入
    connect(login,&QPushButton::clicked,this,[=](){
       if(ui->accountNumber->text() == "abc" && ui->accountPassword->text() == "123")
       {
           isLogin = true;
            ui->success->setText("成功登入");
            QMessageBox::information(this,
                tr("Login"),
                tr("成功登入"),
                QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Ok);
       }else
           QMessageBox::critical(this, tr("登入"), tr("登入失败,请重新确认信息！"));
    });
    QPushButton *add = ui->addNewLine;
    //将用户输入的信息，传输到resultshow端
    connect(add,&QPushButton::clicked,this,[=](){
        if(isLogin)
        {
            QString point1 = ui->Point1->text();
            QString point2 = ui->Point2->text();
            QString dis = ui->distance->text();
            QString line = ui->line->text();
            emit addInfo(point1, point2, line, dis);
        }else
        {
            QMessageBox::critical(this, tr("登入"), tr("登入失败,请重新确认信息！"));
        }
    });
    QPushButton *del = ui->destory;
    connect(del,&QPushButton::clicked,this,[=](){
        if(isLogin)
        {
            QString point1 = ui->Point1->text();
            QString point2 = ui->Point2->text();
            emit delInfo(point1, point2);
        }else
        {
            QMessageBox::critical(this, tr("登入"), tr("登入失败,请重新确认信息！"));
        }
    });
}

Administrator::~Administrator()
{
    delete ui;
}
