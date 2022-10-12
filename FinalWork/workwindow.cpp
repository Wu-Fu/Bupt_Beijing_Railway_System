#include "workwindow.h"
#include "ui_workwindow.h"
#include <resultshow.h>
#include <subwaymap.h>

WorkWindow::WorkWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WorkWindow)
{
    ui->setupUi(this);                                                         //初始化窗口
    this->setFixedSize(1200, 600);
    setWindowTitle("北京地铁查询");
    setWindowIcon(QIcon(":/source/icon.png"));
    ui->textBrowser->setStyleSheet("background-color:rgba(244,244,244,90)");
    ui->textBrowser_2->setStyleSheet("background-color:rgba(244,244,244,90)");
    QFont ft;
    ResultShow *resultScene = new ResultShow;
    QPushButton *startAdministar = ui->administrator;
    SubwayMap *subwayMap = ui->graphicsView;                                    //创建地图层
    //将按键与信号相连，用于与resultshow窗口传递信息
    connect(this,SIGNAL(sendStartPoint(QString)),resultScene,SLOT(receiveStartPoint(QString)));
    connect(this,SIGNAL(sendEndPoint(QString)),resultScene,SLOT(receiveEndPoint(QString)));
    connect(this,SIGNAL(startAdministrator()),resultScene,SLOT(receiveAdministrator()));
    connect(startAdministar,&QPushButton::clicked,this,[=](){
        emit startAdministrator();
    });
    QString startPoint = "";
    //检测出发点的确认键
    connect(ui->startConfirm,&QPushButton::clicked,this,[=](){
        if(resultScene->isInNode(resultScene->QSToS(ui->startPoint->text())))   //如果找到该点的显示
        {
            emit sendStartPoint(ui->startPoint->text());
            QPalette pe;
            pe.setColor(QPalette::WindowText,Qt::black);
            ui->startPointWarn->setPalette(pe);
            ui->startPointWarn->setText("成功找到站点");
        }else
        {
            QPalette pe;
            pe.setColor(QPalette::WindowText,Qt::red);
            ui->startPointWarn->setPalette(pe);
            ui->startPointWarn->setText("未能找到站点，请重新搜索！");
        }
    });
    //检测终点的确认键
    connect(ui->endConfirm,&QPushButton::clicked,this,[=](){
        if(resultScene->isInNode(resultScene->QSToS(ui->endPoint->text())))
        {

            emit sendEndPoint(ui->endPoint->text());
            qDebug() << "找到";
            QPalette pe;
            pe.setColor(QPalette::WindowText,Qt::black);
            ui->endPointWarn->setPalette(pe);
            ui->endPointWarn->setText("成功找到站点");
        }else
        {
            qDebug() << "没找到";
            QPalette pe;
            pe.setColor(QPalette::WindowText,Qt::red);
            ui->endPointWarn->setPalette(pe);
            ui->endPointWarn->setText("未能找到站点，请重新搜索！");
        }
    });
    //检测开始查找键的信息
    connect(ui->startSearch,&QPushButton::clicked,this,[=]{

        std::string startNode = resultScene->QSToS(ui->startPoint->text());
        std::string endNode = resultScene->QSToS(ui->endPoint->text());
        if(resultScene->isInNode(startNode) && resultScene->isInNode(endNode))
        {
            emit sendStartPoint(ui->startPoint->text());
            emit sendEndPoint(ui->endPoint->text());
            resultScene->minTime(startNode,endNode);
            resultScene->show();
        }
        else
        {
            QMessageBox::critical(this, tr("查找"), tr("查找失败,请重新确认信息！"));
        }
    });
}
void WorkWindow::paintEvent(QPaintEvent *)                                      //绘制背景
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/source/BgForStart.jpg");
    painter.drawPixmap(0,0,1200,600,pix);
}
WorkWindow::~WorkWindow()
{
    qDebug() << "END";
    delete ui;
}
